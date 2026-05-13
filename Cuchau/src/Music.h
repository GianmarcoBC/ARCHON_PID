#pragma once

// ============================================================================
//  Music.h — Constantes con las rutas de los archivos de musica
//
//  Cada variable apunta a un archivo MP3 dentro de bin/Resources/Audio/Musica/.
//  Se usan en Rolitas para cargar los streams de musica del juego.
// ============================================================================

// Musica del menu principal
inline const char* MusicaMenu = "bin/Resources/Audio/Musica/slimshady.mp3";

// Musica generica de batalla (1 vs 1 normal)
inline const char* MusicaBatalla1V1 = "bin/Resources/Audio/Musica/MusicaBatalla1V1.mp3";

// Musica especial cuando pelean dos magos (MH vs Platero)
inline const char* MusicaMagoMago = "bin/Resources/Audio/Musica/MusicaMagoMago.mp3";

// Musica especial cuando pelea un peon contra un mago
inline const char* MusicaPeonMago = "bin/Resources/Audio/Musica/MusicaPeonMago.mp3";

// Musica de victoria al finalizar el combate
inline const char* MusicaVictoria = "bin/Resources/Audio/Musica/ifrith.mp3";
