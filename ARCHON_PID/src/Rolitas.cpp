#include "Rolitas.h"
#include <cstring>

Music& Rolitas::GetActual()
{
    switch (estado) {
    case EstadoMusica::Menu:     return musica_menu;
    case EstadoMusica::Combate:  return musica_combate;
    case EstadoMusica::MagoMago: return musica_mago_mago;
    case EstadoMusica::PeonMago: return musica_peon_mago;
    case EstadoMusica::Creditos: return musica_creditos;
    }
    return musica_menu;
}

void Rolitas::SetMusic(const char* p1, const char* p2, bool fin) {
   
    EstadoMusica nuevoEstado;

    if (fin) {
        nuevoEstado = EstadoMusica::Creditos;
    }else{
        StopMusicStream(musica_menu);
        
        if (!strcmp("MH", p1) && !strcmp("Platero", p2)) nuevoEstado = EstadoMusica::MagoMago;
        else if (!strcmp("MH", p1) || !strcmp("Platero", p2)) nuevoEstado = EstadoMusica::PeonMago;
        else nuevoEstado = EstadoMusica::Combate;
    }    

    if (estado == nuevoEstado) return; // Si ya está sonando la música correcta, no hacemos nada

    StopMusicStream(GetActual());  // Para la actual
    estado = nuevoEstado;          // Actualiza el estado
    PlayMusicStream(GetActual());  // Arranca la nueva
}

void Rolitas::Update() {
    UpdateMusicStream(GetActual());
}