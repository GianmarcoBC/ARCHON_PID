#define _CRT_SECURE_NO_WARNINGS
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// ============================================================
//  CONFIGURATION — tweak these and everything adapts
// ============================================================
static const int   SW           = 900;
static const int   SH           = 970;
static const int   COLS         = 9;
static const int   ROWS         = 9;
static const int   CELL         = SW / COLS;    // 100 px
static const int   BOARD_Y      = 70;
static const int   FPS          = 60;
static const float LUM_CYCLE    = 16.0f;
static const float CPU_DELAY    = 0.9f;
static const int   PIECE_RADIUS = CELL / 2 - 6;
static const int   MAX_PIECES   = 40;           // max pieces total on board
static const int   MAX_MOVES    = 81;           // max valid moves per piece (9x9)

// ============================================================
//  COLORS
// ============================================================
static const Color COL_CELL_DARK   = {  55,  40,  25, 255 };
static const Color COL_CELL_LIGHT  = { 210, 190, 150, 255 };
static const Color COL_POWER_DARK  = { 110,  75,   0, 255 };
static const Color COL_POWER_LIGHT = { 255, 210,   0, 255 };
static const Color COL_SELECTED    = {  80, 210,  80, 200 };
static const Color COL_VALID_MOVE  = {  60, 140, 220, 160 };
static const Color COL_LIGHT_PIECE = { 240, 240, 230, 255 };
static const Color COL_DARK_PIECE  = {  35,  35,  65, 255 };
static const Color COL_UI_BG       = {  20,  20,  20, 255 };
static const Color COL_UI_TEXT     = { 220, 220, 220, 255 };
static const Color COL_LIGHT_TURN  = { 255, 255, 200, 255 };
static const Color COL_DARK_TURN   = { 120, 120, 220, 255 };

// ============================================================
//  ENUMS
// ============================================================
enum Side   { SIDE_NONE, SIDE_LIGHT, SIDE_DARK };
enum GState     { GS_MENU, GS_BOARD, GS_GAME_OVER };
enum TPhase     { TP_SELECT, TP_MOVE };
enum Difficulty { DIFF_EASY, DIFF_HARD };

enum PType {
    PT_NONE = 0,
    // Light (1-9)
    PT_WIZARD, PT_UNICORN, PT_ARCHER, PT_VALKYRIE,
    PT_DJINN,  PT_PHOENIX, PT_KNIGHT, PT_DWARF, PT_L_ELEM,
    // Dark (10-18)
    PT_SORCERESS, PT_BASILISK, PT_MANTICORE, PT_BANSHEE,
    PT_TROLL, PT_SHAPESHIFTER, PT_GOBLIN, PT_IMP, PT_D_ELEM,
    PT_COUNT
};

// ============================================================
//  PIECE STATS TABLE
// ============================================================
struct PStats {
    const char* abbrev;
    const char* name;
    float speed;
    float power;
    float range;
    float rate;
    int   moveRange;
};

static const PStats STATS[PT_COUNT] = {
 // abbrev  name            spd   pow    rng   rate  mv
    {"  ", "None",          0,    0,     0,    0,    0 },
    {"Wz", "Wizard",        3.0f, 30.0f, 200,  1.5f, 5 },
    {"Un", "Unicorn",       5.0f, 20.0f,  40,  1.0f, 3 },
    {"Ar", "Archer",        3.5f, 15.0f, 150,  1.2f, 3 },
    {"Vk", "Valkyrie",      4.0f, 25.0f,  50,  1.0f, 4 },
    {"Dj", "Djinn",         4.5f, 18.0f,  80,  1.3f, 4 },
    {"Ph", "Phoenix",       5.0f, 20.0f,  60,  1.1f, 5 },
    {"Kn", "Knight",        4.0f, 22.0f,  45,  1.0f, 3 },
    {"Dw", "Dwarf",         2.5f, 28.0f,  80,  1.4f, 2 },
    {"LE", "L.Elem",        3.0f, 25.0f,  70,  1.2f, 3 },
    {"Sc", "Sorceress",     3.0f, 30.0f, 200,  1.5f, 5 },
    {"Bs", "Basilisk",      3.5f, 20.0f,  40,  1.0f, 3 },
    {"Mn", "Manticore",     4.5f, 25.0f, 100,  1.2f, 4 },
    {"Bn", "Banshee",       4.0f, 20.0f,  80,  1.1f, 4 },
    {"Tr", "Troll",         2.5f, 30.0f,  40,  0.8f, 2 },
    {"Sf", "Shapeshftr",    4.0f, 22.0f,  60,  1.0f, 4 },
    {"Gb", "Goblin",        3.5f, 15.0f,  45,  1.0f, 2 },
    {"Im", "Imp",           3.0f, 18.0f,  70,  1.3f, 2 },
    {"DE", "D.Elem",        3.0f, 25.0f,  70,  1.2f, 3 },
};

// ============================================================
//  DATA STRUCTS
// ============================================================
struct Piece {
    PType type;
    Side  side;
    int   col, row;
    int   alive;    // 1 = alive, 0 = dead  (int avoids bool/STL issues)
};

struct Cell {
    float lum;
    int   isPower;
};

struct MoveOption {
    int col, row, score;
};

struct Game {
    Cell  board[ROWS][COLS];
    Piece pieces[MAX_PIECES];
    int   pieceCount;

    GState state;
    Side   turn;
    TPhase phase;
    int    selected;

    float  time;
    float  cpuTimer;
    int    cpuWaiting;

    int        combatAtk;
    int        combatDef;

    char       msg[160];
    Difficulty diff;
};

// ============================================================
//  HELPERS
// ============================================================
static Color lerpColor(Color a, Color b, float t) {
    Color c;
    c.r = (unsigned char)(a.r + (float)(b.r - a.r) * t);
    c.g = (unsigned char)(a.g + (float)(b.g - a.g) * t);
    c.b = (unsigned char)(a.b + (float)(b.b - a.b) * t);
    c.a = 255;
    return c;
}

static void drawTextCentered(const char* text, int cx, int cy, int size, Color col) {
    int w = MeasureText(text, size);
    DrawText(text, cx - w / 2, cy - size / 2, size, col);
}

static Side getPieceSide(PType t) {
    if (t >= PT_WIZARD    && t <= PT_L_ELEM) return SIDE_LIGHT;
    if (t >= PT_SORCERESS && t <= PT_D_ELEM) return SIDE_DARK;
    return SIDE_NONE;
}

static int isPowerSq(int c, int r) {
    return (c == 0        && r == 0       ) ||
           (c == COLS - 1 && r == 0       ) ||
           (c == 0        && r == ROWS - 1) ||
           (c == COLS - 1 && r == ROWS - 1) ||
           (c == COLS / 2 && r == ROWS / 2);
}

static int pieceAt(const Game* g, int col, int row) {
    for (int i = 0; i < g->pieceCount; i++) {
        if (g->pieces[i].alive && g->pieces[i].col == col && g->pieces[i].row == row)
            return i;
    }
    return -1;
}

static int isValidMove(const Game* g, int idx, int col, int row) {
    if (col < 0 || col >= COLS || row < 0 || row >= ROWS) return 0;
    const Piece* p  = &g->pieces[idx];
    int dc = abs(col - p->col);
    int dr = abs(row - p->row);
    int mr = STATS[p->type].moveRange;
    if (dc > mr || dr > mr) return 0;
    if (dc == 0 && dr == 0) return 0;
    int occ = pieceAt(g, col, row);
    if (occ != -1 && g->pieces[occ].side == p->side) return 0;
    return 1;
}

static int getValidMoves(const Game* g, int idx, MoveOption* out) {
    int count = 0;
    const Piece* p = &g->pieces[idx];
    int mr = STATS[p->type].moveRange;
    for (int dr = -mr; dr <= mr; dr++)
        for (int dc = -mr; dc <= mr; dc++) {
            int nc = p->col + dc, nr = p->row + dr;
            if (isValidMove(g, idx, nc, nr)) {
                out[count].col   = nc;
                out[count].row   = nr;
                out[count].score = 0;
                count++;
            }
        }
    return count;
}

// ============================================================
//  INIT
// ============================================================
static Piece makePiece(PType type, int col, int row) {
    Piece p;
    p.type  = type;
    p.side  = getPieceSide(type);
    p.col   = col;
    p.row   = row;
    p.alive = 1;
    return p;
}

static void addPiece(Game* g, PType type, int col, int row) {
    if (g->pieceCount < MAX_PIECES)
        g->pieces[g->pieceCount++] = makePiece(type, col, row);
}

static void initBoard(Game* g) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) {
            g->board[r][c].isPower = isPowerSq(c, r);
            g->board[r][c].lum    = ((c + r) % 2 == 0) ? 1.0f : 0.0f;
        }
}

// NOTE: starting positions are approximate — adjust to match original ARCHON layout.
static void initPieces(Game* g) {
    g->pieceCount = 0;

    // ---- Dark side (top) ----
    addPiece(g, PT_MANTICORE,   0, 0);
    addPiece(g, PT_GOBLIN,      1, 0);
    addPiece(g, PT_BANSHEE,     2, 0);
    addPiece(g, PT_BASILISK,    3, 0);
    addPiece(g, PT_SORCERESS,   4, 0);
    addPiece(g, PT_BASILISK,    5, 0);
    addPiece(g, PT_BANSHEE,     6, 0);
    addPiece(g, PT_GOBLIN,      7, 0);
    addPiece(g, PT_MANTICORE,   8, 0);
    addPiece(g, PT_TROLL,       0, 1);
    addPiece(g, PT_IMP,         1, 1);
    addPiece(g, PT_SHAPESHIFTER,4, 1);
    addPiece(g, PT_IMP,         7, 1);
    addPiece(g, PT_TROLL,       8, 1);
    addPiece(g, PT_D_ELEM,      2, 2);
    addPiece(g, PT_D_ELEM,      6, 2);
    addPiece(g, PT_D_ELEM,      3, 3);
    addPiece(g, PT_D_ELEM,      5, 3);

    // ---- Light side (bottom) ----
    addPiece(g, PT_L_ELEM,      3, 5);
    addPiece(g, PT_L_ELEM,      5, 5);
    addPiece(g, PT_L_ELEM,      2, 6);
    addPiece(g, PT_L_ELEM,      6, 6);
    addPiece(g, PT_DJINN,       0, 7);
    addPiece(g, PT_DWARF,       1, 7);
    addPiece(g, PT_PHOENIX,     4, 7);
    addPiece(g, PT_DWARF,       7, 7);
    addPiece(g, PT_DJINN,       8, 7);
    addPiece(g, PT_UNICORN,     0, 8);
    addPiece(g, PT_KNIGHT,      1, 8);
    addPiece(g, PT_ARCHER,      2, 8);
    addPiece(g, PT_VALKYRIE,    3, 8);
    addPiece(g, PT_WIZARD,      4, 8);
    addPiece(g, PT_VALKYRIE,    5, 8);
    addPiece(g, PT_ARCHER,      6, 8);
    addPiece(g, PT_KNIGHT,      7, 8);
    addPiece(g, PT_UNICORN,     8, 8);
}

static void initGame(Game* g) {
    g->state      = GS_MENU;
    g->diff       = DIFF_EASY;
    g->turn       = SIDE_LIGHT;
    g->phase      = TP_SELECT;
    g->selected   = -1;
    g->time       = 0.0f;
    g->cpuTimer   = 0.0f;
    g->cpuWaiting = 0;
    g->combatAtk  = -1;
    g->combatDef  = -1;
    strcpy(g->msg, "Select difficulty to begin");
    initBoard(g);
    initPieces(g);
}

// ============================================================
//  WIN CONDITION
// ============================================================
static Side checkWin(const Game* g) {
    int lightPower = 0, darkPower = 0;
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (g->board[r][c].isPower) {
                int idx = pieceAt(g, c, r);
                if (idx != -1) {
                    if (g->pieces[idx].side == SIDE_LIGHT) lightPower++;
                    else                                    darkPower++;
                }
            }
    if (lightPower == 5) return SIDE_LIGHT;
    if (darkPower  == 5) return SIDE_DARK;

    int hasLight = 0, hasDark = 0;
    for (int i = 0; i < g->pieceCount; i++) {
        if (!g->pieces[i].alive) continue;
        if (g->pieces[i].side == SIDE_LIGHT) hasLight = 1;
        if (g->pieces[i].side == SIDE_DARK)  hasDark  = 1;
    }
    if (!hasDark)  return SIDE_LIGHT;
    if (!hasLight) return SIDE_DARK;
    return SIDE_NONE;
}

// ============================================================
//  COMBAT STUB
// ============================================================
static void resolveCombatStub(Game* g) {
    Piece* atk = &g->pieces[g->combatAtk];
    Piece* def = &g->pieces[g->combatDef];

    float lum    = g->board[def->row][def->col].lum;
    float atkMod = (atk->side == SIDE_LIGHT) ? lum : (1.0f - lum);
    float defMod = (def->side == SIDE_LIGHT) ? lum : (1.0f - lum);

    float atkScore = STATS[atk->type].power * (0.7f + 0.6f * atkMod)
                   * (0.5f + 0.5f * ((float)rand() / RAND_MAX));
    float defScore = STATS[def->type].power * (0.7f + 0.6f * defMod)
                   * (0.5f + 0.5f * ((float)rand() / RAND_MAX));

    if (atkScore >= defScore) {
        def->alive = 0;
        atk->col   = def->col;
        atk->row   = def->row;
        strcpy(g->msg, TextFormat("%s defeated %s!", STATS[atk->type].name, STATS[def->type].name));
    } else {
        atk->alive = 0;
        strcpy(g->msg, TextFormat("%s held off %s!", STATS[def->type].name, STATS[atk->type].name));
    }

    g->combatAtk = -1;
    g->combatDef = -1;
}

// ============================================================
//  CPU AI
// ============================================================
static void doCpuMove(Game* g) {
    // Flat array that will hold every possible move for every Dark piece this turn
    MoveOption allMoves[MAX_PIECES * MAX_MOVES];
    // Parallel array: allMoves[i] belongs to the piece with index movePiece[i]
    int        movePiece[MAX_PIECES * MAX_MOVES];
    // How many total moves we have found so far across all Dark pieces
    int        totalMoves = 0;
    // Tracks the highest score seen; starts impossibly low so the first real score always wins
    int        bestScore  = -99999;

    // --- PHASE 1: generate and score every move for every Dark piece ---
    for (int i = 0; i < g->pieceCount; i++) {
        const Piece* p = &g->pieces[i];
        // Skip dead pieces and any piece that belongs to the player (Light side)
        if (!p->alive || p->side != SIDE_DARK) continue;

        // Temporary buffer for this single piece's valid moves
        MoveOption moves[MAX_MOVES];
        // Fill 'moves' with all squares this piece can legally reach; returns how many
        int count = getValidMoves(g, i, moves);

        // Score each candidate move for this piece
        for (int m = 0; m < count; m++) {
            int score = 0;

            // Check whether an enemy piece is already standing on the destination square
            int occ = pieceAt(g, moves[m].col, moves[m].row);
            // Landing on an enemy is the most valuable thing the CPU can do (+100)
            if (occ != -1) score += 100;
            // Occupying a power square is the second priority (+50); holding all 5 wins the game
            if (g->board[moves[m].row][moves[m].col].isPower) score += 50;
            // Manhattan distance from this destination to the board center (col 4, row 4)
            int centerDist = abs(moves[m].col - COLS/2) + abs(moves[m].row - ROWS/2);
            // Reward moves that are closer to the center; max bonus is COLS+ROWS (18), min is 0
            score += (COLS + ROWS - centerDist);

            // Store the final score inside the move option itself
            moves[m].score = score;
            // Update the global best score if this move beats everything seen so far
            if (score > bestScore) bestScore = score;

            // Append this move and its owning piece index to the global flat lists
            allMoves[totalMoves]  = moves[m];
            movePiece[totalMoves] = i;
            totalMoves++;
        }
    }

    // --- EDGE CASE: the CPU has literally no legal move ---
    if (totalMoves == 0) {
        strcpy(g->msg, "CPU has no moves!");
        g->turn       = SIDE_LIGHT;   // give the turn back to the player
        g->phase      = TP_SELECT;    // reset to piece-selection phase
        g->cpuWaiting = 0;            // clear the "CPU is thinking" flag
        return;
    }

    // --- PHASE 2: collect all moves tied at the best score, then pick one at random ---
    // Using an index list instead of copying MoveOption structs to save memory
    int   candidates[MAX_PIECES * MAX_MOVES];
    int   candCount = 0;
    for (int i = 0; i < totalMoves; i++)
        // Include every move that matches the top score (ties are common at low complexity)
        if (allMoves[i].score >= bestScore)
            candidates[candCount++] = i;

    // Pick a random index from the candidates list to avoid totally predictable play
    int    chosen    = candidates[rand() % candCount];
    // Look up which piece owns that chosen move
    int    pieceIdx  = movePiece[chosen];
    // Destination column and row for the chosen move
    int    nc        = allMoves[chosen].col;
    int    nr        = allMoves[chosen].row;
    // Pointer to the actual piece that will move
    Piece* cp        = &g->pieces[pieceIdx];
    // Check if a player piece is currently sitting on the destination
    int    occ       = pieceAt(g, nc, nr);

    // --- PHASE 3: execute the chosen move ---
    if (occ != -1 && g->pieces[occ].side == SIDE_LIGHT) {
        // Destination is occupied by a Light piece → initiate combat instead of a plain move
        g->combatAtk = pieceIdx;  // attacker is the CPU piece
        g->combatDef = occ;       // defender is the player piece
        strcpy(g->msg, TextFormat("CPU: %s attacks %s!", STATS[cp->type].name, STATS[g->pieces[occ].type].name));
        // Resolve the fight immediately (stub: dice-roll with luminance modifier)
        resolveCombatStub(g);
    } else {
        // Destination is empty → just slide the piece there
        cp->col = nc;
        cp->row = nr;
        strcpy(g->msg, TextFormat("CPU moved %s to (%d,%d)", STATS[cp->type].name, nc, nr));
    }

    // --- PHASE 4: hand the turn back to the player ---
    g->turn       = SIDE_LIGHT;   // it is now the player's turn
    g->phase      = TP_SELECT;    // player must start by selecting a piece
    g->cpuWaiting = 0;            // clear the delay flag so the timer stops
    g->selected   = -1;           // make sure nothing is highlighted as selected

    // --- PHASE 5: check if the CPU's move ended the game ---
    Side winner = checkWin(g);
    if (winner != SIDE_NONE) {
        // Someone has won — freeze the game and display the result
        g->state = GS_GAME_OVER;
        strcpy(g->msg, winner == SIDE_LIGHT ? "LIGHT WINS!" : "DARK WINS!");
    } else {
        // Game continues — append "Your turn" to whatever combat/move message is already set
        char tmp[160];
        strcpy(tmp, g->msg);                                  // save the existing message
        strcpy(g->msg, TextFormat("%s  |  Your turn", tmp)); // append the turn prompt
    }
}

// ============================================================
//  HARD AI — helpers
// ============================================================

// Apply a move for minimax simulation using deterministic combat
// (no randomness — higher power always wins; attacker wins on tie).
static void applySimMove(Game* g, int pieceIdx, int nc, int nr) {
    Piece* p   = &g->pieces[pieceIdx];
    int    occ = pieceAt(g, nc, nr);
    if (occ != -1 && g->pieces[occ].side != p->side) {
        // Deterministic outcome: attacker wins if power >=, else attacker dies
        if (STATS[p->type].power >= STATS[g->pieces[occ].type].power) {
            g->pieces[occ].alive = 0;
            p->col = nc;
            p->row = nr;
        } else {
            p->alive = 0;
        }
    } else {
        p->col = nc;
        p->row = nr;
    }
}

// Static board evaluation from Dark's perspective (positive = Dark winning).
// Called at minimax leaf nodes.
static int evaluateBoard(const Game* g) {
    // Power square positions for proximity scoring
    static const int pwrC[5] = { 0, 8, 0, 8, 4 };
    static const int pwrR[5] = { 0, 0, 8, 8, 4 };
    int score = 0;

    // 1. Piece value: each alive piece contributes its power stat to its side's total
    for (int i = 0; i < g->pieceCount; i++) {
        const Piece* p = &g->pieces[i];
        if (!p->alive) continue;
        int val = (int)(STATS[p->type].power * 3);
        if (p->side == SIDE_DARK) score += val;
        else                       score -= val;
    }

    // 2. Power square control — holding all 5 wins the game, so these are worth the most
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            if (g->board[r][c].isPower) {
                int idx = pieceAt(g, c, r);
                if (idx != -1) {
                    if (g->pieces[idx].side == SIDE_DARK) score += 200;
                    else                                    score -= 200;
                }
            }

    // 3. Positional pressure: reward pieces that are close to unoccupied power squares
    for (int ps = 0; ps < 5; ps++) {
        if (pieceAt(g, pwrC[ps], pwrR[ps]) != -1) continue; // already occupied, skip
        for (int i = 0; i < g->pieceCount; i++) {
            const Piece* p = &g->pieces[i];
            if (!p->alive) continue;
            int dist      = abs(p->col - pwrC[ps]) + abs(p->row - pwrR[ps]);
            int proximity = (COLS + ROWS) - dist; // higher = closer to this power square
            if (p->side == SIDE_DARK) score += proximity;
            else                       score -= proximity;
        }
    }

    // Mobility omitted from leaf evaluation — calling getValidMoves (which calls pieceAt)
    // for every piece at every leaf node makes the search exponentially slower.

    return score;
}

// Minimax with alpha-beta pruning.
// 'g' is passed by value so each recursive call works on its own copy of the board.
// maximizing = 1 means it's Dark's turn (CPU trying to maximise score).
// maximizing = 0 means it's Light's turn (player trying to minimise score).
static int minimax(Game g, int depth, int alpha, int beta, int maximizing) {
    // Terminal checks — winning fast is better, so add depth bonus
    Side winner = checkWin(&g);
    if (winner == SIDE_DARK)  return  100000 + depth; // Dark wins: the sooner the better
    if (winner == SIDE_LIGHT) return -100000 - depth; // Light wins: the sooner the worse
    if (depth == 0)           return evaluateBoard(&g); // leaf node: static evaluation

    Side side = maximizing ? SIDE_DARK : SIDE_LIGHT;
    int  best  = maximizing ? -99999 : 99999;
    int  done  = 0; // set to 1 on alpha-beta cutoff to break both loops cleanly

    for (int i = 0; i < g.pieceCount && !done; i++) {
        if (!g.pieces[i].alive || g.pieces[i].side != side) continue;
        MoveOption moves[MAX_MOVES];
        int count = getValidMoves(&g, i, moves);
        for (int m = 0; m < count && !done; m++) {
            Game sim = g; // copy board state for this branch
            applySimMove(&sim, i, moves[m].col, moves[m].row);
            int eval = minimax(sim, depth - 1, alpha, beta, !maximizing);
            if (maximizing) {
                if (eval > best)  best  = eval;
                if (eval > alpha) alpha = eval;
            } else {
                if (eval < best)  best = eval;
                if (eval < beta)  beta = eval;
            }
            if (beta <= alpha) done = 1; // cutoff: opponent won't allow this branch
        }
    }

    // If no moves found, fall back to static evaluation (stalemate)
    return (best == (maximizing ? -99999 : 99999)) ? evaluateBoard(&g) : best;
}

// Hard-difficulty CPU move using Minimax (3 plies total: dark move + light response + dark response).
// Uses deterministic combat for simulation; actual combat still uses resolveCombatStub.
// Reduce MINIMAX_DEPTH if the CPU pause feels too long.
// Depth 1 = CPU looks 2 plies ahead: its own move + opponent's best reply.
// Each extra depth multiplies search time by ~branching_factor (~200 moves).
// Depth 2+ will freeze the game unless evaluateBoard is made much cheaper first.
static const int MINIMAX_DEPTH = 1;

static void doCpuMoveHard(Game* g) {
    int bestScore = -99999;
    int bestPiece = -1;
    int bestCol   = -1;
    int bestRow   = -1;

    // Try every dark piece and every legal move; keep the one Minimax ranks highest
    for (int i = 0; i < g->pieceCount; i++) {
        if (!g->pieces[i].alive || g->pieces[i].side != SIDE_DARK) continue;
        MoveOption moves[MAX_MOVES];
        int count = getValidMoves(g, i, moves);
        for (int m = 0; m < count; m++) {
            Game sim = *g;                                           // scratch copy of game
            applySimMove(&sim, i, moves[m].col, moves[m].row);      // apply this dark move
            // After our move it's Light's turn (minimizing), search MINIMAX_DEPTH more plies
            int score = minimax(sim, MINIMAX_DEPTH, -99999, 99999, 0);
            if (score > bestScore) {
                bestScore = score;
                bestPiece = i;
                bestCol   = moves[m].col;
                bestRow   = moves[m].row;
            }
        }
    }

    if (bestPiece == -1) {
        strcpy(g->msg, "CPU has no moves!");
        g->turn       = SIDE_LIGHT;
        g->phase      = TP_SELECT;
        g->cpuWaiting = 0;
        return;
    }

    // Execute the best move found (same logic as easy mode)
    Piece* cp  = &g->pieces[bestPiece];
    int    occ = pieceAt(g, bestCol, bestRow);

    if (occ != -1 && g->pieces[occ].side == SIDE_LIGHT) {
        g->combatAtk = bestPiece;
        g->combatDef = occ;
        strcpy(g->msg, TextFormat("CPU: %s attacks %s!", STATS[cp->type].name, STATS[g->pieces[occ].type].name));
        resolveCombatStub(g);
    } else {
        cp->col = bestCol;
        cp->row = bestRow;
        strcpy(g->msg, TextFormat("CPU moved %s to (%d,%d)", STATS[cp->type].name, bestCol, bestRow));
    }

    g->turn       = SIDE_LIGHT;
    g->phase      = TP_SELECT;
    g->cpuWaiting = 0;
    g->selected   = -1;

    Side winner = checkWin(g);
    if (winner != SIDE_NONE) {
        g->state = GS_GAME_OVER;
        strcpy(g->msg, winner == SIDE_LIGHT ? "LIGHT WINS!" : "DARK WINS!");
    } else {
        char tmp[160];
        strcpy(tmp, g->msg);
        strcpy(g->msg, TextFormat("%s  |  Your turn", tmp));
    }
}

// ============================================================
//  UPDATE
// ============================================================
static void updateLuminance(Game* g) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) {
            float phase         = (float)(c + r) / (float)(COLS + ROWS);
            float t             = g->time / LUM_CYCLE + phase;
            g->board[r][c].lum  = 0.5f + 0.5f * sinf(t * 2.0f * 3.14159f);
        }
}

static void handlePlayerInput(Game* g) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;

    Vector2 mouse = GetMousePosition();
    int mc = (int)(mouse.x / CELL);
    int mr = (int)((mouse.y - BOARD_Y) / CELL);
    if (mc < 0 || mc >= COLS || mr < 0 || mr >= ROWS) return;

    if (g->phase == TP_SELECT) {
        int idx = pieceAt(g, mc, mr);
        if (idx != -1 && g->pieces[idx].side == SIDE_LIGHT) {
            g->selected = idx;
            g->phase    = TP_MOVE;
            strcpy(g->msg, TextFormat("Selected %s - click destination", STATS[g->pieces[idx].type].name));
        }
    } else {
        if (!isValidMove(g, g->selected, mc, mr)) {
            int idx = pieceAt(g, mc, mr);
            if (idx != -1 && g->pieces[idx].side == SIDE_LIGHT) {
                g->selected = idx;
                strcpy(g->msg, TextFormat("Selected %s - click destination", STATS[g->pieces[idx].type].name));
            } else {
                g->selected = -1;
                g->phase    = TP_SELECT;
                strcpy(g->msg, "Cancelled - select a piece");
            }
            return;
        }

        Piece* mp  = &g->pieces[g->selected];
        int    occ = pieceAt(g, mc, mr);

        if (occ != -1 && g->pieces[occ].side == SIDE_DARK) {
            g->combatAtk = g->selected;
            g->combatDef = occ;
            strcpy(g->msg, TextFormat("You: %s attacks %s!", STATS[mp->type].name, STATS[g->pieces[occ].type].name));
            resolveCombatStub(g);
        } else {
            mp->col = mc;
            mp->row = mr;
            strcpy(g->msg, TextFormat("Moved %s to (%d,%d)", STATS[mp->type].name, mc, mr));
        }

        g->selected = -1;
        g->phase    = TP_SELECT;

        Side winner = checkWin(g);
        if (winner != SIDE_NONE) {
            g->state = GS_GAME_OVER;
            strcpy(g->msg, winner == SIDE_LIGHT ? "LIGHT WINS!" : "DARK WINS!");
            return;
        }

        g->turn       = SIDE_DARK;
        g->cpuWaiting = 1;
        g->cpuTimer   = CPU_DELAY;
    }
}

static void handleMenuInput(Game* g) {
    if (!IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) return;
    Vector2 mouse = GetMousePosition();

    // Button layout (must match drawMenu below)
    int btnW = 200, btnH = 70, gap = 40;
    int startX = SW / 2 - (btnW * 2 + gap) / 2;
    Rectangle easyBtn = { (float)startX,              400, (float)btnW, (float)btnH };
    Rectangle hardBtn = { (float)(startX + btnW + gap), 400, (float)btnW, (float)btnH };

    if (CheckCollisionPointRec(mouse, easyBtn)) {
        g->diff  = DIFF_EASY;
        g->state = GS_BOARD;
        strcpy(g->msg, "Easy  |  Your turn - select a piece");
    } else if (CheckCollisionPointRec(mouse, hardBtn)) {
        g->diff  = DIFF_HARD;
        g->state = GS_BOARD;
        strcpy(g->msg, "Hard  |  Your turn - select a piece");
    }
}

static void update(Game* g, float dt) {
    g->time += dt;
    updateLuminance(g);

    if (g->state == GS_MENU) {
        handleMenuInput(g);
        return;
    }

    if (g->state == GS_GAME_OVER) {
        if (IsKeyPressed(KEY_R)) initGame(g); // returns to menu so player can change difficulty
        return;
    }

    if (g->turn == SIDE_LIGHT) {
        handlePlayerInput(g);
    } else {
        if (g->cpuWaiting) {
            g->cpuTimer -= dt;
            if (g->cpuTimer <= 0.0f) {
                if (g->diff == DIFF_HARD) doCpuMoveHard(g);
                else                       doCpuMove(g);
            }
        }
    }
}

// ============================================================
//  DRAW
// ============================================================
static void drawBoard(const Game* g) {
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            const Cell* cell = &g->board[r][c];
            Color col = lerpColor(
                cell->isPower ? COL_POWER_DARK  : COL_CELL_DARK,
                cell->isPower ? COL_POWER_LIGHT : COL_CELL_LIGHT,
                cell->lum);
            DrawRectangle(c * CELL, BOARD_Y + r * CELL, CELL, CELL, col);
            DrawRectangleLines(c * CELL, BOARD_Y + r * CELL, CELL, CELL, { 0, 0, 0, 60 });
            if (cell->isPower)
                DrawRectangleLines(c * CELL + 3, BOARD_Y + r * CELL + 3,
                                   CELL - 6, CELL - 6, { 255, 180, 0, 180 });
        }
    }
}

static void drawValidMoves(const Game* g) {
    if (g->selected < 0) return;
    MoveOption moves[MAX_MOVES];
    int count = getValidMoves(g, g->selected, moves);
    for (int i = 0; i < count; i++)
        DrawRectangle(moves[i].col * CELL + 2, BOARD_Y + moves[i].row * CELL + 2,
                      CELL - 4, CELL - 4, COL_VALID_MOVE);
}

static void drawSelected(const Game* g) {
    if (g->selected < 0) return;
    const Piece* p = &g->pieces[g->selected];
    DrawRectangle(p->col * CELL, BOARD_Y + p->row * CELL, CELL, CELL, COL_SELECTED);
}

static void drawPieces(const Game* g) {
    for (int i = 0; i < g->pieceCount; i++) {
        const Piece* p = &g->pieces[i];
        if (!p->alive) continue;

        int cx = p->col * CELL + CELL / 2;
        int cy = BOARD_Y + p->row * CELL + CELL / 2;

        Color fill = (p->side == SIDE_LIGHT) ? COL_LIGHT_PIECE : COL_DARK_PIECE;
        Color text = (p->side == SIDE_LIGHT) ? BLACK : WHITE;

        DrawCircle(cx, cy, PIECE_RADIUS + 2, BLACK);
        DrawCircle(cx, cy, PIECE_RADIUS, fill);
        if (g->board[p->row][p->col].isPower)
            DrawCircleLines(cx, cy, PIECE_RADIUS + 2, GOLD);
        drawTextCentered(STATS[p->type].abbrev, cx, cy, 16, text);
    }
}

static void drawMenu(const Game* g) {
    // Draw the board and pieces in the background as a preview
    drawBoard(g);
    drawPieces(g);
    // Dark overlay so the menu text is readable
    DrawRectangle(0, 0, SW, SH, { 0, 0, 0, 180 });

    // Title
    drawTextCentered("ARCHON", SW / 2, 160, 72, GOLD);
    drawTextCentered("The Light and the Dark", SW / 2, 240, 26, WHITE);
    drawTextCentered("Select Difficulty", SW / 2, 350, 28, COL_UI_TEXT);

    // Button layout (must match handleMenuInput above)
    int btnW = 200, btnH = 70, gap = 40;
    int startX = SW / 2 - (btnW * 2 + gap) / 2;
    int easyX  = startX;
    int hardX  = startX + btnW + gap;

    // Easy button
    Color easyFill; easyFill = (g->diff == DIFF_EASY)
        ? Color{ 30, 160,  30, 255 }
        : Color{ 15,  80,  15, 255 };
    DrawRectangle(easyX, 400, btnW, btnH, easyFill);
    DrawRectangleLines(easyX, 400, btnW, btnH, WHITE);
    drawTextCentered("EASY", easyX + btnW / 2, 435, 30, WHITE);
    drawTextCentered("Greedy heuristic", easyX + btnW / 2, 460, 14, LIGHTGRAY);

    // Hard button
    Color hardFill; hardFill = (g->diff == DIFF_HARD)
        ? Color{ 180,  30,  30, 255 }
        : Color{  80,  15,  15, 255 };
    DrawRectangle(hardX, 400, btnW, btnH, hardFill);
    DrawRectangleLines(hardX, 400, btnW, btnH, WHITE);
    drawTextCentered("HARD", hardX + btnW / 2, 435, 30, WHITE);
    drawTextCentered("Minimax + positional", hardX + btnW / 2, 460, 14, LIGHTGRAY);

    drawTextCentered("Click a button to start", SW / 2, 510, 18, GRAY);
}

static void drawUI(const Game* g) {
    DrawRectangle(0, 0, SW, BOARD_Y, COL_UI_BG);
    DrawText("ARCHON", 10, 8, 28, GOLD);

    const char* diffLabel = (g->diff == DIFF_HARD) ? "HARD" : "EASY";
    Color diffCol = (g->diff == DIFF_HARD) ? Color{ 220, 80, 80, 255 } : Color{ 80, 200, 80, 255 };
    DrawText(diffLabel, 130, 12, 20, diffCol);

    const char* turnLabel = (g->turn == SIDE_LIGHT) ? "LIGHT (You)" : "DARK (CPU)";
    Color turnCol = (g->turn == SIDE_LIGHT) ? COL_LIGHT_TURN : COL_DARK_TURN;
    DrawText(TextFormat("Turn: %s", turnLabel), 220, 10, 20, turnCol);

    if (g->cpuWaiting)
        DrawText(TextFormat("CPU thinking... %.1fs", g->cpuTimer), 530, 10, 18, GRAY);

    int barY = BOARD_Y + ROWS * CELL;
    DrawRectangle(0, barY, SW, SH - barY, COL_UI_BG);
    DrawText(g->msg, 10, barY + 8, 18, COL_UI_TEXT);

    if (g->state == GS_GAME_OVER) {
        DrawRectangle(0, 0, SW, SH, { 0, 0, 0, 160 });
        drawTextCentered(g->msg, SW / 2, SH / 2 - 20, 48, GOLD);
        drawTextCentered("Press R for menu", SW / 2, SH / 2 + 40, 24, WHITE);
    }
}

// ============================================================
//  MAIN
// ============================================================
int main() {
    srand((unsigned int)time(0));

    InitWindow(SW, SH, "ARCHON - The Light and the Dark  [PROTOTYPE]");
    SetTargetFPS(FPS);

    Game g;
    initGame(&g);

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        update(&g, dt);

        BeginDrawing();
        ClearBackground(BLACK);
        if (g.state == GS_MENU) {
            drawMenu(&g);
        } else {
            drawBoard(&g);
            drawSelected(&g);
            drawValidMoves(&g);
            drawPieces(&g);
            drawUI(&g);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}