#include "PiezaTablero.h"

/*
 * DrawT - Dibuja la pieza en su casilla del tablero
 *
 * Los sprites originales son de 640x640 (pensados para combate 3D),
 * asi que se escalan a 64x64 (tamanoCasilla) usando DrawTexturePro.
 *
 * paramx/paramy: esquina superior-izquierda del tablero en pantalla
 * La posicion final se calcula como: offset + columna/fila * 64
 */
void PiezaTablero::DrawT(const int paramx, const int paramy)
{
    float w = (float)Sprite.width;      // Ancho real del sprite (640)
    float h = (float)Sprite.height;     // Alto real del sprite (640)
    float size = 64.0f;                 // Tamanio de casilla en el tablero

    // Calcular posicion en pixeles segun fila/columna
    pos.x = (float)(paramx + columna * 64);
    pos.y = (float)(paramy + fila * 64);

    DrawTexturePro(
        Sprite,
        { 0, 0, w, h },                                // Rectangulo fuente: sprite completo
        { pos.x + size / 2, pos.y + size / 2, size, size }, // Rectangulo destino: centrado en casilla, escalado a 64x64
        { size / 2, size / 2 },                         // Origen: centro del destino (para centrar el sprite)
        0.0f,                                           // Sin rotacion
        WHITE                                           // Sin tintado
    );
}
