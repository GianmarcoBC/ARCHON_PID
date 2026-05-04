#include "Combate.h"

void Combate::Update()
{
    float dt = GetFrameTime();

    //Player 1
    P1.Update(dt);

    if (cooldown1 <= 0.0f) {
        if (IsKeyPressed(KEY_SPACE)) {
            Disparos_1.push_back(P1.Shoot()); // Agrega un nuevo disparo al vector
            P1.PlayAttackSound(); // Reproduce el sonido de ataque al disparar
            cooldown1 = P1.get_Cooldown(); // Reinicia el tiempo de recarga
        }
    }
    else {
        cooldown1 -= dt; // Reduce el tiempo de recarga
    }

    //Gestión de los disparos del Player 1
    interaccion.DisparosContraPersonaje(Disparos_1, P1, P2, dt); // Procesa los disparos de P1 contra P2


    //Player 2
    if (ia != nullptr) {
        // La IA mueve a P2 y decide si dispara
        bool dispara = ia->Update(dt, Disparos_1);  // esquiva los disparos de P1
        if (cooldown2 <= 0.0f) {
            if (dispara) {
                Disparos_2.push_back(P2.Shoot());
                P2.PlayAttackSound();
                cooldown2 = P2.get_Cooldown(); // Reinicia el tiempo de recarga
            }
        }
        else {
            cooldown2 -= dt; // Reduce el tiempo de recarga
        }
    }

    else {
        // El jugador controla a P2
        P2.Update(dt);
        
        if (cooldown2 <= 0.0f) {
            if (IsKeyPressed(KEY_RIGHT_CONTROL)) {
                Disparos_2.push_back(P2.Shoot()); // Agrega un nuevo disparo al vector
                P2.PlayAttackSound(); // Reproduce el sonido de ataque al disparar
                cooldown2 = P2.get_Cooldown(); // Reinicia el tiempo de recarga
            }
        }
        else {
            cooldown2 -= dt; // Reduce el tiempo de recarga
        }
    }

    //Gestión de los disparos del Player 2
    interaccion.DisparosContraPersonaje(Disparos_2, P2, P1, dt); // Procesa los disparos de P2 contra P1

    //Colisiones de personajes con obstáculos sólidos y entre ellos
    interaccion.PersonajesContraObstaculos(P1, P2, Obstaculos);
    interaccion.PersonajeContraPersonaje(P1, P2);

    interaccion.ClampArena(P1); // Mantiene a P1 dentro de la arena
    interaccion.ClampArena(P2); // Mantiene a P2 dentro de la arena

}

void Combate::Draw()
{
    //Gana jugador 2
    if (P1.GetVida() == 0) {
        DrawTexturePro(
            GameOver,
            { 0, 0, (float)GameOver.width, (float)GameOver.height }, // fuente: sprite completo
            { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, // destino: pantalla completa
            { 0, 0 },
            0.0f,
            WHITE
        );       
        DrawText("Player 2 Wins!", 350, 300, 40, RED);
    
    }

    //Gana jugador 1
    else if (P2.GetVida() == 0) {
        DrawTexturePro(
            GameOver,
            { 0, 0, (float)GameOver.width, (float)GameOver.height }, // fuente: sprite completo
            { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() }, // destino: pantalla completa
            { 0, 0 },
            0.0f,
            WHITE
        );          
        DrawText("Player 1 Wins!", 350, 300, 40, RED);
    }

    //Juego en curso
    else {
        DrawTexturePro(
            Fondo,
            { 0, 0, (float)Fondo.width, (float)Fondo.height }, // fuente: sprite completo
            { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()}, // destino: pantalla completa
            { 0, 0 },
            0.0f,
            WHITE
        );     
        
        // Primero arbustos decorativos (debajo de los personajes)
        for (auto& obs : Obstaculos) {
            if (!obs.solido) {
                float t = GetTime();
                unsigned char alpha = (sinf(t * 5.0f) > 0) ? 255 : 80;
                DrawTexture(*obs.Sprite, obs.hitbox.x, obs.hitbox.y, { 255, 255, 255, alpha });
            }
        }

        P1.Draw();
        P2.Draw();


        for (auto& b : Disparos_1) {
            b.Draw();
        }

        for (auto& b : Disparos_2) {
            b.Draw();
        }

        // Luego arbustos sólidos (encima de los personajes)
        for (auto& obs : Obstaculos) {
            if (obs.solido)
                DrawTexture(*obs.Sprite, obs.hitbox.x, obs.hitbox.y, WHITE);
        }
    }
    
}

void Combate::GenerarObstaculos()
{
    for (int i = 0; i < 10; i++) {
        bool esSolido = (GetRandomValue(0, 1) == 1);
        Texture2D* tx = esSolido ? &ObsSolido : &ObsDecor;

        // Genera una posición aleatoria dentro de los límites de la pantalla.
        // Modificar si no se quiere randomizar la posición de los obstáculos.
        float x = (float)GetRandomValue(100, GetScreenWidth() - 100);
        float y = (float)GetRandomValue(100, GetScreenHeight() - 100);
        Obstaculos.push_back({ tx, { x, y, (float)tx->width, (float)tx->height }, esSolido });
    }
}
