#include "Pereza.h"
#include "Rolitas.h"
#include "SaveManager.h"

//  Estados del juego
enum class EstadoJuego
{
    Menu,         // Selección de personajes / modo
    Combate,      // Combate en curso
    Pausa,        // Pausa con opciones Guardar / Cargar / Continuar / Salir
    CargandoSave  // Frame de transición: construye el Combate desde el save
};

// struct en lugar de class porque todos los miembros son públicos por defecto.
// Agrupa todo lo relacionado con la pausa en un solo sitio.
struct MenuPausa
{
    enum Opcion { Continuar = 0, Guardar, Cargar, VolverMenu, NUM_OPCIONES };

    int   cursor = 0;      // Índice de la opción actualmente seleccionada
    bool  guardadoOK = false;  // Si el último guardado fue exitoso o no
    float timerMsg = 0.0f;   // Tiempo restante para mostrar el mensaje de confirmación

    // Resetea todo al estado inicial. Se llama cada vez que se abre la pausa
    // para que no queden restos de la vez anterior (cursor en otra posición,
    // mensaje de guardado sobrando, etc.)
    void Reset() { cursor = 0; guardadoOK = false; timerMsg = 0.0f; }

    // Devuelve el índice de la opción pulsada, o -1 si no se pulsó nada.
    // El llamador compara ese valor con el enum para saber qué hacer.
    int Update(float dt)
    {
        // Si hay un mensaje visible (guardado OK / error), lo vamos consumiendo
        // restando el tiempo transcurrido en este frame hasta que llegue a 0
        if (timerMsg > 0.0f) timerMsg -= dt;

        // Mover cursor hacia abajo. El % NUM_OPCIONES hace que al llegar al
        // final vuelva al principio (0, 1, 2, 3, 0, 1, 2, 3...)
        if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))
            cursor = (cursor + 1) % NUM_OPCIONES;

        // Mover cursor hacia arriba. El + NUM_OPCIONES antes del % evita
        // números negativos: si cursor=0 y restamos 1, sin eso daría -1 % 4
        // que en C++ puede ser -1. Con el truco queda (0-1+4)%4 = 3. ✓
        if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))
            cursor = (cursor - 1 + NUM_OPCIONES) % NUM_OPCIONES;

        // Si se pulsa ENTER confirmamos la opción actual devolviendo su índice
        if (IsKeyPressed(KEY_ENTER))
            return cursor;

        // ESC siempre cierra la pausa sin hacer nada, como si eligieras Continuar
        if (IsKeyPressed(KEY_ESCAPE))
            return Continuar;

        // Ninguna tecla relevante pulsada este frame
        return -1;
    }

    // Dibuja el panel de pausa encima del combate congelado.
    // Recibe si hay guardado en disco para poder deshabilitar visualmente
    // la opción Cargar cuando no hay nada que cargar.
    void Draw(bool hayGuardado)
    {
        int W = GetScreenWidth();
        int H = GetScreenHeight();

        // Rectángulo negro semitransparente sobre toda la pantalla.
        // El 160 es el canal alpha (0=invisible, 255=opaco). Da el efecto
        // de "oscurecer" el combate de fondo sin taparlo del todo.
        DrawRectangle(0, 0, W, H, { 0, 0, 0, 160 });

        // Calculamos la posición del panel para que quede centrado en pantalla
        int panelW = 380, panelH = 310;
        int px = W / 2 - panelW / 2;  // borde izquierdo del panel
        int py = H / 2 - panelH / 2;  // borde superior del panel

        // Fondo del panel, casi opaco (alpha 230) con color azul muy oscuro
        DrawRectangle(px, py, panelW, panelH, { 20, 20, 30, 230 });
        // Borde blanco alrededor del panel
        DrawRectangleLines(px, py, panelW, panelH, WHITE);

        // Título centrado horizontalmente. MeasureText devuelve el ancho en
        // píxeles del texto, dividirlo entre 2 y restarlo al centro de pantalla
        const char* titulo = "PAUSA";
        DrawText(titulo, W / 2 - MeasureText(titulo, 36) / 2, py + 18, 36, YELLOW);

        // Array de textos para cada opción. El operador ternario cambia el
        // texto de Cargar según haya o no fichero de guardado en disco.
        const char* etiquetas[NUM_OPCIONES] = {
            "Continuar",
            "Guardar Partida",
            hayGuardado ? "Cargar Partida" : "Cargar  (sin datos)",
            "Volver al Menu"
        };

        // Colores por opción. Cargar aparece en gris oscuro si no hay save,
        // indicando visualmente que está deshabilitada aunque se pueda seleccionar.
        Color colores[NUM_OPCIONES] = {
            WHITE, SKYBLUE,
            hayGuardado ? LIME : DARKGRAY,
            ORANGE
        };

        // Dibujamos cada opción. El índice i va de 0 a NUM_OPCIONES-1
        for (int i = 0; i < NUM_OPCIONES; ++i) {

            // Posición vertical de esta opción. Cada una está 52 píxeles
            // por debajo de la anterior, empezando a py+88 (debajo del título)
            int bY = py + 88 + i * 52;

            // ¿Es esta la opción actualmente seleccionada por el cursor?
            bool sel = (i == cursor);

            // Si está seleccionada, dibujamos un rectángulo blanco muy
            // transparente (alpha 25) de fondo como efecto de resaltado
            if (sel) DrawRectangle(px + 20, bY - 4, panelW - 40, 38, { 255,255,255,25 });

            // Dibujamos el texto. Si está seleccionado lo pintamos en amarillo,
            // si no, usamos el color propio de esa opción del array colores[]
            DrawText(etiquetas[i], W / 2 - MeasureText(etiquetas[i], 24) / 2, bY, 24, sel ? YELLOW : colores[i]);
        }

        // Mensaje de confirmación tras guardar. Solo se muestra mientras
        // timerMsg > 0 (dura 2 segundos, se va consumiendo en Update)
        if (timerMsg > 0.0f) {
            const char* msg = guardadoOK ? "Partida guardada!" : "Error al guardar";
            DrawText(msg,
                W / 2 - MeasureText(msg, 20) / 2,
                py + panelH + 12, 20,          // justo debajo del panel
                guardadoOK ? GREEN : RED);      // verde si OK, rojo si error
        }

        // Ayuda de controles fija en la parte inferior de la pantalla
        DrawText("W/S = navegar    ENTER = confirmar    ESC = continuar",
            W / 2 - MeasureText("W/S = navegar    ENTER = confirmar    ESC = continuar", 14) / 2,
            H - 28, 14, DARKGRAY);
    }
};

int main()
{

    InitWindow(0, 0, "ARCHON PID - Combate");
    SetTargetFPS(60);
    InitAudioDevice();

    EstadoJuego estado = EstadoJuego::Menu;

    Menu_Combate* menu = new Menu_Combate();
    Combate* combate = nullptr;
    Rolitas       rolitas;
    MenuPausa     pausa;

    bool modoIA = false;
    int  dificultad = 1;

    SaveData savePendiente;

    while (!WindowShouldClose()) {

        // GetFrameTime() devuelve el tiempo en segundos que tardó en procesarse
        // el frame anterior. Se usa para que el movimiento sea independiente
        // de los FPS (si el juego va a 30fps o 60fps, todo se mueve igual)
        float dt = GetFrameTime();

        // Actualiza el stream de música activo (necesario llamarlo cada frame
        // para que raylib siga reproduciendo el audio sin cortes)
        rolitas.Update();

        // Todo lo que se dibuje entre BeginDrawing y EndDrawing aparece en pantalla
        BeginDrawing();
        // Limpia la pantalla con color blanco antes de dibujar el frame nuevo,
        // evitando que se acumulen los frames anteriores
        ClearBackground(RAYWHITE);

        // La máquina de estados. Según el valor de 'estado' ejecuta un bloque
        // u otro. Es la forma más limpia de gestionar fases distintas del juego
        // sin llenar el main de ifs anidados.
        switch (estado)
        {

            // ESTADO: MENÚ PRINCIPAL
        case EstadoJuego::Menu:
        {
            // menu->Update() procesa la selección de personajes y devuelve
            // true solo cuando el jugador pulsa ENTER para empezar.
            // Todo lo que hay dentro del if solo ocurre ese frame concreto.
            if (menu->Update()) {

                // Guardamos el modo y dificultad elegidos para poder
                // incluirlos en el fichero de guardado más adelante
                modoIA = menu->EsModoIA();
                dificultad = menu->GetDificultad();

                // Creamos el Combate con los personajes elegidos.
                // El constructor es distinto según si hay IA o no.
                if (modoIA)
                    combate = new Combate(menu->GetSelP1(), menu->GetSelP2(), dificultad);
                else
                    combate = new Combate(menu->GetSelP1(), menu->GetSelP2());

                // Reseteamos la pausa por si quedó sucia de una partida anterior
                pausa.Reset();
                // Cambiamos de estado: el próximo frame ya entrará en Combate
                estado = EstadoJuego::Combate;
            }

            // Dibujamos el menú de selección de personajes
            menu->Draw();

            // Solo mostramos el atajo F5 si realmente existe un save en disco,
            // para no confundir al jugador cuando no hay nada que cargar
            if (SaveManager::ExisteGuardado()) {
                const char* hint = "[ F5 ]  Continuar partida guardada";
                DrawText(hint,
                    GetScreenWidth() / 2 - MeasureText(hint, 16) / 2,
                    GetScreenHeight() - 50, 16, LIME);

                if (IsKeyPressed(KEY_F5)) {
                    // Cargamos el save en savePendiente. No creamos el Combate
                    // aquí directamente porque necesitamos un frame de transición
                    // (estado CargandoSave) para construirlo limpiamente
                    savePendiente = SaveManager::Cargar();
                    if (savePendiente.valida)
                        estado = EstadoJuego::CargandoSave;
                }
            }
            break;
        }

        // ESTADO: CARGANDO SAVE (frame de transición)
        case EstadoJuego::CargandoSave:
        {
            // Buscamos los Pj por nombre en la tabla de personajes conocidos.
            // Esto es necesario porque el save solo guarda el nombre como string,
            // no el struct Pj completo.
            const Pj* pj1 = SaveManager::BuscarPjPorNombre(savePendiente.nombreP1);
            const Pj* pj2 = SaveManager::BuscarPjPorNombre(savePendiente.nombreP2);

            // Si ambos nombres son válidos, construimos el combate
            if (pj1 && pj2) {

                // Si había un combate anterior en memoria lo destruimos primero
                // para no provocar una fuga de memoria
                delete combate;

                // Construimos el Combate con los personajes del save
                if (savePendiente.modoIA)
                    combate = new Combate(*pj1, *pj2, savePendiente.dificultad);
                else
                    combate = new Combate(*pj1, *pj2);

                // Restauramos vida y posición al estado exacto del momento del guardado
                combate->CargarEstado(savePendiente);

                // Recuperamos también el modo y dificultad para futuros guardados
                modoIA = savePendiente.modoIA;
                dificultad = savePendiente.dificultad;

                pausa.Reset();
                estado = EstadoJuego::Combate;
            }
            else {
                // Algún nombre no se reconoció (save corrupto o de otra versión).
                // Volvemos al menú en lugar de crashear.
                estado = EstadoJuego::Menu;
            }

            // Limpiamos el save pendiente tanto si fue bien como si no,
            // para que no queden datos basura en memoria
            savePendiente = SaveData{};
            break;
        }

        // ESTADO: COMBATE ACTIVO
        case EstadoJuego::Combate:
        {
            // Actualiza física, movimiento, disparos, colisiones...
            combate->Update();
            // Dibuja el fondo, personajes, disparos, obstáculos...
            combate->Draw();

            // Actualiza la música según los personajes en juego y si
            // el combate ha terminado (para poner la música de victoria)
            rolitas.SetMusic(menu->GetSelP1().nombre, menu->GetSelP2().nombre, combate->IsGameOver());

            // Solo permitimos pausa y guardado si el combate sigue en curso.
            // Si ya hay un ganador no tiene sentido guardar ese estado.
            if (!combate->IsGameOver() && IsKeyPressed(KEY_F6)) {
                pausa.Reset();
                estado = EstadoJuego::Pausa;
            }

            // F5 = guardado rápido. Guarda y muestra el resultado
            // en pantalla durante este frame sin abrir ningún menú.
            if (!combate->IsGameOver() && IsKeyPressed(KEY_F5)) {
                bool ok = combate->GuardarEstado(modoIA, dificultad);
                const char* msg = ok ? "Partida guardada!" : "Error al guardar";
                DrawText(msg,
                    GetScreenWidth() / 2 - MeasureText(msg, 22) / 2,
                    36, 22, ok ? GREEN : RED);
            }
            break;
        }

        // ESTADO: PAUSA
        case EstadoJuego::Pausa:
        {
            // Dibujamos el combate congelado de fondo antes que nada,
            // para que el panel de pausa aparezca encima de él
            combate->Draw();

            // Comprobamos cada frame si hay save en disco, ya que el
            // propio menú de pausa puede crear uno (opción Guardar)
            bool hayGuardado = SaveManager::ExisteGuardado();

            // Update devuelve -1 si no se pulsó nada, o el índice de la opción elegida
            int accion = pausa.Update(dt);
            // Dibuja el panel semitransparente con las opciones
            pausa.Draw(hayGuardado);

            // Actuamos según la opción elegida (o no hacemos nada si accion == -1)
            if (accion == MenuPausa::Continuar) {
                // Simplemente volvemos al combate
                estado = EstadoJuego::Combate;
            }
            else if (accion == MenuPausa::Guardar) {
                // Guardamos y activamos el mensaje de confirmación 2 segundos
                bool ok = combate->GuardarEstado(modoIA, dificultad);
                pausa.guardadoOK = ok;
                pausa.timerMsg = 2.0f;
                // No cambiamos de estado: seguimos en Pausa para que el jugador
                // vea el mensaje y pueda elegir qué hacer a continuación
            }
            else if (accion == MenuPausa::Cargar) {
                if (hayGuardado) {
                    savePendiente = SaveManager::Cargar();
                    if (savePendiente.valida)
                        estado = EstadoJuego::CargandoSave;
                }
            }
            else if (accion == MenuPausa::VolverMenu) {
                // Destruimos el combate actual liberando su memoria
                delete combate;
                combate = nullptr;
                // Destruimos el menú actual y creamos uno nuevo para
                // resetear toda la selección de personajes desde cero
                delete menu;
                menu = new Menu_Combate();
                estado = EstadoJuego::Menu;
            }
            break;
        }
        } // fin del switch

        // Manda todo lo dibujado este frame a la pantalla
        EndDrawing();

    } // fin del while

    // Al salir del bucle liberamos toda la memoria dinámica
    // antes de cerrar para no dejar fugas
    delete combate;
    delete menu;
    CloseAudioDevice();  // Cierra el sistema de audio de raylib
    CloseWindow();       // Cierra la ventana y el contexto de raylib
    return 0;
}