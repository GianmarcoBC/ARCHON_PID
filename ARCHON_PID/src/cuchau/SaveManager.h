#pragma once
#include "Pj_info.h"
#include <cstdio>
#include <cstring>

// Ruta del fichero de guardado (relativa al working directory del proyecto)
inline constexpr const char* SAVE_FILE = "savegame.txt";

// Datos que se persisten en disco
struct SaveData
{
    const char* nombreP1 = "";
    float vidaP1 = 0;
    float posP1x = 0, posP1y = 0;   // Posicion en plano XZ (Vec2)

    const char* nombreP2 = "";
    float vidaP2 = 0;
    float posP2x = 0, posP2y = 0;

    bool modoIA = false;
    int  dificultad = 1;

    bool valida = false;  // true si se cargo correctamente desde disco
};

// Funciones estaticas para guardar, cargar y buscar personajes por nombre.
// No necesita instancia: todo es static.
class SaveManager
{
public:

    // Escribe un SaveData al disco en formato texto legible.
    static bool Guardar(const SaveData& d)
    {
        FILE* f = fopen(SAVE_FILE, "w");
        if (!f) return false;

        fprintf(f, "# ARCHON 3D - Fichero de partida guardada\n");
        fprintf(f, "# No editar manualmente salvo que sepas lo que haces.\n\n");
        fprintf(f, "[partida]\n");
        fprintf(f, "modoIA=%d\n",     d.modoIA ? 1 : 0);
        fprintf(f, "dificultad=%d\n", d.dificultad);
        fprintf(f, "\n[jugador1]\n");
        fprintf(f, "nombre=%s\n",     d.nombreP1);
        fprintf(f, "vida=%g\n",       d.vidaP1);
        fprintf(f, "posX=%g\n",       d.posP1x);
        fprintf(f, "posY=%g\n",       d.posP1y);
        fprintf(f, "\n[jugador2]\n");
        fprintf(f, "nombre=%s\n",     d.nombreP2);
        fprintf(f, "vida=%g\n",       d.vidaP2);
        fprintf(f, "posX=%g\n",       d.posP2x);
        fprintf(f, "posY=%g\n",       d.posP2y);

        fclose(f);
        return true;
    }

    // Lee el fichero de guardado y devuelve un SaveData.
    // Si el fichero no existe o esta corrupto, devuelve SaveData con valida=false.
    static SaveData Cargar()
    {
        SaveData d{};
        FILE* f = fopen(SAVE_FILE, "r");
        if (!f) return d;

        // Buffers temporales para los nombres (static para que sobrevivan al return)
        static char bufNomP1[64]{};
        static char bufNomP2[64]{};
        bufNomP1[0] = bufNomP2[0] = '\0';

        char linea[256];
        int seccion = 0; // 0=ninguna, 1=partida, 2=jugador1, 3=jugador2

        while (fgets(linea, sizeof(linea), f))
        {
            // Ignorar comentarios y lineas vacias
            if (linea[0] == '#' || linea[0] == '\n' || linea[0] == '\r') continue;

            // Detectar secciones
            if (strncmp(linea, "[partida]",  9) == 0) { seccion = 1; continue; }
            if (strncmp(linea, "[jugador1]", 10) == 0) { seccion = 2; continue; }
            if (strncmp(linea, "[jugador2]", 10) == 0) { seccion = 3; continue; }

            // Parsear clave=valor
            char clave[64]{}, valor[128]{};
            if (sscanf(linea, "%63[^=]=%127[^\n\r]", clave, valor) != 2) continue;

            if (seccion == 1) {
                if (strcmp(clave, "modoIA")     == 0) d.modoIA     = (atoi(valor) != 0);
                if (strcmp(clave, "dificultad") == 0) d.dificultad = atoi(valor);
            }
            else if (seccion == 2) {
                if (strcmp(clave, "nombre") == 0) { strncpy(bufNomP1, valor, 63); d.nombreP1 = bufNomP1; }
                if (strcmp(clave, "vida")   == 0) d.vidaP1  = (float)atof(valor);
                if (strcmp(clave, "posX")   == 0) d.posP1x  = (float)atof(valor);
                if (strcmp(clave, "posY")   == 0) d.posP1y  = (float)atof(valor);
            }
            else if (seccion == 3) {
                if (strcmp(clave, "nombre") == 0) { strncpy(bufNomP2, valor, 63); d.nombreP2 = bufNomP2; }
                if (strcmp(clave, "vida")   == 0) d.vidaP2  = (float)atof(valor);
                if (strcmp(clave, "posX")   == 0) d.posP2x  = (float)atof(valor);
                if (strcmp(clave, "posY")   == 0) d.posP2y  = (float)atof(valor);
            }
        }
        fclose(f);

        // Validar que al menos ambos nombres se leyeron
        d.valida = (bufNomP1[0] != '\0' && bufNomP2[0] != '\0');
        return d;
    }

    // Comprueba si existe un fichero de guardado en disco
    static bool ExisteGuardado()
    {
        FILE* f = fopen(SAVE_FILE, "r");
        if (!f) return false;
        fclose(f);
        return true;
    }

    // Busca un Pj_info por nombre entre los 16 personajes conocidos.
    // Devuelve nullptr si no lo encuentra (save corrupto o de otra version).
    static const Pj_info* BuscarPjPorNombre(std::string_view nombre)
    {
        static const Pj_info* todos[] = {
            &MH, &Phoenix, &Golem, &Djinni, &Unicorn, &Valkyrie, &Archer, &Knight,
            &Platero, &ShapeShifter, &Troll, &Dragon, &Basilisk, &Banshee, &Manticore, &Goblin
        };
        for (auto* pj : todos)
            if (pj->nombre.data() == nombre.data()) return pj;
        return nullptr;
    }
};
