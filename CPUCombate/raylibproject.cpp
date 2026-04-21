/*
 *  Archon-style 1v1 Combat — CPU AI Demo
 *  ======================================
 *  Controls:  WASD / Arrow keys  — move
 *             Space / Enter      — fire
 *             R                  — restart after match ends
 *
 *  All AI thresholds are derived from arena / piece geometry so the behaviour
 *  scales correctly regardless of window size.  The only true constants at
 *  file-scope are the window dimensions and FPS target.
 */

#include "raylib.h"
#include <cmath>
#include <cstdlib>   // rand

static constexpr int MAX_PROJS = 64;  // fixed pool — avoids debug-CRT dependency

// ── Minimal global constants ──────────────────────────────────────────────────
static constexpr int WIN_W = 800;
static constexpr int WIN_H = 620;
static constexpr int FPS   = 60;

// ─────────────────────────────────────────────────────────────────────────────
//  Vec2 — tiny 2-D vector, only the operations we actually use
// ─────────────────────────────────────────────────────────────────────────────
struct Vec2 {
    float x = 0, y = 0;

    Vec2 operator+(Vec2 o)  const { return {x+o.x, y+o.y}; }
    Vec2 operator-(Vec2 o)  const { return {x-o.x, y-o.y}; }
    Vec2 operator*(float s) const { return {x*s,   y*s};   }

    float dot  (Vec2 o) const { return x*o.x + y*o.y; }
    // signed magnitude of 2-D "cross product" (z-component of 3-D cross)
    float cross(Vec2 o) const { return x*o.y - y*o.x; }
    float len  ()       const { return sqrtf(x*x + y*y); }
    Vec2  norm ()       const { float d=len(); return d>0 ? Vec2{x/d,y/d} : Vec2{1,0}; }
    // 90-degree CCW perpendicular
    Vec2  perp ()       const { return {-y, x}; }
};

// ─────────────────────────────────────────────────────────────────────────────
//  PieceStats — all numbers come from the caller, nothing hardcoded here
// ─────────────────────────────────────────────────────────────────────────────
struct PieceStats {
    float speed;       // pixels per frame
    float projSpeed;   // pixels per frame
    float projRange;   // pixels before projectile expires
    float fireRate;    // frames between shots
    int   maxHealth;
    Color color;
};

// ─────────────────────────────────────────────────────────────────────────────
//  Projectile
// ─────────────────────────────────────────────────────────────────────────────
struct Projectile {
    Vec2  pos, vel;
    float rangeLeft, radius;
    Color color;
    bool  fromPlayer;   // true = player shot it, false = CPU shot it
};

// ─────────────────────────────────────────────────────────────────────────────
//  Piece
// ─────────────────────────────────────────────────────────────────────────────
struct Piece {
    Vec2       pos;
    float      radius;
    PieceStats stats;
    int        health;
    float      cooldown;   // frames until next allowed shot
    Vec2       face;       // current facing / firing direction
    bool       isPlayer;

    // Move in direction `dir` (should be normalised), clamped to arena.
    void move(Vec2 dir, Rectangle arena) {
        pos.x = fmaxf(arena.x + radius,
                fminf(arena.x + arena.width  - radius, pos.x + dir.x * stats.speed));
        pos.y = fmaxf(arena.y + radius,
                fminf(arena.y + arena.height - radius, pos.y + dir.y * stats.speed));
    }

    // Returns true and fills `out` if the shot fires this frame.
    bool tryFire(Projectile& out, float projRadius) {
        if (cooldown > 0) return false;
        cooldown     = stats.fireRate;
        Vec2  fd     = face.norm();
        float offset = radius + projRadius + 1.0f;
        out = {
            {pos.x + fd.x * offset, pos.y + fd.y * offset},
            {fd.x * stats.projSpeed, fd.y * stats.projSpeed},
            stats.projRange, projRadius,
            stats.color, isPlayer
        };
        return true;
    }

    void tick() { if (cooldown > 0) cooldown -= 1.0f; }
};

// ─────────────────────────────────────────────────────────────────────────────
//  CombatAI — finite-state-machine controller for the CPU piece
//
//  States
//  ------
//  APPROACH  Close the gap while weaving left/right (harder to hit).
//  ORBIT     Strafe at optimal range; reverse orbit direction periodically.
//  DODGE     Step perpendicular to an incoming player projectile.
//
//  Firing
//  ------
//  In every state the AI evaluates its current aim each frame and fires
//  whenever the facing direction is within ~±20° of the enemy, with a small
//  sinusoidal wobble on the threshold to avoid pixel-perfect shooting.
//
//  Reaction delay
//  --------------
//  The decision is recomputed every `reactionDelay` frames instead of every
//  frame, giving the AI a human-like ~120 ms response time.
//
//  Portability
//  -----------
//  Every threshold is derived from `arena` dimensions or `me.radius`.
//  No magic pixel values live inside this struct.
// ─────────────────────────────────────────────────────────────────────────────
struct CombatAI {

    enum State { APPROACH, ORBIT, DODGE };

    struct Action { float dx, dy; bool fire; };

    // ── References to the two combatants and arena ────────────────────────────
    Piece&    me;
    Piece&    enemy;
    Rectangle arena;
    float     projRadius;   // used when choosing a dodge side

    // ── Derived thresholds ────────────────────────────────────────────────────
    float optimalRange;       // target distance from enemy
    float approachMargin;     // hysteresis band around optimalRange
    float dodgeCrossLimit;    // max miss-distance to still trigger a dodge
    float aimDotThreshold;    // cos(angle) threshold to allow firing

    // ── State ─────────────────────────────────────────────────────────────────
    State state;
    float orbitDir;           // +1 = CCW, -1 = CW
    float orbitTimer;
    float orbitSwitch;        // frames between orbit direction flips

    // ── Reaction-delay buffer ─────────────────────────────────────────────────
    Action pendingAction;
    float  pendingTTL;
    float  reactionDelay;

    // ── Constructor ───────────────────────────────────────────────────────────
    CombatAI(Piece& me, Piece& enemy, Rectangle arena, float projRadius)
        : me(me), enemy(enemy), arena(arena), projRadius(projRadius),
          state(APPROACH), orbitDir(1.0f), orbitTimer(0.0f),
          pendingAction{0,0,false}, pendingTTL(0.0f)
    {
        float shortSide   = fminf(arena.width, arena.height);
        optimalRange      = shortSide * 0.38f;
        approachMargin    = optimalRange * 0.25f;
        dodgeCrossLimit   = me.radius * 3.0f;
        aimDotThreshold   = 0.93f;
        // Random first orbit-flip interval so two AIs don't sync
        orbitSwitch       = (float)FPS * (1.8f + (rand() % 100) / 72.0f);
        reactionDelay     = (float)FPS * 0.12f;  // ~7 frames ≈ 120 ms
    }

    // ── Main update: call once per frame ──────────────────────────────────────
    Action update(const Projectile* projs, int projCount) {
        if (pendingTTL > 0) { pendingTTL--; return pendingAction; }
        pendingAction = decide(projs, projCount);
        pendingTTL    = reactionDelay;
        return pendingAction;
    }

private:

    // ── Core decision logic ───────────────────────────────────────────────────
    Action decide(const Projectile* projs, int projCount) {
        Vec2  toEnemy = enemy.pos - me.pos;
        float dist    = toEnemy.len();
        Vec2  n       = toEnemy.norm();       // unit vector toward enemy

        // 1. Check for incoming threats
        float ddx = 0, ddy = 0;
        bool  mustDodge = findDodge(projs, projCount, ddx, ddy);

        // 2. State transition
        if (mustDodge)
            state = DODGE;
        else if (dist > optimalRange + approachMargin)
            state = APPROACH;
        else
            state = ORBIT;

        // 3. Compute movement direction
        float dx = 0, dy = 0;

        if (state == DODGE) {
            dx = ddx; dy = ddy;

        } else if (state == APPROACH) {
            // Weave sideways while closing in
            Vec2  perp  = n.perp();
            float weave = sinf((float)GetTime() * 2.5f) * 0.45f;
            Vec2  dir   = Vec2{n.x + perp.x*weave, n.y + perp.y*weave}.norm();
            dx = dir.x; dy = dir.y;

        } else { // ORBIT
            // Tangential + radial correction
            Vec2  perp = {-n.y * orbitDir, n.x * orbitDir};
            float err  = (dist - optimalRange) / fmaxf(optimalRange * 0.3f, 1.0f);
            err = fmaxf(-1.0f, fminf(1.0f, err));
            Vec2 dir = Vec2{perp.x*0.85f + n.x*err*0.45f,
                            perp.y*0.85f + n.y*err*0.45f}.norm();
            dx = dir.x; dy = dir.y;

            // Periodically flip orbit direction to be less predictable
            orbitTimer++;
            if (orbitTimer >= orbitSwitch) {
                orbitDir   = -orbitDir;
                orbitTimer = 0;
                orbitSwitch = (float)FPS * (1.5f + (rand() % 100) / 49.0f);
            }
        }

        // 4. Nudge away from arena walls
        dx = nudgeAxis(dx, me.pos.x, arena.x, arena.x + arena.width,  me.radius * 3.0f);
        dy = nudgeAxis(dy, me.pos.y, arena.y, arena.y + arena.height, me.radius * 3.0f);

        // 5. Always face the enemy (+ tiny sinusoidal jitter → imperfect aim)
        float t      = (float)GetTime();
        float jitter = sinf(t * 13.7f) * 0.04f;
        float angle  = atan2f(n.y, n.x) + jitter;
        me.face      = {cosf(angle), sinf(angle)};

        // 6. Fire when aim is good enough
        float dot    = me.face.norm().dot(n);
        float thresh = aimDotThreshold + sinf(t * 7.3f) * 0.03f;
        bool  fire   = (dot >= thresh);

        return {dx, dy, fire};
    }

    // ── Projectile threat detection ───────────────────────────────────────────
    // Returns true and sets (outDx, outDy) when a player projectile is on a
    // collision course.  Chooses the perpendicular direction that keeps us
    // further from the walls.
    bool findDodge(const Projectile* projs, int projCount,
                   float& outDx, float& outDy)
    {
        for (int i = 0; i < projCount; i++) {
            const Projectile& p = projs[i];
            if (!p.fromPlayer) continue;          // only dodge player shots

            Vec2  toMe = me.pos - p.pos;
            Vec2  pn   = Vec2{p.vel.x, p.vel.y}.norm();
            float dot  = toMe.dot(pn);
            if (dot < 0) continue;                // projectile already past us

            // Perpendicular distance from our centre to the projectile's path
            float cross = fabsf(toMe.cross(pn));
            if (cross > dodgeCrossLimit) continue; // will miss anyway

            // Threatened — pick the perpendicular that keeps us from walls
            Vec2  d1 = {-pn.y,  pn.x};
            Vec2  d2 = { pn.y, -pn.x};
            float s1 = wallScore(me.pos.x + d1.x * me.stats.speed * 4,
                                  me.pos.y + d1.y * me.stats.speed * 4);
            float s2 = wallScore(me.pos.x + d2.x * me.stats.speed * 4,
                                  me.pos.y + d2.y * me.stats.speed * 4);
            Vec2 best = (s1 >= s2) ? d1 : d2;
            outDx = best.x; outDy = best.y;
            return true;
        }
        return false;
    }

    // ── Helpers ───────────────────────────────────────────────────────────────

    // Distance from (x,y) to the nearest wall — higher is better.
    float wallScore(float x, float y) const {
        return fminf(fminf(x - arena.x,  arena.x + arena.width  - x),
                     fminf(y - arena.y,  arena.y + arena.height - y));
    }

    // Flip velocity component when too close to a wall edge.
    static float nudgeAxis(float v, float pos, float lo, float hi, float margin) {
        if (pos - lo < margin) return  fabsf(v);
        if (hi - pos < margin) return -fabsf(v);
        return v;
    }
};

// ─────────────────────────────────────────────────────────────────────────────
//  UI helpers
// ─────────────────────────────────────────────────────────────────────────────
static void DrawHpBar(float x, float y, float w, float h,
                      int hp, int maxHp, Color col, const char* label)
{
    DrawRectangle((int)x, (int)y, (int)w, (int)h, {50,50,50,255});
    if (maxHp > 0)
        DrawRectangle((int)x, (int)y, (int)(w * hp / maxHp), (int)h, col);
    DrawRectangleLinesEx({x, y, w, h}, 1, {180,180,180,255});
    DrawText(TextFormat("%s  %d/%d", label, hp, maxHp),
             (int)x + 6, (int)(y + h/2) - 9, 18, {230,230,230,255});
}

static void DrawPiece(const Piece& pc) {
    DrawCircleV({pc.pos.x, pc.pos.y}, pc.radius, pc.stats.color);
    Vec2 fd = pc.face.norm();
    DrawLineV({pc.pos.x, pc.pos.y},
              {pc.pos.x + fd.x * pc.radius * 1.6f,
               pc.pos.y + fd.y * pc.radius * 1.6f},
              WHITE);
}

// ─────────────────────────────────────────────────────────────────────────────
int main()
{
    InitWindow(WIN_W, WIN_H, "Archon Combat — CPU AI");
    SetTargetFPS(FPS);

    // ── Layout (derived from window size) ─────────────────────────────────────
    const float PAD   = 30.0f;
    const float HUD_H = 55.0f;
    Rectangle   arena = {PAD, PAD, WIN_W - 2*PAD, WIN_H - 2*PAD - HUD_H};

    // ── Piece and projectile sizes (derived from arena) ───────────────────────
    float pieceR = fmaxf(arena.width, arena.height) / 32.0f;
    float projR  = fmaxf(2.0f, pieceR / 3.0f);

    // ── Stats (all relative to arena dimensions) ──────────────────────────────
    float baseSpeed     = arena.width / 175.0f;
    float baseProjSpd   = arena.width /  65.0f;
    float baseProjRange = arena.width *   0.62f;
    float baseFireRate  = (float)FPS  *   0.65f;   // frames between shots

    PieceStats playerStats = {baseSpeed,
                               baseProjSpd, baseProjRange, baseFireRate,
                               3, SKYBLUE};
    PieceStats cpuStats    = {baseSpeed * 0.95f,            // slightly slower
                               baseProjSpd, baseProjRange,
                               baseFireRate * 1.1f,          // slightly slower fire
                               3, {220,70,50,255}};

    // ── Factory lambdas so we can reset cleanly ───────────────────────────────
    auto makePlayer = [&]() -> Piece {
        return {{arena.x + arena.width * 0.15f, arena.y + arena.height * 0.5f},
                pieceR, playerStats, playerStats.maxHealth, 0.0f, {1,0}, true};
    };
    auto makeCPU = [&]() -> Piece {
        return {{arena.x + arena.width * 0.85f, arena.y + arena.height * 0.5f},
                pieceR, cpuStats, cpuStats.maxHealth, 0.0f, {-1,0}, false};
    };

    Piece player = makePlayer();
    Piece cpu    = makeCPU();

    // CombatAI holds non-const references, so we manage it on the heap to
    // allow clean re-initialisation on restart.
    CombatAI* ai = new CombatAI(cpu, player, arena, projR);

    Projectile  projectiles[MAX_PROJS];
    int         projCount = 0;
    bool        gameOver  = false;
    const char* winner    = nullptr;

    // ── Main loop ─────────────────────────────────────────────────────────────
    while (!WindowShouldClose()) {

        // ── Restart ───────────────────────────────────────────────────────────
        if (IsKeyPressed(KEY_R)) {
            player    = makePlayer();
            cpu       = makeCPU();
            delete ai;
            ai        = new CombatAI(cpu, player, arena, projR);
            projCount = 0;
            gameOver  = false;
            winner    = nullptr;
        }

        // ── Update ────────────────────────────────────────────────────────────
        if (!gameOver) {

            // -- Player input --------------------------------------------------
            float pdx = (float)((IsKeyDown(KEY_RIGHT)||IsKeyDown(KEY_D)) -
                                 (IsKeyDown(KEY_LEFT) ||IsKeyDown(KEY_A)));
            float pdy = (float)((IsKeyDown(KEY_DOWN) ||IsKeyDown(KEY_S)) -
                                 (IsKeyDown(KEY_UP)   ||IsKeyDown(KEY_W)));

            if (pdx != 0 || pdy != 0) {
                Vec2 dir = Vec2{pdx, pdy}.norm();
                player.face = dir;
                player.move(dir, arena);
            }

            if (IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_ENTER)) {
                Projectile proj;
                if (player.tryFire(proj, projR) && projCount < MAX_PROJS)
                    projectiles[projCount++] = proj;
            }

            // -- CPU AI --------------------------------------------------------
            CombatAI::Action act = ai->update(projectiles, projCount);
            Vec2 cpuDir = Vec2{act.dx, act.dy};
            if (cpuDir.len() > 0.01f)
                cpu.move(cpuDir.norm(), arena);

            if (act.fire) {
                Projectile proj;
                if (cpu.tryFire(proj, projR) && projCount < MAX_PROJS)
                    projectiles[projCount++] = proj;
            }

            // -- Tick cooldowns ------------------------------------------------
            player.tick();
            cpu.tick();

            // -- Advance projectiles, resolve hits ----------------------------
            // Compact in place: swap dead projectile with last, decrement count.
            for (int i = 0; i < projCount; ) {
                Projectile& p = projectiles[i];
                p.pos.x    += p.vel.x;
                p.pos.y    += p.vel.y;
                p.rangeLeft -= Vec2{p.vel.x, p.vel.y}.len();

                bool remove = (p.rangeLeft <= 0) ||
                              (!CheckCollisionPointRec({p.pos.x, p.pos.y}, arena));

                if (!remove) {
                    if (p.fromPlayer &&
                        CheckCollisionCircles({p.pos.x, p.pos.y}, p.radius,
                                              {cpu.pos.x, cpu.pos.y}, cpu.radius)) {
                        cpu.health--;
                        remove = true;
                    }
                    if (!remove && !p.fromPlayer &&
                        CheckCollisionCircles({p.pos.x, p.pos.y}, p.radius,
                                              {player.pos.x, player.pos.y}, player.radius)) {
                        player.health--;
                        remove = true;
                    }
                }

                if (remove)
                    projectiles[i] = projectiles[--projCount]; // swap with last
                else
                    i++;
            }

            // -- Win condition -------------------------------------------------
            if (player.health <= 0) { gameOver = true; winner = "CPU"; }
            if (cpu.health    <= 0) { gameOver = true; winner = "PLAYER"; }
        }

        // ── Draw ──────────────────────────────────────────────────────────────
        BeginDrawing();
        ClearBackground({18, 18, 28, 255});

        // Arena background + faint grid
        DrawRectangleRec(arena, {38, 42, 58, 255});
        int stepX = (int)(arena.width  / 8);
        int stepY = (int)(arena.height / 8);
        for (int gx=(int)arena.x+stepX; gx < arena.x+arena.width;  gx+=stepX)
            DrawLine(gx,(int)arena.y, gx,(int)(arena.y+arena.height), {50,55,75,255});
        for (int gy=(int)arena.y+stepY; gy < arena.y+arena.height; gy+=stepY)
            DrawLine((int)arena.x,gy, (int)(arena.x+arena.width),gy,  {50,55,75,255});
        DrawRectangleLinesEx(arena, 2, {90,95,130,255});

        // Projectiles
        for (int i = 0; i < projCount; i++)
            DrawCircleV({projectiles[i].pos.x, projectiles[i].pos.y},
                         projectiles[i].radius, projectiles[i].color);

        // Pieces
        DrawPiece(player);
        DrawPiece(cpu);

        // HUD — health bars
        float barW = arena.width / 2.0f - 8.0f;
        float barY = arena.y + arena.height + (HUD_H - 24.0f) / 2.0f;
        DrawHpBar(arena.x,                         barY, barW, 24,
                  player.health, player.stats.maxHealth, SKYBLUE,        "PLAYER");
        DrawHpBar(arena.x + arena.width/2.0f + 8,  barY, barW, 24,
                  cpu.health,    cpu.stats.maxHealth,    {220,70,50,255}, "CPU");

        // Controls hint
        DrawText("WASD/Arrows: move  |  Space/Enter: fire  |  R: restart",
                 (int)arena.x, (int)(WIN_H - 22), 15, {100,100,120,255});

        // Game-over overlay
        if (gameOver) {
            DrawRectangle(0, 0, WIN_W, WIN_H, {0,0,0,120});
            const char* msg = TextFormat("%s WINS!", winner);
            int mw = MeasureText(msg, 72);
            DrawText(msg, WIN_W/2 - mw/2, WIN_H/2 - 50, 72, GOLD);
            const char* sub = "Press  R  to play again";
            DrawText(sub, WIN_W/2 - MeasureText(sub,26)/2, WIN_H/2 + 42, 26, LIGHTGRAY);
        }

        EndDrawing();
    }

    delete ai;
    CloseWindow();
    return 0;
}
