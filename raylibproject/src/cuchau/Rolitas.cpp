#include "Rolitas.h"
#include "raylib.h"
#include <cstring>

namespace Archon_PID {

    //  GetActual — Devuelve el stream de musica que corresponde al estado actual

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

    void Rolitas::SetMusic(std::string_view p1, std::string_view p2, bool fin) {

        EstadoMusica nuevoEstado;

        if (fin) {
            nuevoEstado = EstadoMusica::Victoria;
        }
        else {
            // Seleccionar musica segun los personajes
            if (p1 == "MH" && p2 == "Platero")      nuevoEstado = EstadoMusica::MagoMago;
            else if (p1 == "MH" || p2 == "Platero") nuevoEstado = EstadoMusica::PeonMago;
            else                                     nuevoEstado = EstadoMusica::Combate;
        }

        // Solo cambiar si el estado es diferente
        if (estado == nuevoEstado) return;

        // Transicion: detener el track actual e iniciar el nuevo
        StopMusicStream(GetActual());
        estado = nuevoEstado;
        PlayMusicStream(GetActual());
    }

    //  Update — Actualiza el buffer del stream de musica actual
    //
    //  Debe llamarse cada frame para que la musica siga sonando sin interrupciones.

    void Rolitas::Update() {
        UpdateMusicStream(GetActual());
    }

    void Rolitas::ResetToMenu()
    {
        StopMusicStream(GetActual());
        estado = EstadoMusica::Menu;
        PlayMusicStream(musica_menu);
    }

    void Rolitas::Stop()
    {
        StopMusicStream(GetActual());
    }

}