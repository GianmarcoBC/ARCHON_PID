#include "Rolitas.h"

void Rolitas::SetMusic(const char* p1, const char* p2, bool fin) {
    if (!fin) {
        if (p1 == "MH" && p2 == "Platero") {
            StopMusicStream(musica_menu);
            PlayMusicStream(musica_mago_mago);
        }
        else if (p1 == "MH" || p2 == "Platero") {
            StopMusicStream(musica_menu);
            PlayMusicStream(musica_peon_mago);
        }
        else {
            StopMusicStream(musica_menu);
            PlayMusicStream(musica_combate);
        }
    }
    else {
        StopMusicStream(musica_mago_mago);
        StopMusicStream(musica_peon_mago);
        StopMusicStream(musica_combate);
        PlayMusicStream(musica_creditos);
    }
    
}

void Rolitas::Update() {
    UpdateMusicStream(musica_menu);
    UpdateMusicStream(musica_combate);
    UpdateMusicStream(musica_mago_mago);
    UpdateMusicStream(musica_peon_mago);
    UpdateMusicStream(musica_creditos);
}