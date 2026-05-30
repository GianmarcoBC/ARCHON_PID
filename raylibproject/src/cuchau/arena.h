#pragma once
#include "raylib.h"
#include "obstaculo.h"
#include <vector>

namespace Archon_PID {

    class Arena
    {
        // --- Dimensiones de la arena ---
        static constexpr float sueloWidth = 40.0f;   // Ancho del suelo (eje X)
        static constexpr float sueloLength = 25.0f;   // Largo del suelo (eje Z)
        static constexpr float wallHeight = 2.0f;    // Alto de cada panel de la pared trasera
        static constexpr float wallWidth = 4.0f;    // Ancho de cada panel de la pared trasera

        // --- Recursos de renderizado ---
        Texture2D texBg{};         // Textura de fondo 2D (ciudad)
        Texture2D texPared{};      // Textura de la pared trasera
        Texture2D texSuelo{};      // Textura del suelo
        Texture2D texGameOver{};   // Textura de pantalla de victoria

        Mesh  sueloMesh{};         // Malla del plano del suelo
        Model sueloModel{};        // Modelo del suelo (malla + textura)

        Mesh  wallMesh{};          // Malla de un panel de pared
        Model wallModel{};         // Modelo de pared (malla + textura)

        friend class Interacciones; // Interacciones necesita acceso a las dimensiones de la arena para resolver colisiones

    public:

        Arena();

        float getSueloWidth()  const { return sueloWidth; }
        float getSueloLength() const { return sueloLength; }

        void DrawFondo() const; // Dibuja el fondo 2D (ciudad) cubriendo toda la pantalla
        void Draw(Camera camera) const;

        ~Arena() {
            // Liberar texturas y modelos
            UnloadTexture(texBg);
            UnloadTexture(texPared);
            UnloadTexture(texSuelo);
            UnloadTexture(texGameOver);
            UnloadModel(sueloModel);
            UnloadModel(wallModel);
            // Liberar obstaculos (texturas, modelos y memoria)
        }
    };

}