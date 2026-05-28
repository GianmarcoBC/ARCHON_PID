#include "Particles.h"
#include "rlgl.h"

void Particles::init(GameState& gs) {
    for (int i = 0; i < 80; i++) {
        Particula p;
        p.x = (float)(rand() % 800); p.y = (float)(rand() % 600);
        p.radio = 5.0f + rand() % 15;
        p.angulo = (rand() % 360) * M_PI / 180.0f;
        p.velocidadAngular = 0.001f + (rand() % 5) / 5000.0f;
        gs.particulas.push_back(p);
    }
    for (int i = 0; i < 40; i++) {
        ParticulaFuego p;
        p.x = 50.0f + rand() % 700; p.y = (float)(rand() % 100);
        p.vy = 0.3f + (rand() % 10) / 10.0f;
        p.vida = (float)(rand() % 100); p.vidaMax = 80.0f + rand() % 60;
        p.size = 1.5f + (rand() % 4);
        gs.particulasFuego.push_back(p);
    }
    for (int i = 0; i < 60; i++) {
        ParticulaTeatro p;
        p.x = 90.0f + rand() % 620; p.y = 130.0f + rand() % 390;
        p.vx = ((rand() % 100) - 50) * 0.005f;
        p.vy = ((rand() % 100) - 50) * 0.005f - 0.05f;
        p.vida = (float)(rand() % 200); p.vidaMax = 150.0f + rand() % 150;
        p.size = 1.5f;
        p.r = 0.5f + (rand() % 5) * 0.08f;
        p.g = 0.1f + (rand() % 3) * 0.05f;
        p.b = 0.05f;
        gs.particulasTeatro.push_back(p);
    }
    for (int i = 0; i < 50; i++) {
        ParticulaPolvo p;
        p.x = 85.0f + rand() % 630; p.y = 80.0f + rand() % 460;
        p.vx = ((rand() % 100) - 50) * 0.003f;
        p.vy = 0.03f + rand() % 100 * 0.001f;
        p.vida = (float)(rand() % 200); p.vidaMax = 120.0f + rand() % 200;
        p.size = 1;
        gs.particulasPolvo.push_back(p);
    }
    for (int i = 0; i < 35; i++) {
        ParticulaNota p;
        p.x = 160.0f + rand() % 480; p.y = 110.0f + rand() % 420;
        p.vx = ((rand() % 100) - 50) * 0.007f;
        p.vy = 0.1f + (rand() % 100) * 0.008f;
        p.vida = (float)(rand() % 200); p.vidaMax = 100.0f + rand() % 200;
        p.size = 3.0f + (rand() % 4);
        p.rot = (float)(rand() % 360);
        p.tipo = rand() % 3;
        gs.particulasNotas.push_back(p);
    }
}

void Particles::update(GameState& gs) {
    // Mouse position in virtual 800x600 Y-up coords
    float mxv = (float)GetMouseX() * 800.f / GetScreenWidth();
    float myv = 600.f - (float)GetMouseY() * 600.f / GetScreenHeight();

    for (auto& p : gs.particulas) {
        p.angulo += p.velocidadAngular;
        if (p.angulo > 2 * M_PI) p.angulo -= 2.0f * M_PI;
        // Push stars away from mouse
        float px = p.x + cosf(p.angulo) * p.radio;
        float py = p.y + sinf(p.angulo) * p.radio;
        float dx = px - mxv, dy = py - myv;
        float dist2 = dx*dx + dy*dy;
        if (dist2 < 3600.f && dist2 > 1.f) { // radius ~60px
            float f = 0.8f / sqrtf(dist2);
            p.x += dx * f;
            p.y += dy * f;
        }
    }

    if (gs.controlesOpciones[3].valor) {
        for (auto& p : gs.particulasFuego) {
            p.vida += p.vy;
            if (p.vida > p.vidaMax) {
                p.x = 50.0f + rand() % 700; p.y = (float)(rand() % 60);
                p.vida = 0; p.vidaMax = 80.0f + rand() % 60; p.size = 1.5f + (rand() % 4);
            }
            p.y += p.vy * 0.8f;
            p.x += sinf(gs.tiempo * 0.01f + p.x * 0.05f) * 0.3f;
            // Mouse repels fire particles
            float dx = p.x - mxv, dy = p.y - myv;
            float d2 = dx*dx + dy*dy;
            if (d2 < 2500.f && d2 > 1.f) { float f = 0.5f/sqrtf(d2); p.x += dx*f; p.y += dy*f; }
        }
        for (auto& p : gs.particulasTeatro) {
            p.vida += 0.4f; p.x += p.vx; p.y += p.vy;
            if (p.vida > p.vidaMax) {
                p.x = 90.0f + rand() % 620; p.y = 130.0f + rand() % 390;
                p.vx = ((rand() % 100) - 50) * 0.005f;
                p.vy = ((rand() % 100) - 50) * 0.005f - 0.05f;
                p.vida = 0; p.vidaMax = 150.0f + rand() % 150;
            }
            // Mouse attracts theater sparks slightly
            float dx = mxv - p.x, dy = myv - p.y;
            float d2 = dx*dx + dy*dy;
            if (d2 < 4900.f && d2 > 1.f) { float f = 0.15f/sqrtf(d2); p.vx += dx*f*0.01f; p.vy += dy*f*0.01f; }
        }
        for (auto& p : gs.particulasPolvo) {
            p.vida += 0.3f;
            p.x += p.vx + sinf(gs.tiempo * 0.005f + p.y * 0.01f) * 0.1f;
            p.y += p.vy;
            if (p.vida > p.vidaMax) {
                p.x = 85.0f + rand() % 630; p.y = 80.0f + rand() % 460;
                p.vx = ((rand() % 100) - 50) * 0.003f;
                p.vy = 0.03f + rand() % 100 * 0.001f;
                p.vida = 0; p.vidaMax = 120.0f + rand() % 200;
            }
            // Mouse swirls dust
            float dx = p.x - mxv, dy = p.y - myv;
            float d2 = dx*dx + dy*dy;
            if (d2 < 3600.f && d2 > 1.f) { float f = 0.3f/sqrtf(d2); p.vx += dy*f*0.01f; p.vy -= dx*f*0.01f; }
        }
        for (auto& p : gs.particulasNotas) {
            p.vida += 0.3f; p.x += p.vx; p.y += p.vy;
            p.rot += 0.5f;
            if (p.vida > p.vidaMax) {
                p.x = 160.0f + rand() % 480; p.y = 110.0f + rand() % 420;
                p.vx = ((rand() % 100) - 50) * 0.007f;
                p.vy = 0.1f + (rand() % 100) * 0.008f;
                p.vida = 0; p.vidaMax = 100.0f + rand() % 200;
                p.tipo = rand() % 3;
                p.size = 3.0f + (rand() % 4);
            }
        }
    }
}

void Particles::drawStars(const GameState& gs) {
    if (!gs.controlesOpciones[3].valor) return;
    for (auto& p : gs.particulas) {
        float px = p.x + cosf(p.angulo) * p.radio;
        float py = p.y + sinf(p.angulo) * p.radio;
        float b = 0.7f + 0.3f * sinf(gs.tiempo * 0.005f + p.x * 0.01f);
        rlBegin(RL_TRIANGLES);
        rlColor4f(b, b, 1.0f, 1.0f);
        rlVertex2f(px - 1, py); rlVertex2f(px + 1, py); rlVertex2f(px, py + 1);
        rlEnd();
    }
}
