#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <map>
#include "Pj.h"

// ============================================================
//  Estructura que representa el estado completo de una partida
//  guardada. Solo almacena datos serializables (sin punteros).
// ============================================================
struct SaveData
{
    // Personajes elegidos (nombre como clave de búsqueda)
    std::string nombreP1;
    std::string nombreP2;

    // Vida actual de cada personaje en el momento del guardado
    float vidaP1 = 0.0f;
    float vidaP2 = 0.0f;

    // Posición de cada personaje
    float posP1x = 0.0f, posP1y = 0.0f;
    float posP2x = 0.0f, posP2y = 0.0f;

    // Modo de juego y dificultad
    bool  modoIA    = false;
    int   dificultad = 1;   // 0=Fácil 1=Normal 2=Difícil

    // Partida válida (false si el fichero no existía o estaba corrupto)
    bool valida = false;
};

// ============================================================
//  SaveManager
//  Escribe y lee el estado de partida en un fichero .txt
//  con formato clave=valor por sección [seccion].
//
//  Ejemplo de fichero generado:
//
//  [partida]
//  modoIA=0
//  dificultad=1
//
//  [jugador1]
//  nombre=MH
//  vida=320.50
//  posX=124.00
//  posY=200.00
//
//  [jugador2]
//  nombre=Dragon
//  vida=850.00
//  posX=740.00
//  posY=180.00
// ============================================================
class SaveManager
{
    static const char* RUTA_GUARDADO;

    // Parsea el fichero y devuelve un mapa seccion.clave → valor
    static std::map<std::string, std::string> Parsear(const std::string& ruta);

    // Ayuda a leer un float del mapa o devuelve defecto
    static float LeerFloat(const std::map<std::string, std::string>& m,
                           const std::string& clave, float defecto = 0.0f);
    static int   LeerInt  (const std::map<std::string, std::string>& m,
                           const std::string& clave, int defecto = 0);
    static std::string LeerStr(const std::map<std::string, std::string>& m,
                               const std::string& clave,
                               const std::string& defecto = "");

public:
    // Escribe la partida al disco. Devuelve true si tuvo éxito.
    static bool Guardar(const SaveData& datos);

    // Lee la partida del disco. El campo 'valida' del resultado
    // indica si la carga fue correcta.
    static SaveData Cargar();

    // Devuelve true si existe un fichero de guardado en disco.
    static bool ExisteGuardado();

    // Busca un Pj por nombre en las listas conocidas.
    // Devuelve nullptr si no lo encuentra.
    static const Pj* BuscarPjPorNombre(const std::string& nombre);
};
