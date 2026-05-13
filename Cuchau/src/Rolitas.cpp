#include "Rolitas.h"
#include "raylib.h"
#include <cstring>

// ============================================================================
//  GetActual — Devuelve el stream de musica que corresponde al estado actual
// ============================================================================

Music& Rolitas::GetActual()
{
    switch (estado) {
    case EstadoMusica::Menu:     return musica_menu;
    case EstadoMusica::Combate:  return musica_combate;
    case EstadoMusica::MagoMago: return musica_mago_mago;
    case EstadoMusica::PeonMago: return musica_peon_mago;
    case EstadoMusica::Victoria: return musica_victoria;
    }
    return musica_menu;  // Fallback por seguridad
}

// ============================================================================
//  SetMusic — Determina que musica debe sonar segun el estado del combate
//
//  Reglas de seleccion:
//    - Si el combate termino (fin=true) → musica de victoria
//    - Si P1 es "MH" y P2 es "Platero" → musica mago vs mago
//    - Si P1 es "MH" o P2 es "Platero" → musica peon vs mago
//    - En cualquier otro caso → musica de combate generica
//
//  Solo cambia si el nuevo estado es diferente al actual (evita reiniciar
//  el mismo track cada frame).
// ============================================================================

void Rolitas::SetMusic(const char* p1, const char* p2, bool fin) {

    EstadoMusica nuevoEstado;

    if (fin) {
        nuevoEstado = EstadoMusica::Victoria;
    }
    else {
        // Detener la musica del menu al entrar en combate
        StopMusicStream(musica_menu);

        // Seleccionar musica segun los personajes
        if (!strcmp("MH", p1) && !strcmp("Platero", p2))      nuevoEstado = EstadoMusica::MagoMago;
        else if (!strcmp("MH", p1) || !strcmp("Platero", p2)) nuevoEstado = EstadoMusica::PeonMago;
        else                                                  nuevoEstado = EstadoMusica::Combate;
    }

    // Solo cambiar si el estado es diferente
    if (estado == nuevoEstado) return;

    // Transicion: detener el track actual e iniciar el nuevo
    StopMusicStream(GetActual());
    estado = nuevoEstado;
    PlayMusicStream(GetActual());
}

// ============================================================================
//  Update — Actualiza el buffer del stream de musica actual
//
//  Debe llamarse cada frame para que la musica siga sonando sin interrupciones.
// ============================================================================

void Rolitas::Update() {
    UpdateMusicStream(GetActual());
}
