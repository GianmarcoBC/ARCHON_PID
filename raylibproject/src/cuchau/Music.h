#pragma once
#include <string_view>

// ============================================================================
//  Music.h — Constantes con las rutas de los archivos de musica
//
//  Cada variable apunta a un archivo MP3 dentro de bin/Resources/Audio/Musica/.
//  Se usan en Rolitas para cargar los streams de musica del juego.
// ============================================================================

namespace Archon_PID {

	// Musica del menu principal
	inline std::string_view MusicaMenu = "bin/Resources/AAAudio/Musica/ageofwar.mp3";

	// Musica generica de batalla (1 vs 1 normal)
	inline std::string_view MusicaBatalla1V1 = "bin/Resources/AAAudio/Musica/MusicaBatalla1V1.mp3";

	// Musica especial cuando pelean dos magos (MH vs Platero)
	inline std::string_view MusicaMagoMago = "bin/Resources/AAAudio/Musica/MusicaMagoMago.mp3";

	// Musica especial cuando pelea un peon contra un mago
	inline std::string_view MusicaPeonMago = "bin/Resources/AAAudio/Musica/MusicaPeonMago.mp3";

	// Musica de victoria al finalizar el combate
	inline std::string_view MusicaVictoria = "bin/Resources/AAAudio/Musica/ifrith.mp3";

} // namespace Archon_PID