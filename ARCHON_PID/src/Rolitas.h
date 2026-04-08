#pragma once
#include "Music.h"

class Rolitas
{
    Music musica_combate = LoadMusicStream(MusicaBatalla1V1);  // archivo en bin/
    Music musica_menu = LoadMusicStream(MusicaMenu); // Música del menú
    Music musica_mago_mago = LoadMusicStream(MusicaMagoMago);
    Music musica_peon_mago = LoadMusicStream(MusicaPeonMago);
    Music musica_creditos = LoadMusicStream(MusicaCreditos);
public:
        Rolitas() {
            PlayMusicStream(musica_menu);
        }
        void SetMusic(const char* p1, const char* p2, bool fin);
        void Update();
        ~Rolitas() {
            UnloadMusicStream(musica_menu);
            UnloadMusicStream(musica_combate);
            UnloadMusicStream(musica_mago_mago);
            UnloadMusicStream(musica_peon_mago);
            UnloadMusicStream(musica_creditos);
        }
};

