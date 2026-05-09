#include "SaveManager.h"
#include <iostream>
#include <cstring>

//Ruta del fichero de guardado
const char* SaveManager::RUTA_GUARDADO = "savegame.txt";

//Lista completa de personajes para búsqueda por nombre
static const Pj* TODOS_LOS_PJS[] = {
    &MH, &Phoenix, &Golem, &Djinni, &Unicorn, &Valkyrie, &Archer, &Knight,
    &Platero, &ShapeShifter, &Troll, &Dragon, &Basilisk, &Banshee, &Manticore, &Goblin
};
static constexpr int NUM_TOTAL_PJS = 16;

//  Parsea el fichero .txt con formato [seccion] / clave=valor
//  Devuelve mapa con claves "seccion.clave"

std::map<std::string, std::string> SaveManager::Parsear(const std::string& ruta)
{
    // Creamos el mapa vacío que vamos a ir rellenando y devolveremos al final
    std::map<std::string, std::string> resultado;

    // Abrimos el fichero. ifstream = "input file stream", flujo de lectura
    std::ifstream f(ruta);

    // Si no se pudo abrir (no existe, sin permisos...) devolvemos el mapa vacío
    if (!f.is_open()) return resultado;

    // 'linea' guardará cada línea del fichero según la vamos leyendo
    // 'seccionActual' recuerda en qué sección estamos ([partida], [jugador1]...)
    std::string linea, seccionActual;

    // getline lee una línea entera del fichero y la mete en 'linea'
    // el while sigue mientras queden líneas por leer
    while (std::getline(f, linea)) {

        // find_first_not_of busca el primer carácter que NO sea espacio, tabulador
        // o salto de línea. Así ignoramos la indentación y líneas en blanco.
        // 'ini' es la posición de ese primer carácter útil
        size_t ini = linea.find_first_not_of(" \t\r\n");

        // npos significa "no encontrado", es decir, la línea era completamente vacía
        // 'continue' salta al siguiente ciclo del while, ignorando esta línea
        if (ini == std::string::npos) continue;

        // Recortamos la línea desde el primer carácter útil hacia adelante
        // Ejemplo: "   vida=320" se convierte en "vida=320"
        linea = linea.substr(ini);

        // Si el primer carácter es '#' o ';' es un comentario, lo ignoramos
        if (linea[0] == '#' || linea[0] == ';') continue;

        // Si la línea empieza por '[' es una cabecera de sección tipo [jugador1]
        if (linea[0] == '[') {

            // Buscamos el corchete de cierre ']'
            size_t fin = linea.find(']');

            // Si lo encontramos, extraemos el texto que hay entre [ y ]
            // substr(1, fin-1) significa: empieza en posición 1 (saltando el '[')
            // y coge fin-1 caracteres (sin incluir el ']')
            // Ejemplo: "[jugador1]" → seccionActual = "jugador1"
            if (fin != std::string::npos)
                seccionActual = linea.substr(1, fin - 1);

            // Pasamos a la siguiente línea, esto no es un par clave=valor
            continue;
        }

        // Buscamos el signo '=' para separar clave y valor
        size_t eq = linea.find('=');

        // Solo procesamos si encontramos '=' Y ya sabemos en qué sección estamos
        if (eq != std::string::npos && !seccionActual.empty()) {

            // La clave es "seccion.nombreCampo"
            // substr(0, eq) coge desde el inicio hasta antes del '='
            // Ejemplo: sección "jugador1" + campo "vida" → clave = "jugador1.vida"
            std::string clave = seccionActual + "." + linea.substr(0, eq);

            // El valor es todo lo que hay después del '='
            // Ejemplo: "vida=320.50" → valor = "320.50"
            std::string valor = linea.substr(eq + 1);

            // Metemos el par en el mapa
            // Si la clave ya existía, sobreescribe el valor anterior
            resultado[clave] = valor;
        }
    }

    // Devolvemos el mapa ya relleno con todos los pares del fichero
    return resultado;
}

float SaveManager::LeerFloat(const std::map<std::string, std::string>& m,const std::string& clave, float defecto)
{
    // 'it' es un iterador, que funciona como un puntero a una entrada del mapa. find() busca la clave dentro del
    // mapa y devuelve un iterador apuntando a ella si la encuentra.
    auto it = m.find(clave);

    // m.end() es un iterador especial que significa "no encontrado", apunta
    // al final del mapa (fuera de él, no a ningún elemento real).
    // Si find() devuelve end(), la clave no existe en el mapa.
    // En ese caso devolvemos el valor por defecto que nos pasaron
    if (it == m.end()) return defecto;

    // Si llegamos aquí, la clave existe. it->second es el valor asociado,
    // es decir, el string que hay a la derecha del '=' en el fichero.
    // Ejemplo: it->second == "320.50"
    //
    // std::stof convierte ese string a float ("320.50" → 320.50f)
    // Lo metemos en un try porque stof puede fallar si el string no es
    // un número válido (ej: "abc", "", "32O" con una O en vez de cero)
    try { return std::stof(it->second); }

    // Los '...' capturan cualquier tipo de excepción sin importar cuál sea.
    // Si stof lanzó un error, llegamos aquí y devolvemos el valor por defecto
    // en lugar de que el programa se rompa.
    catch (...) { return defecto; }
}

int SaveManager::LeerInt(const std::map<std::string, std::string>& m, const std::string& clave, int defecto)
{
    // Código análogo a la función anterior, pero usando std::stoi para convertir a int en lugar de float
    auto it = m.find(clave);
    if (it == m.end()) return defecto;
    try { return std::stoi(it->second); }
    catch (...) { return defecto; }
}

std::string SaveManager::LeerStr(const std::map<std::string, std::string>& m, const std::string& clave, const std::string& defecto)
{
    // Código análogo a la función anterior
    auto it = m.find(clave);
    if (it == m.end()) return defecto;
    return it->second;
}

//  Guardar
bool SaveManager::Guardar(const SaveData& datos)
{
    // ofstream = "output file stream", flujo de escritura. Al construirlo con la ruta, intenta crear o sobreescribir
    // el fichero. Si ya existía un save anterior, lo machaca desde cero.
    std::ofstream f(RUTA_GUARDADO);

    // Si no se pudo abrir (sin permisos, ruta inválida...) avisamos por consola
    // y devolvemos false para que el llamador sepa que el guardado falló.
    // cerr es como cout pero para errores, en algunos entornos se muestra en rojo.
    if (!f.is_open()) {
        std::cerr << "[SaveManager] Error: no se pudo abrir " << RUTA_GUARDADO << " para escribir.\n";
        return false;
    }

    // El operador << funciona igual que con cout pero escribe en el fichero
    // en lugar de en la consola. Aquí escribimos dos líneas de comentario
    // para que si alguien abre el txt sepa qué es el fichero.
    f << "# ARCHON PID - Fichero de partida guardada\n";
    f << "# No editar manualmente salvo que sepas lo que haces.\n\n";

    // Sección [partida] con el modo de juego y la dificultad.
    // El operador ternario (condicion ? a : b) convierte el bool modoIA
    // a 1 o 0 porque los bool escritos directamente saldrían como "true"/"false"
    // y nuestro Parsear espera números enteros.
    f << "[partida]\n";
    f << "modoIA=" << (datos.modoIA ? 1 : 0) << "\n";
    f << "dificultad=" << datos.dificultad << "\n\n";  // \n\n = línea en blanco entre secciones

    // Sección [jugador1] con nombre, vida y posición actuales.
    // Estos valores vienen del SaveData que construyó GuardarEstado()
    // justo antes de llamar a esta función.
    f << "[jugador1]\n";
    f << "nombre=" << datos.nombreP1 << "\n";
    f << "vida=" << datos.vidaP1 << "\n";
    f << "posX=" << datos.posP1x << "\n";
    f << "posY=" << datos.posP1y << "\n\n";

    // Igual para el jugador 2
    f << "[jugador2]\n";
    f << "nombre=" << datos.nombreP2 << "\n";
    f << "vida=" << datos.vidaP2 << "\n";
    f << "posX=" << datos.posP2x << "\n";
    f << "posY=" << datos.posP2y << "\n";

    // Cerramos el fichero explícitamente para asegurarnos de que todos los datos
    // se escriben al disco en este momento. Sin esto, podrían quedarse en un
    // buffer de memoria y no llegar al fichero si el programa peta justo después.
    f.close();

    // Todo fue bien, avisamos al llamador
    return true;
}

//  Cargar
SaveData SaveManager::Cargar()
{
    // Creamos un SaveData vacío. Como 'valida' se inicializa a false por defecto
    // en la definición del struct, si algo falla y salimos antes, el llamador
    // sabrá que la carga no fue correcta comprobando d.valida
    SaveData d;

    // Parseamos el fichero y obtenemos el mapa "seccion.clave" → "valor"
    auto m = Parsear(RUTA_GUARDADO);

    // Si el mapa está vacío significa que el fichero no existía o estaba vacío
    // Devolvemos el SaveData sin rellenar (d.valida == false)
    if (m.empty()) return d;

    // LeerInt devuelve 0 o 1 desde el fichero. Como modoIA es un bool,
    // comparamos con != 0 para convertirlo correctamente (0→false, 1→true)
    // El tercer parámetro es el valor por defecto si la clave no existe
    d.modoIA = (LeerInt(m, "partida.modoIA", 0) != 0);
    d.dificultad = LeerInt(m, "partida.dificultad", 1);   // por defecto Normal

    // Rellenamos los datos del jugador 1 desde la sección [jugador1]
    // Si alguna clave no existe en el mapa, se usa el valor por defecto
    d.nombreP1 = LeerStr(m, "jugador1.nombre", "");      // "" si no existe
    d.vidaP1 = LeerFloat(m, "jugador1.vida", 0.0f);
    d.posP1x = LeerFloat(m, "jugador1.posX", 100.0f);  // posición por defecto
    d.posP1y = LeerFloat(m, "jugador1.posY", 100.0f);  // esquina superior izquierda

    // Igual para el jugador 2, con su posición por defecto al otro lado
    d.nombreP2 = LeerStr(m, "jugador2.nombre", "");
    d.vidaP2 = LeerFloat(m, "jugador2.vida", 0.0f);
    d.posP2x = LeerFloat(m, "jugador2.posX", 800.0f);  // posición por defecto
    d.posP2y = LeerFloat(m, "jugador2.posY", 100.0f);  // esquina superior derecha

    // Validación mínima: comprobamos que el save tiene sentido antes de usarlo.
    // Si alguien editó el txt a mano y borró un nombre, o guardó con vida 0
    // (lo que significaría que la partida ya había terminado), lo marcamos
    // como inválido para no cargar un estado roto.
    // El && encadena condiciones: todas deben cumplirse para que valida sea true
    d.valida = (!d.nombreP1.empty() && !d.nombreP2.empty() && d.vidaP1 > 0.0f && d.vidaP2 > 0.0f);

    // Devolvemos el SaveData relleno. El llamador comprobará d.valida
    // antes de intentar usarlo para construir el Combate
    return d;
}

//  ExisteGuardado
bool SaveManager::ExisteGuardado()
{
    std::ifstream f(RUTA_GUARDADO);
    // El fichero se pudo abrir, lo que significa que existe. No es necesario leerlo ni validarlo aquí, solo comprobar su existencia.
    return f.good();
}

//  BuscarPjPorNombre
const Pj* SaveManager::BuscarPjPorNombre(const std::string& nombre)
{
    for (int i = 0; i < NUM_TOTAL_PJS; ++i)
        if (nombre == TODOS_LOS_PJS[i]->nombre)
            return TODOS_LOS_PJS[i];
    return nullptr;
}
