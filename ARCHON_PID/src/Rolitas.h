#pragma once
#include "Music.h"

class Rolitas
{
    Music musica_combate = LoadMusicStream(MusicaBatalla1V1);  // archivo en bin/
    Music musica_menu = LoadMusicStream(MusicaTablero_Inicio); // Música del menú
    Music musica_mago_mago = LoadMusicStream(MusicaMagoMago);
    Music musica_peon_mago = LoadMusicStream(MusicaPeonMago);
public:
        Rolitas() {
            PlayMusicStream(musica_menu);
        }
        void SetCombatMusic(const char* p1, const char* p2);
        void Update();
        ~Rolitas() {
            UnloadMusicStream(musica_menu);
            UnloadMusicStream(musica_combate);
            UnloadMusicStream(musica_mago_mago);
            UnloadMusicStream(musica_peon_mago);
        }
};

