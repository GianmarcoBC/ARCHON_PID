#pragma once
#include "Music.h"
#include "raylib.h"

// ============================================================================
//  Rolitas.h — Gestor de musica del juego
//
//  Maquina de estados de musica con 5 estados:
//    Menu     — Musica del menu principal
//    Combate  — Musica generica de batalla
//    MagoMago — Musica especial cuando pelean MH vs Platero
//    PeonMago — Musica especial cuando pelea MH o Platero contra otro
//    Victoria — Musica de victoria al terminar el combate
//
//  Solo un track suena a la vez. SetMusic() cambia el estado segun
//  los nombres de los personajes y si el combate termino.
// ============================================================================

enum class EstadoMusica { Menu, Combate, MagoMago, PeonMago, Victoria };

class Rolitas
{
    // Streams de musica cargados al inicializar
    Music musica_combate   = LoadMusicStream(MusicaBatalla1V1.data());
    Music musica_menu      = LoadMusicStream(MusicaMenu.data());
    Music musica_mago_mago = LoadMusicStream(MusicaMagoMago.data());
    Music musica_peon_mago = LoadMusicStream(MusicaPeonMago.data());
    Music musica_victoria  = LoadMusicStream(MusicaVictoria.data());

    EstadoMusica estado = EstadoMusica::Menu;  // Estado actual de la musica

    // Devuelve referencia al stream que corresponde al estado actual
    Music& GetActual();

public:
    // Constructor: inicia reproduciendo la musica del menu
    Rolitas() { PlayMusicStream(musica_menu); }

    // Cambia la musica segun los personajes en combate y si el juego termino
    void SetMusic(std::string_view p1, std::string_view p2, bool fin);

    // Actualiza el buffer del stream de musica actual (debe llamarse cada frame)
    void Update();

    // Vuelve a la musica del menu (se llama al salir del combate)
    void ResetToMenu();

    // Detiene toda la musica de combate sin reproducir nada nuevo
    void Stop();

    // Destructor: libera todos los streams de musica
    ~Rolitas() {
        UnloadMusicStream(musica_menu);
        UnloadMusicStream(musica_combate);
        UnloadMusicStream(musica_mago_mago);
        UnloadMusicStream(musica_peon_mago);
        UnloadMusicStream(musica_victoria);
    }
};
