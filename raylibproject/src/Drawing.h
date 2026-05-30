#pragma once
#include "GameState.h"

namespace Archon_PID {

    class Drawing {
    public:
        static void setupProjection();
        static void texto12(float x, float y, const std::string& t, Color color);
        static void texto18(float x, float y, const std::string& t, Color color);
        static void texto24(float x, float y, const std::string& t, Color color);
        static void instrucciones(float x, float y, const std::string& t, float tiempo);
        static void circulo(float x, float y, float r, int seg, Color color);
        static void circuloLinea(float x, float y, float r, int seg, Color color);
        static void antorcha(float x, float y, float tiempo);
        static void antorchaGrande(float x, float y, float tiempo);
        static void logo(float tiempo);
        static void cursorAnimado(float x, float y, float tiempo);
        static void barraSlider(float x, float y, float w, int valor, int vmax, bool esBool);
        static void botonControl(float cx, float cy, float r, bool activo, float pr, float pg, float pb);
        static void panelMenu(float y);
        static void panelBatalla(float y, bool sel);
        static void iconoEspada(float x, float y, Color color);
        static void iconoOpciones(float x, float y, Color color);
        static void iconoLibro(float x, float y, Color color);
        static void iconoEscudos(float x, float y, Color color);
        static void iconoIA(float x, float y, Color color);
        static void iconoNota(float x, float y, Color color);
        static void spriteCaballero(float x, float y, float esc, float tiempo);
        static void spriteArquero(float x, float y, float esc, float tiempo);
        static void spriteOrco(float x, float y, float esc, float tiempo);
        static void spriteNigromante(float x, float y, float esc, float tiempo);
        static void dibujarSprite(int idx, float x, float y, float esc, float tiempo);
        static void simboloLuz(float x, float y, float esc, float tiempo);
        static void simboloOscuridad(float x, float y, float esc, float tiempo);
        // Back arrow button — draws arrow + "VOLVER" label, returns true if mouse is hovering over it
        static bool botonVolver(float x, float y, float tiempo);
    };

}