#include "Rolitas.h"

void Rolitas::SetCombatMusic(const char* p1, const char* p2) {
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

void Rolitas::Update() {
    UpdateMusicStream(musica_menu);
    UpdateMusicStream(musica_combate);
    UpdateMusicStream(musica_mago_mago);
    UpdateMusicStream(musica_peon_mago);
}