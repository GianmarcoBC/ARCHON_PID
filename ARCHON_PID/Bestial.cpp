// ============================================================
//  ARCHON - MENÚ CINEMÁTICO COMPLETO
//  Compilar Linux/Mac:
//    g++ archon_menu.cpp -o archon -lfreeglut -lGL -lGLU
//  Compilar Windows (MinGW + freeglut):
//    g++ archon_menu.cpp -o archon -lfreeglut -lopengl32 -lglu32
// ============================================================
#include <freeglut.h>
#include <vector>
#include <string>
#include <cmath>
#include <cstdlib>
#include <algorithm>

#define M_PI 3.14159265358979323846f

// ============================================================
// ESTADOS
// ============================================================
enum Estado {
    MENU,
    JUGAR, JUGAR_PVP, JUGAR_IA,
    OPCIONES,
    MUSICA,
    ENCICLOPEDIA
};
Estado estadoActual = MENU;
Estado siguienteEstado = MENU;

// ============================================================
// TIEMPO GLOBAL
// ============================================================
float tiempo = 0.0f;

// ============================================================
// OFFSETS DE ANIMACIÓN DE ENTRADA
// ============================================================
float menuOffset = -600.0f;
float jugarOffset = -600.0f;
float encicloOffset = -800.0f;
float opcionesOffset = -800.0f;
float musicaOffset = -800.0f;

// ============================================================
// TRANSICIÓN SLASH
// ============================================================
bool  slashActivo = false;
float slashX = -1000.0f;

// ============================================================
// MENÚ PRINCIPAL
// ============================================================
std::vector<std::string> opcionesMenu = { "JUGAR", "OPCIONES", "ENCICLOPEDIA" };
int opcionMenuSel = 0;

// ============================================================
// MENÚ JUGAR
// ============================================================
std::vector<std::string> opcionesJugar = { "PVP", "CONTRA IA" };
int opcionJugarSel = 0;
std::vector<std::string> descJugar = {
    "Desafia a otro jugador. Domina\nel tablero y demuestra tu maestria.",
    "Enfrenta a la maquina. Pon a\nprueba tu estrategia contra la IA."
};

// ============================================================
// ENCICLOPEDIA — LIBRO ANTIGUO
// ============================================================
const int TOTAL_PAGINAS = 4;
int paginaLibro = 0;

struct DatosTropa {
    std::string nombre, bando, stat1, stat2, stat3;
    std::vector<std::string> descripcion;
};
std::vector<DatosTropa> tropas = {
    { "CABALLERO", "Fuerzas de Luz",   "ATQ: Alto",    "DEF: Alto",   "MOV: Medio",
      { "Guerrero de elite blindado con", "armadura sagrada. Lidera la carga",
        "en nombre del rey de la Luz.",   "Efectivo contra unidades pesadas." } },
    { "ARQUERO",   "Fuerzas de Luz",   "ATQ: Medio",   "DEF: Bajo",   "MOV: Alto",
      { "Explorador agil de ojos certeros.", "Sus flechas besan el viento antes",
        "de encontrar su blanco.",            "Efectivo contra unidades voladoras." } },
    { "ORCO",      "Fuerzas Oscuras",  "ATQ: Muy Alto","DEF: Medio",  "MOV: Bajo",
      { "Bestia de la oscuridad. Su furia",   "es legendaria en el campo de batalla.",
        "Arremete sin miedo ni piedad.",       "Efectivo contra estructuras." } },
    { "NIGROMANTE","Fuerzas Oscuras",  "ATQ: Bajo",    "DEF: Bajo",   "MOV: Medio",
      { "Maestro de la magia sombria.",    "Invoca espiritus y corroe la",
        "voluntad del enemigo.",            "Efectivo contra grupos numerosos." } }
};

// ============================================================
// MENÚ OPCIONES — TEATRO
// ============================================================
struct OpcionControl {
    std::string nombre;
    int valor, min, max;
    bool esBool;
};
std::vector<OpcionControl> controlesOpciones = {
    { "Volumen Musica",      8, 0, 10, false },
    { "Volumen Efectos",     7, 0, 10, false },
    { "Pantalla Completa",   0, 0,  1, true  },
    { "Particulas",          1, 0,  1, true  },
    { "Velocidad Animacion", 5, 1, 10, false },
    { "Idioma",              0, 0,  1, true  },
};
int opcionOpcionesSel = 0;

// ============================================================
// MENÚ MÚSICA — SALA DE ÓRGANO
// ============================================================
struct Cancion {
    std::string titulo;
    std::string artista;
    std::string duracion;   // "3:42" etc.
    std::string contexto;   // cuándo suena en el juego
    float       r, g, b;   // color temático de la canción
};

std::vector<Cancion> canciones = {
    { "Throne of Shadows",    "Archon OST", "4:12", "Menu Principal",    0.6f, 0.5f, 0.9f },
    { "Blood on the Board",   "Archon OST", "3:47", "Batalla PvP",       0.9f, 0.2f, 0.2f },
    { "March of the Fallen",  "Archon OST", "5:03", "Batalla vs IA",     0.3f, 0.7f, 0.4f },
    { "Sacred Grounds",       "Archon OST", "2:58", "Victoria Luz",      0.9f, 0.85f,0.3f },
    { "The Abyss Calls",      "Archon OST", "4:31", "Victoria Oscuridad",0.5f, 0.1f, 0.8f },
    { "Eternal Siege",        "Archon OST", "6:15", "Modo Clasico",      0.8f, 0.4f, 0.1f },
    { "Lament of the Knight", "Archon OST", "3:22", "Derrota",           0.4f, 0.4f, 0.6f },
};

int    cancionActual = 0;    // índice de la canción seleccionada/reproduciéndose
int    cancionHover = 0;    // índice resaltado en la lista
bool   reproduciendo = true; // play/pause
bool   repetir = false;
bool   aleatorio = false;
int    volumenMusica = 8;    // 0-10 (espejo del control en opciones)
float  progresoCancion = 0.0f; // 0.0 - 1.0 (simulado)
float  velocidadProg = 0.0004f; // avance por frame

// Ondas del visualizador (array de alturas simuladas)
const int NUM_BARRAS = 48;
float barrasViz[NUM_BARRAS];
float barraTarget[NUM_BARRAS];

void inicializarVisualizador() {
    for (int i = 0; i < NUM_BARRAS; i++) {
        barrasViz[i] = 2.0f + rand() % 8;
        barraTarget[i] = 2.0f + rand() % 30;
    }
}

// ============================================================
// PARTÍCULAS
// ============================================================
struct Particula { float x, y, radio, angulo, velocidadAngular; };
struct ParticulaFuego { float x, y, vy, vida, vidaMax, size; };
struct ParticulaTeatro { float x, y, vx, vy, vida, vidaMax, size, r, g, b; };
struct ParticulaPolvo { float x, y, vx, vy, vida, vidaMax, size; };

// Partículas musicales (notas flotantes)
struct ParticulaNota {
    float x, y, vy, vx, vida, vidaMax, size, rot;
    int   tipo; // 0=corchea, 1=redonda, 2=silencio
};

std::vector<Particula>       particulas;
std::vector<ParticulaFuego>  particulasFuego;
std::vector<ParticulaTeatro> particulasTeatro;
std::vector<ParticulaPolvo>  particulasPolvo;
std::vector<ParticulaNota>   particulasNotas;

// ============================================================
// UTILIDADES DE DIBUJO
// ============================================================
void texto12(float x, float y, const std::string& t) {
    glRasterPos2f(x, y);
    for (char c : t) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
}
void texto18(float x, float y, const std::string& t) {
    glRasterPos2f(x, y);
    for (char c : t) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
}
void texto24(float x, float y, const std::string& t) {
    glRasterPos2f(x, y);
    for (char c : t) glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
}

void circulo(float x, float y, float r, int seg = 24) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y);
    for (int i = 0; i <= seg; i++) {
        float a = i * 2 * M_PI / seg;
        glVertex2f(x + cos(a) * r, y + sin(a) * r);
    }
    glEnd();
}
void circuloLinea(float x, float y, float r, int seg = 24) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < seg; i++) {
        float a = i * 2 * M_PI / seg;
        glVertex2f(x + cos(a) * r, y + sin(a) * r);
    }
    glEnd();
}

void instrucciones(float x, float y, const std::string& t) {
    float al = 0.5f + 0.5f * sin(tiempo * 0.007f);
    glColor3f(al * 0.7f, al * 0.55f, al * 0.3f);
    texto12(x, y, t);
}

// ============================================================
// LOGO
// ============================================================
void logo() {
    float esc = 1.0f + sin(tiempo * 0.008f) * 0.04f;
    glPushMatrix();
    glTranslatef(400 + sin(tiempo * 0.003f) * 15, 520 + sin(tiempo * 0.004f) * 5, 0);
    glScalef(esc, esc, 1);
    glColor3f(0.9f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(-120, -20); glVertex2f(120, -20);
    glVertex2f(120, 20); glVertex2f(-120, 20);
    glEnd();
    glColor3f(0.2f, 0.1f, 0.0f); glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glVertex2f(-120, -20); glVertex2f(120, -20);
    glVertex2f(120, 20); glVertex2f(-120, 20);
    glEnd();
    glColor3f(0.1f, 0.05f, 0.0f);
    texto24(-55, -8, "ARCHON");
    glPopMatrix();
}

// ============================================================
// PARTÍCULAS GENERALES (estrellas)
// ============================================================
void dibujarParticulas() {
    glPointSize(2);
    glBegin(GL_POINTS);
    for (auto& p : particulas) {
        float px = p.x + cos(p.angulo) * p.radio;
        float py = p.y + sin(p.angulo) * p.radio;
        float b = 0.7f + 0.3f * sin(tiempo * 0.005f + p.x * 0.01f);
        glColor3f(b, b, 1.0f);
        glVertex2f(px, py);
    }
    glEnd();
}

// ============================================================
// ELEMENTOS DE FONDO COMUNES
// ============================================================
void nube(float x, float y, float esc = 1.0f, float alpha = 0.3f) {
    glColor4f(0.3f, 0.3f, 0.3f, alpha);
    for (int i = 0; i < 3; i++) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x + i * 20 * esc, y);
        for (int j = 0; j < 20; j++) {
            float a = j * 2 * M_PI / 20;
            glVertex2f(x + i * 20 * esc + cos(a) * 15 * esc, y + sin(a) * 10 * esc);
        }
        glEnd();
    }
}

void antorcha(float x, float y) {
    float fuego = sin(tiempo * 0.03f) * 6;
    glColor3f(0.3f, 0.15f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(x - 4, y); glVertex2f(x + 4, y);
    glVertex2f(x + 4, y + 25); glVertex2f(x - 4, y + 25);
    glEnd();
    glColor3f(1, 0.5f, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 40 + fuego); glVertex2f(x - 8, y + 25); glVertex2f(x + 8, y + 25);
    glEnd();
    glColor3f(1, 1, 0);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y + 35 + fuego); glVertex2f(x - 5, y + 25); glVertex2f(x + 5, y + 25);
    glEnd();
    glPointSize(2);
    glBegin(GL_POINTS);
    for (int i = 0; i < 6; i++) {
        glColor3f(1, 0.7f, 0.3f);
        glVertex2f(x + (rand() % 12) - 6, y + 25 + fuego + (rand() % 12) - 6);
    }
    glEnd();
}

void antorchaGrande(float x, float y) {
    float fuego = sin(tiempo * 0.04f) * 10;
    glColor3f(0.4f, 0.2f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(x - 6, y); glVertex2f(x + 6, y);
    glVertex2f(x + 6, y + 35); glVertex2f(x - 6, y + 35);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(x - 14, y + 30); glVertex2f(x + 14, y + 30);
    glVertex2f(x + 14, y + 36); glVertex2f(x - 14, y + 36);
    glEnd();
    glColor3f(1, 0.4f, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y + 55 + fuego);
    for (int i = 0; i < 12; i++) {
        float a = i * 2 * M_PI / 12;
        glVertex2f(x + cos(a) * 12, y + 35 + sin(a) * 5);
    }
    glEnd();
    glColor3f(1, 1, 0.3f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(x, y + 48 + fuego);
    for (int i = 0; i < 12; i++) {
        float a = i * 2 * M_PI / 12;
        glVertex2f(x + cos(a) * 7, y + 35 + sin(a) * 4);
    }
    glEnd();
    glPointSize(2.5f);
    glBegin(GL_POINTS);
    for (int i = 0; i < 10; i++) {
        glColor3f(1, 0.6f + (rand() % 4) / 10.0f, 0);
        glVertex2f(x + (rand() % 20) - 10, y + 35 + fuego + (rand() % 20));
    }
    glEnd();
}

// ============================================================
// FONDOS
// ============================================================
void fondoCastillo() {
    glBegin(GL_QUADS);
    glColor3f(0.02f, 0.02f, 0.05f); glVertex2f(0, 0);
    glColor3f(0.05f, 0.05f, 0.15f); glVertex2f(800, 0);
    glColor3f(0.1f, 0.1f, 0.25f); glVertex2f(800, 600);
    glColor3f(0.05f, 0.05f, 0.1f); glVertex2f(0, 600);
    glEnd();
    glColor3f(0.95f, 0.95f, 0.85f); circulo(700, 520, 35, 50);
    nube(50 + sin(tiempo * 0.001f) * 100, 500, 1.0f, 0.30f);
    nube(300 + sin(tiempo * 0.0015f) * 120, 480, 1.2f, 0.35f);
    nube(600 + sin(tiempo * 0.0008f) * 80, 510, 0.8f, 0.25f);
    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_QUADS);
    glVertex2f(0, 150); glVertex2f(800, 150); glVertex2f(800, 300); glVertex2f(0, 300);
    glEnd();
    for (int i = 0; i < 5; i++) {
        float x = 120 + i * 140;
        glBegin(GL_QUADS);
        glVertex2f(x, 300); glVertex2f(x + 60, 300); glVertex2f(x + 60, 430); glVertex2f(x, 430);
        glEnd();
        for (int j = 0; j < 3; j++) {
            glBegin(GL_QUADS);
            glVertex2f(x + j * 20, 430); glVertex2f(x + 10 + j * 20, 430);
            glVertex2f(x + 10 + j * 20, 450); glVertex2f(x + j * 20, 450);
            glEnd();
        }
    }
}

void fondoBatalla() {
    glBegin(GL_QUADS);
    glColor3f(0.05f, 0.01f, 0.01f); glVertex2f(0, 0);
    glColor3f(0.1f, 0.02f, 0.02f); glVertex2f(800, 0);
    glColor3f(0.18f, 0.05f, 0.05f); glVertex2f(800, 600);
    glColor3f(0.08f, 0.02f, 0.02f); glVertex2f(0, 600);
    glEnd();
    glColor3f(0.95f, 0.75f, 0.6f); circulo(680, 500, 45, 50);
    glColor4f(0.8f, 0.3f, 0.1f, 0.08f); circulo(680, 500, 80, 50);
    nube(80 + sin(tiempo * 0.0012f) * 90, 490, 1.3f, 0.4f);
    nube(350 + sin(tiempo * 0.001f) * 110, 470, 1.5f, 0.45f);
    nube(620 + sin(tiempo * 0.0009f) * 70, 505, 1.0f, 0.35f);
    glColor3f(0.07f, 0.03f, 0.03f);
    glBegin(GL_QUADS); glVertex2f(0, 100); glVertex2f(800, 100); glVertex2f(800, 220); glVertex2f(0, 220); glEnd();
    glColor3f(0.06f, 0.02f, 0.02f);
    for (int i = 0; i < 6; i++) {
        float x = 80 + i * 120, h = 150 + sin(i * 1.3f) * 30;
        glBegin(GL_QUADS); glVertex2f(x, 220); glVertex2f(x + 50, 220); glVertex2f(x + 50, 220 + h); glVertex2f(x, 220 + h); glEnd();
        for (int j = 0; j < 4; j++) {
            glBegin(GL_QUADS);
            glVertex2f(x + j * 12, 220 + h); glVertex2f(x + j * 12 + 8, 220 + h);
            glVertex2f(x + j * 12 + 8, 220 + h + 12); glVertex2f(x + j * 12, 220 + h + 12);
            glEnd();
        }
    }
    for (auto& p : particulasFuego) {
        float vn = p.vida / p.vidaMax;
        float al = (vn < 0.3f) ? vn / 0.3f : 1.0f - (vn - 0.3f) / 0.7f;
        glColor4f(1.0f, 0.3f + vn * 0.4f, 0.0f, al * 0.7f);
        glPointSize(p.size);
        glBegin(GL_POINTS); glVertex2f(p.x, p.y); glEnd();
    }
}

void fondoBiblioteca() {
    glBegin(GL_QUADS);
    glColor3f(0.06f, 0.04f, 0.02f); glVertex2f(0, 0);
    glColor3f(0.10f, 0.07f, 0.03f); glVertex2f(800, 0);
    glColor3f(0.12f, 0.08f, 0.03f); glVertex2f(800, 600);
    glColor3f(0.07f, 0.05f, 0.02f); glVertex2f(0, 600);
    glEnd();
    for (int e = 0; e < 3; e++) {
        float ey = 80 + e * 170;
        glColor3f(0.08f, 0.05f, 0.02f);
        glBegin(GL_QUADS);
        glVertex2f(0, ey); glVertex2f(800, ey); glVertex2f(800, ey + 12); glVertex2f(0, ey + 12);
        glEnd();
        for (int b = 0; b < 30; b++) {
            float bx = b * 28 + sin(b * 13) * 3;
            float bh = 60 + (b * 17) % 50;
            glColor3f(0.05f + (b * 7 % 5) * 0.03f, 0.03f + (b * 3 % 4) * 0.02f, 0.01f + (b * 11 % 3) * 0.015f);
            glBegin(GL_QUADS);
            glVertex2f(bx, ey + 12); glVertex2f(bx + 24, ey + 12);
            glVertex2f(bx + 24, ey + 12 + bh); glVertex2f(bx, ey + 12 + bh);
            glEnd();
            glColor3f(0.2f, 0.15f, 0.05f); glLineWidth(0.8f);
            glBegin(GL_LINES); glVertex2f(bx, ey + 12); glVertex2f(bx, ey + 12 + bh); glEnd();
        }
    }
    float cx = 400, cy = 575;
    glColor3f(0.3f, 0.2f, 0.05f); glLineWidth(1.5f);
    glBegin(GL_LINES); glVertex2f(cx, cy); glVertex2f(cx, cy - 30); glEnd();
    glBegin(GL_LINES); glVertex2f(cx - 60, cy - 30); glVertex2f(cx + 60, cy - 30); glEnd();
    for (int i = -1; i <= 1; i++) {
        float cvx = cx + i * 60;
        glColor3f(0.9f, 0.85f, 0.7f);
        glBegin(GL_QUADS);
        glVertex2f(cvx - 4, cy - 30); glVertex2f(cvx + 4, cy - 30);
        glVertex2f(cvx + 4, cy - 30 + 15); glVertex2f(cvx - 4, cy - 30 + 15);
        glEnd();
        float fl = sin(tiempo * 0.04f + i) * 4;
        glColor4f(1, 0.6f, 0, 0.9f);
        glBegin(GL_TRIANGLES);
        glVertex2f(cvx, cy - 30 + 25 + fl); glVertex2f(cvx - 5, cy - 30 + 15); glVertex2f(cvx + 5, cy - 30 + 15);
        glEnd();
    }
    glPointSize(1.5f);
    glBegin(GL_POINTS);
    for (auto& p : particulasPolvo) {
        float vn = p.vida / p.vidaMax;
        float al = (vn < 0.2f) ? vn / 0.2f : (vn > 0.8f) ? (1.0f - vn) / 0.2f : 0.5f;
        glColor4f(0.8f, 0.7f, 0.5f, al * 0.6f);
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

void fondoTeatro() {
    glBegin(GL_QUADS);
    glColor3f(0.01f, 0.01f, 0.03f); glVertex2f(0, 0);
    glColor3f(0.02f, 0.01f, 0.05f); glVertex2f(800, 0);
    glColor3f(0.03f, 0.01f, 0.07f); glVertex2f(800, 600);
    glColor3f(0.01f, 0.01f, 0.04f); glVertex2f(0, 600);
    glEnd();
    glColor3f(0.08f, 0.05f, 0.02f);
    glBegin(GL_QUADS); glVertex2f(0, 0); glVertex2f(800, 0); glVertex2f(800, 120); glVertex2f(0, 120); glEnd();
    glColor3f(0.4f, 0.3f, 0.1f); glLineWidth(2);
    glBegin(GL_LINES); glVertex2f(0, 120); glVertex2f(800, 120); glEnd();
    for (int i = 0; i < 10; i++) {
        float lx = 40 + i * 80;
        float br = 0.7f + 0.3f * sin(tiempo * 0.015f + i * 0.7f);
        glColor3f(0.15f, 0.1f, 0.05f); glLineWidth(1);
        glBegin(GL_LINES); glVertex2f(lx, 600); glVertex2f(lx, 565); glEnd();
        glColor3f(br, br * 0.9f, br * 0.4f); circulo(lx, 562, 7, 12);
        glColor4f(br, br * 0.7f, 0.1f, 0.15f); circulo(lx, 562, 20, 16);
    }
    glColor3f(0.25f, 0.04f, 0.04f);
    glBegin(GL_QUADS); glVertex2f(0, 120); glVertex2f(80, 120); glVertex2f(80, 600); glVertex2f(0, 600); glEnd();
    glColor3f(0.18f, 0.02f, 0.02f);
    for (int i = 0; i < 4; i++) {
        float px = 15 + i * 18;
        glBegin(GL_QUADS);
        glVertex2f(px, 120); glVertex2f(px + 10, 120); glVertex2f(px + 6, 600); glVertex2f(px - 4, 600);
        glEnd();
    }
    glColor3f(0.7f, 0.55f, 0.1f); glLineWidth(3);
    glBegin(GL_LINES); glVertex2f(80, 120); glVertex2f(80, 600); glEnd();
    glColor3f(0.25f, 0.04f, 0.04f);
    glBegin(GL_QUADS); glVertex2f(720, 120); glVertex2f(800, 120); glVertex2f(800, 600); glVertex2f(720, 600); glEnd();
    glColor3f(0.18f, 0.02f, 0.02f);
    for (int i = 0; i < 4; i++) {
        float px = 725 + i * 18;
        glBegin(GL_QUADS);
        glVertex2f(px, 120); glVertex2f(px + 10, 120); glVertex2f(px + 14, 600); glVertex2f(px + 4, 600);
        glEnd();
    }
    glColor3f(0.7f, 0.55f, 0.1f); glLineWidth(3);
    glBegin(GL_LINES); glVertex2f(720, 120); glVertex2f(720, 600); glEnd();
    glColor3f(0.22f, 0.03f, 0.03f);
    glBegin(GL_QUADS); glVertex2f(0, 550); glVertex2f(800, 550); glVertex2f(800, 600); glVertex2f(0, 600); glEnd();
    glColor3f(0.7f, 0.5f, 0.08f);
    for (int i = 0; i < 40; i++) {
        float fx = 10 + i * 20, flen = 12 + sin(tiempo * 0.02f + i * 0.4f) * 4;
        glLineWidth(1);
        glBegin(GL_LINES); glVertex2f(fx, 550); glVertex2f(fx, 550 - flen); glEnd();
    }
    glColor3f(0.8f, 0.6f, 0.1f); glLineWidth(2);
    glBegin(GL_LINES); glVertex2f(0, 550); glVertex2f(800, 550); glEnd();
    float ckx = 400, cky = 575;
    glColor3f(0.75f, 0.7f, 0.6f); circulo(ckx, cky + 5, 14, 20);
    glBegin(GL_QUADS);
    glVertex2f(ckx - 10, cky - 5); glVertex2f(ckx + 10, cky - 5);
    glVertex2f(ckx + 8, cky - 14); glVertex2f(ckx - 8, cky - 14);
    glEnd();
    glColor3f(0.05f, 0.02f, 0.02f);
    circulo(ckx - 5, cky + 7, 3, 8); circulo(ckx + 5, cky + 7, 3, 8);
    glBegin(GL_TRIANGLES); glVertex2f(ckx, cky + 1); glVertex2f(ckx - 2, cky - 3); glVertex2f(ckx + 2, cky - 3); glEnd();
    glColor3f(0.75f, 0.7f, 0.6f);
    for (int i = 0; i < 4; i++) {
        float tx = ckx - 6 + i * 4;
        glBegin(GL_QUADS);
        glVertex2f(tx, -2 + cky - 14); glVertex2f(tx + 3, -2 + cky - 14);
        glVertex2f(tx + 3, -6 + cky - 14); glVertex2f(tx, -6 + cky - 14);
        glEnd();
    }
    antorchaGrande(100, 140); antorchaGrande(700, 140);
    for (int i = 0; i < 8; i++) {
        float bx = i * 110 + sin(tiempo * 0.002f + i) * 20;
        float br = 0.03f + 0.02f * sin(tiempo * 0.01f + i * 0.5f);
        glColor4f(0.5f, 0.5f, 0.7f, br);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(bx + 30, 120);
        for (int j = 0; j <= 20; j++) {
            float a = j * M_PI / 20;
            glVertex2f(bx + 30 + cos(a) * 55, 120 + sin(a) * 20);
        }
        glEnd();
    }
    glPointSize(1.5f);
    glBegin(GL_POINTS);
    for (auto& p : particulasTeatro) {
        float vn = p.vida / p.vidaMax;
        float al = (vn < 0.2f) ? vn / 0.2f : (vn > 0.8f) ? (1.0f - vn) / 0.2f : 1.0f;
        glColor4f(p.r * al, p.g * al, p.b * al, al * 0.8f);
        glVertex2f(p.x, p.y);
    }
    glEnd();
}

// ============================================================
// FONDO SALA DE ÓRGANO — MENÚ MÚSICA
// ============================================================
void fondoOrgano() {
    // Cielo nocturno azul profundo con toque morado
    glBegin(GL_QUADS);
    glColor3f(0.01f, 0.01f, 0.06f); glVertex2f(0, 0);
    glColor3f(0.02f, 0.01f, 0.08f); glVertex2f(800, 0);
    glColor3f(0.04f, 0.02f, 0.12f); glVertex2f(800, 600);
    glColor3f(0.02f, 0.01f, 0.08f); glVertex2f(0, 600);
    glEnd();

    // Vidriera central (rosetón gótico) — fondo superior
    float rx = 400, ry = 555, rr = 55;
    // Halo externo con pulso
    float hpulse = 0.06f + 0.03f * sin(tiempo * 0.008f);
    glColor4f(0.3f, 0.1f, 0.6f, hpulse);
    circulo(rx, ry, rr + 30, 40);
    // Anillos del rosetón
    glColor4f(0.5f, 0.2f, 0.9f, 0.25f); circulo(rx, ry, rr, 40);
    glColor4f(0.7f, 0.3f, 1.0f, 0.15f); circulo(rx, ry, rr - 12, 40);
    // Pétalos del rosetón
    for (int i = 0; i < 12; i++) {
        float a = i * 2 * M_PI / 12;
        float a2 = (i + 1) * 2 * M_PI / 12;
        float r1 = rr - 22, r2 = rr - 4;
        float pr = 0.35f + 0.15f * sin(tiempo * 0.01f + i * 0.5f);
        float pg = 0.1f + 0.08f * sin(tiempo * 0.013f + i * 0.3f);
        float pb = 0.7f + 0.2f * sin(tiempo * 0.009f + i * 0.7f);
        glColor4f(pr, pg, pb, 0.4f);
        glBegin(GL_TRIANGLES);
        glVertex2f(rx, ry);
        glVertex2f(rx + cos(a) * r2, ry + sin(a) * r2);
        glVertex2f(rx + cos(a2) * r2, ry + sin(a2) * r2);
        glEnd();
        glColor4f(pr * 0.6f, pg * 0.6f, pb * 0.6f, 0.2f);
        glBegin(GL_TRIANGLES);
        glVertex2f(rx + cos(a) * r1, ry + sin(a) * r1);
        glVertex2f(rx + cos(a2) * r1, ry + sin(a2) * r1);
        glVertex2f(rx + cos((a + a2) / 2) * (r2 + 4), ry + sin((a + a2) / 2) * (r2 + 4));
        glEnd();
    }
    // Borde del rosetón
    glColor3f(0.5f, 0.3f, 0.8f); glLineWidth(2);
    circuloLinea(rx, ry, rr, 40);
    circuloLinea(rx, ry, rr - 12, 40);
    // Cruz central del rosetón
    glColor3f(0.6f, 0.4f, 0.9f); glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(rx - rr, ry); glVertex2f(rx + rr, ry);
    glVertex2f(rx, ry - rr); glVertex2f(rx, ry + rr);
    glEnd();

    // Arcos góticos de fondo (ventanas)
    for (int w = 0; w < 5; w++) {
        float wx = 80 + w * 160, wbase = 100, wheight = 320;
        float ww = 60;
        // Relleno vidriera con color animado
        float wr = 0.05f + 0.03f * sin(tiempo * 0.006f + w * 0.8f);
        float wg = 0.02f + 0.02f * sin(tiempo * 0.009f + w * 0.5f);
        float wb = 0.12f + 0.06f * sin(tiempo * 0.007f + w * 1.1f);
        glColor4f(wr, wg, wb, 0.5f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(wx + ww / 2, wbase + wheight);
        for (int j = 0; j <= 20; j++) {
            float a = j * M_PI / 20;
            glVertex2f(wx + ww / 2 + cos(a) * ww / 2, wbase + wheight - 10 + sin(a) * ww / 2);
        }
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(wx, wbase); glVertex2f(wx + ww, wbase);
        glVertex2f(wx + ww, wbase + wheight - ww / 2); glVertex2f(wx, wbase + wheight - ww / 2);
        glEnd();
        // Marcos de la ventana
        glColor3f(0.15f, 0.08f, 0.2f); glLineWidth(2);
        glBegin(GL_LINE_STRIP);
        glVertex2f(wx, wbase);
        glVertex2f(wx, wbase + wheight - ww / 2);
        for (int j = 0; j <= 20; j++) {
            float a = j * M_PI / 20;
            glVertex2f(wx + ww / 2 + cos(a) * ww / 2, wbase + wheight - ww / 2 + sin(a) * ww / 2);
        }
        glVertex2f(wx + ww, wbase + wheight - ww / 2);
        glVertex2f(wx + ww, wbase);
        glEnd();
        // Parteluz central
        glColor3f(0.12f, 0.06f, 0.18f); glLineWidth(1.5f);
        glBegin(GL_LINES);
        glVertex2f(wx + ww / 2, wbase); glVertex2f(wx + ww / 2, wbase + wheight - ww / 2);
        glEnd();
        // Travesaño
        glBegin(GL_LINES);
        glVertex2f(wx, wbase + wheight * 0.45f); glVertex2f(wx + ww, wbase + wheight * 0.45f);
        glEnd();
    }

    // Tubo del órgano izquierdo (grande)
    auto tuboOrgano = [](float x, float base, float h, float w) {
        glColor3f(0.2f, 0.15f, 0.25f);
        glBegin(GL_QUADS);
        glVertex2f(x, base); glVertex2f(x + w, base);
        glVertex2f(x + w, base + h); glVertex2f(x, base + h);
        glEnd();
        // Reflejo metálico
        glColor3f(0.35f, 0.28f, 0.42f);
        glBegin(GL_QUADS);
        glVertex2f(x + 2, base + h * 0.1f); glVertex2f(x + w * 0.3f, base + h * 0.1f);
        glVertex2f(x + w * 0.3f, base + h * 0.9f); glVertex2f(x + 2, base + h * 0.9f);
        glEnd();
        // Borde
        glColor3f(0.45f, 0.35f, 0.55f); glLineWidth(1);
        glBegin(GL_LINE_LOOP);
        glVertex2f(x, base); glVertex2f(x + w, base);
        glVertex2f(x + w, base + h); glVertex2f(x, base + h);
        glEnd();
        // Parte superior redondeada
        glColor3f(0.3f, 0.22f, 0.38f);
        circulo(x + w / 2, base + h, w / 2, 12);
        };

    // Tubos lado izquierdo
    float tuboBaseL = 100;
    tuboOrgano(15, tuboBaseL, 420, 28);
    tuboOrgano(47, tuboBaseL, 370, 24);
    tuboOrgano(75, tuboBaseL, 310, 20);
    tuboOrgano(99, tuboBaseL, 260, 18);
    tuboOrgano(121, tuboBaseL, 210, 16);
    tuboOrgano(141, tuboBaseL, 175, 14);

    // Tubos lado derecho (espejo)
    float tuboBaseR = 100;
    tuboOrgano(757, tuboBaseR, 420, 28);
    tuboOrgano(729, tuboBaseR, 370, 24);
    tuboOrgano(705, tuboBaseR, 310, 20);
    tuboOrgano(683, tuboBaseR, 260, 18);
    tuboOrgano(663, tuboBaseR, 210, 16);
    tuboOrgano(645, tuboBaseR, 175, 14);

    // Base / consola del órgano
    glColor3f(0.12f, 0.08f, 0.18f);
    glBegin(GL_QUADS);
    glVertex2f(0, 95); glVertex2f(800, 95);
    glVertex2f(800, 105); glVertex2f(0, 105);
    glEnd();
    glColor3f(0.3f, 0.2f, 0.4f); glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(0, 105); glVertex2f(800, 105);
    glEnd();

    // Suelo de mármol oscuro
    glColor3f(0.06f, 0.04f, 0.09f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(800, 0);
    glVertex2f(800, 100); glVertex2f(0, 100);
    glEnd();
    // Vetas del mármol
    glColor4f(0.15f, 0.1f, 0.2f, 0.4f);
    for (int i = 0; i < 6; i++) {
        float lx = 40 + i * 130 + sin(i * 2.3f) * 20;
        glBegin(GL_LINES);
        glVertex2f(lx, 0); glVertex2f(lx + 60 + sin(i) * 30, 100);
        glEnd();
    }

    // Partículas notas musicales flotantes
    Cancion& cact = canciones[cancionActual];
    for (auto& p : particulasNotas) {
        float vn = p.vida / p.vidaMax;
        float al = (vn < 0.15f) ? vn / 0.15f : (vn > 0.75f) ? (1.0f - vn) / 0.25f : 1.0f;
        glPushMatrix();
        glTranslatef(p.x, p.y, 0);
        glRotatef(p.rot, 0, 0, 1);
        if (p.tipo == 0) {
            // Corchea simplificada
            glColor4f(cact.r, cact.g, cact.b, al * 0.8f);
            circulo(0, 0, p.size, 8);
            glBegin(GL_LINES);
            glVertex2f(p.size * 0.8f, 0); glVertex2f(p.size * 0.8f, p.size * 3);
            glVertex2f(p.size * 0.8f, p.size * 3); glVertex2f(p.size * 2.2f, p.size * 2);
            glEnd();
        }
        else if (p.tipo == 1) {
            // Redonda
            glColor4f(cact.r, cact.g, cact.b, al * 0.6f);
            circuloLinea(0, 0, p.size, 10);
        }
        else {
            // Silencio (rectángulo pequeño)
            glColor4f(cact.r * 0.7f, cact.g * 0.7f, cact.b * 0.7f, al * 0.5f);
            glBegin(GL_QUADS);
            glVertex2f(-p.size, -p.size * 0.4f); glVertex2f(p.size, -p.size * 0.4f);
            glVertex2f(p.size, p.size * 0.4f);  glVertex2f(-p.size, p.size * 0.4f);
            glEnd();
        }
        glPopMatrix();
    }
}

// ============================================================
// ICONOS
// ============================================================
void iconoEspada(float x, float y) {
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    glVertex2f(x, y + 10); glVertex2f(x, y - 10);
    glVertex2f(x - 5, y - 5); glVertex2f(x + 5, y - 5);
    glEnd();
}
void iconoOpciones(float x, float y) {
    circuloLinea(x, y, 8, 20);
    for (int i = 0; i < 8; i++) {
        float a = i * 2 * M_PI / 8;
        glBegin(GL_LINES);
        glVertex2f(x + cos(a) * 8, y + sin(a) * 8);
        glVertex2f(x + cos(a) * 11, y + sin(a) * 11);
        glEnd();
    }
}
void iconoLibro(float x, float y) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 8, y + 8); glVertex2f(x + 8, y + 8);
    glVertex2f(x + 8, y - 8); glVertex2f(x - 8, y - 8);
    glEnd();
    glBegin(GL_LINES); glVertex2f(x, y + 8); glVertex2f(x, y - 8); glEnd();
}
void iconoEscudos(float x, float y) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(x - 10, y + 8); glVertex2f(x - 4, y + 8);
    glVertex2f(x - 4, y); glVertex2f(x - 7, y - 6); glVertex2f(x - 10, y);
    glEnd();
    glBegin(GL_LINE_LOOP);
    glVertex2f(x + 4, y + 8); glVertex2f(x + 10, y + 8);
    glVertex2f(x + 10, y); glVertex2f(x + 7, y - 6); glVertex2f(x + 4, y);
    glEnd();
}
void iconoIA(float x, float y) {
    circuloLinea(x, y, 8, 20);
    glBegin(GL_LINES);
    glVertex2f(x - 4, y); glVertex2f(x + 4, y);
    glVertex2f(x, y - 4); glVertex2f(x, y + 4);
    glEnd();
    for (int i = 0; i < 8; i++) {
        float a = i * 2 * M_PI / 8;
        glBegin(GL_LINES);
        glVertex2f(x + cos(a) * 8, y + sin(a) * 8);
        glVertex2f(x + cos(a) * 11, y + sin(a) * 11);
        glEnd();
    }
}
// Icono nota musical
void iconoNota(float x, float y) {
    glLineWidth(1.5f);
    circulo(x, y, 4, 8);
    glBegin(GL_LINES);
    glVertex2f(x + 3.2f, y); glVertex2f(x + 3.2f, y + 10);
    glVertex2f(x + 3.2f, y + 10); glVertex2f(x + 9, y + 7);
    glEnd();
}

// ============================================================
// PANELES
// ============================================================
void panelMenu(float y) {
    glColor3f(0.8f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(250, y + 25); glVertex2f(600, y + 25);
    glVertex2f(640, y - 25); glVertex2f(290, y - 25);
    glEnd();
}
void panelBatalla(float y, bool sel) {
    if (sel) {
        glColor3f(0.4f, 0.0f, 0.0f);
        glBegin(GL_QUADS);
        glVertex2f(155, y + 30); glVertex2f(660, y + 30);
        glVertex2f(700, y - 30); glVertex2f(195, y - 30);
        glEnd();
        glColor3f(0.85f, 0.05f, 0.05f);
        glBegin(GL_QUADS);
        glVertex2f(150, y + 28); glVertex2f(655, y + 28);
        glVertex2f(695, y - 28); glVertex2f(190, y - 28);
        glEnd();
        glColor3f(1.0f, 0.3f, 0.3f);
        glBegin(GL_QUADS);
        glVertex2f(150, y + 20); glVertex2f(655, y + 20);
        glVertex2f(655, y + 28); glVertex2f(150, y + 28);
        glEnd();
    }
    else {
        glColor3f(0.15f, 0.05f, 0.05f);
        glBegin(GL_QUADS);
        glVertex2f(150, y + 25); glVertex2f(655, y + 25);
        glVertex2f(695, y - 25); glVertex2f(190, y - 25);
        glEnd();
    }
}

// ============================================================
// CURSOR ANIMADO
// ============================================================
void cursor(float x, float y) {
    float s = 10 + sin(tiempo * 0.008f) * 3;
    float rot = sin(tiempo * 0.008f) * 15;
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glRotatef(rot, 0, 0, 1);
    glBegin(GL_TRIANGLES);
    glVertex2f(0, 0); glVertex2f(-s, s); glVertex2f(-s, -s);
    glEnd();
    glPopMatrix();
}

// ============================================================
// SPRITES DE TROPAS (procedurales)
// ============================================================
void spriteCaballero(float x, float y, float esc) {
    glPushMatrix(); glTranslatef(x, y, 0); glScalef(esc, esc, 1);
    glColor3f(0.7f, 0.75f, 0.8f);
    glBegin(GL_QUADS); glVertex2f(-12, -20); glVertex2f(12, -20); glVertex2f(12, 10); glVertex2f(-12, 10); glEnd();
    glColor3f(0.6f, 0.65f, 0.7f); circulo(0, 18, 10, 16);
    glColor3f(0.4f, 0.4f, 0.45f);
    glBegin(GL_QUADS); glVertex2f(-7, 14); glVertex2f(7, 14); glVertex2f(6, 10); glVertex2f(-6, 10); glEnd();
    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_QUADS); glVertex2f(-18, -5); glVertex2f(-10, -5); glVertex2f(-10, 8); glVertex2f(-18, 8); glEnd();
    glColor3f(1, 0.9f, 0.1f); glLineWidth(1);
    glBegin(GL_LINES); glVertex2f(-18, 1); glVertex2f(-10, 1); glEnd();
    glColor3f(0.85f, 0.85f, 0.9f);
    glBegin(GL_QUADS); glVertex2f(10, -5); glVertex2f(13, -5); glVertex2f(13, 20); glVertex2f(10, 20); glEnd();
    glColor3f(0.55f, 0.6f, 0.65f);
    glBegin(GL_QUADS); glVertex2f(-10, -35); glVertex2f(-2, -35); glVertex2f(-2, -20); glVertex2f(-10, -20); glEnd();
    glBegin(GL_QUADS); glVertex2f(2, -35); glVertex2f(10, -35); glVertex2f(10, -20); glVertex2f(2, -20); glEnd();
    glPopMatrix();
}
void spriteArquero(float x, float y, float esc) {
    glPushMatrix(); glTranslatef(x, y, 0); glScalef(esc, esc, 1);
    glColor3f(0.4f, 0.25f, 0.1f);
    glBegin(GL_QUADS); glVertex2f(-10, -20); glVertex2f(10, -20); glVertex2f(8, 8); glVertex2f(-8, 8); glEnd();
    glColor3f(0.75f, 0.6f, 0.45f); circulo(0, 16, 9, 16);
    glColor3f(0.25f, 0.15f, 0.05f);
    glBegin(GL_TRIANGLES); glVertex2f(0, 28); glVertex2f(-9, 14); glVertex2f(9, 14); glEnd();
    glColor3f(0.55f, 0.35f, 0.1f); glLineWidth(2);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 12; i++) {
        float a = (float)i / 12 * M_PI - M_PI / 2;
        glVertex2f(16 + cos(a) * 14, sin(a) * 18);
    }
    glEnd();
    glColor3f(0.9f, 0.85f, 0.7f); glLineWidth(1);
    glBegin(GL_LINES); glVertex2f(16, -18); glVertex2f(16, 18); glEnd();
    glColor3f(0.6f, 0.4f, 0.15f);
    glBegin(GL_LINES); glVertex2f(-10, 2); glVertex2f(16, 2); glEnd();
    glColor3f(0.7f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES); glVertex2f(-10, 2); glVertex2f(-5, 5); glVertex2f(-5, -1); glEnd();
    glColor3f(0.3f, 0.2f, 0.08f);
    glBegin(GL_QUADS); glVertex2f(-8, -35); glVertex2f(-2, -35); glVertex2f(-2, -20); glVertex2f(-8, -20); glEnd();
    glBegin(GL_QUADS); glVertex2f(2, -35); glVertex2f(8, -35); glVertex2f(8, -20); glVertex2f(2, -20); glEnd();
    glPopMatrix();
}
void spriteOrco(float x, float y, float esc) {
    glPushMatrix(); glTranslatef(x, y, 0); glScalef(esc, esc, 1);
    glColor3f(0.2f, 0.35f, 0.1f);
    glBegin(GL_QUADS); glVertex2f(-16, -22); glVertex2f(16, -22); glVertex2f(14, 12); glVertex2f(-14, 12); glEnd();
    glColor3f(0.25f, 0.4f, 0.12f); circulo(0, 22, 13, 16);
    glColor3f(0.9f, 0.85f, 0.7f);
    glBegin(GL_TRIANGLES);
    glVertex2f(-5, 15); glVertex2f(-3, 15); glVertex2f(-4, 9);
    glVertex2f(3, 15); glVertex2f(5, 15); glVertex2f(4, 9);
    glEnd();
    glColor3f(0.9f, 0.1f, 0.05f); circulo(-5, 25, 3, 8); circulo(5, 25, 3, 8);
    glColor3f(0.45f, 0.3f, 0.1f);
    glBegin(GL_QUADS); glVertex2f(14, -10); glVertex2f(17, -10); glVertex2f(17, 25); glVertex2f(14, 25); glEnd();
    glColor3f(0.5f, 0.5f, 0.5f); circulo(15.5f, 27, 6, 10);
    glColor3f(0.18f, 0.28f, 0.08f);
    glBegin(GL_QUADS); glVertex2f(-14, -38); glVertex2f(-4, -38); glVertex2f(-4, -22); glVertex2f(-14, -22); glEnd();
    glBegin(GL_QUADS); glVertex2f(4, -38); glVertex2f(14, -38); glVertex2f(14, -22); glVertex2f(4, -22); glEnd();
    glPopMatrix();
}
void spriteNigromante(float x, float y, float esc) {
    glPushMatrix(); glTranslatef(x, y, 0); glScalef(esc, esc, 1);
    glColor3f(0.1f, 0.05f, 0.15f);
    glBegin(GL_TRIANGLES); glVertex2f(-14, -28); glVertex2f(14, -28); glVertex2f(0, 14); glEnd();
    glBegin(GL_QUADS); glVertex2f(-8, -28); glVertex2f(8, -28); glVertex2f(8, 14); glVertex2f(-8, 14); glEnd();
    glColor3f(0.12f, 0.06f, 0.18f); circulo(0, 20, 10, 16);
    glBegin(GL_TRIANGLES); glVertex2f(0, 36); glVertex2f(-10, 18); glVertex2f(10, 18); glEnd();
    glColor3f(0.5f, 0.0f, 0.8f); circulo(-4, 21, 2.5f, 8); circulo(4, 21, 2.5f, 8);
    glColor3f(0.35f, 0.25f, 0.1f);
    glBegin(GL_QUADS); glVertex2f(10, -26); glVertex2f(13, -26); glVertex2f(13, 28); glVertex2f(10, 28); glEnd();
    glColor4f(0.4f, 0.0f, 0.9f, 0.8f); circulo(11.5f, 33, 7, 14);
    glColor4f(0.7f, 0.3f, 1.0f, 0.4f); circulo(11.5f, 33, 11, 14);
    float rr = 0.4f + 0.4f * sin(tiempo * 0.02f);
    glColor3f(rr, 0, rr * 1.5f); glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 6; i++) {
        float a = i * 2 * M_PI / 6 + tiempo * 0.01f;
        glVertex2f(11.5f + cos(a) * 15, 33 + sin(a) * 8);
    }
    glEnd();
    glPopMatrix();
}
void dibujarSprite(int idx, float x, float y, float esc) {
    switch (idx) {
    case 0: spriteCaballero(x, y, esc); break;
    case 1: spriteArquero(x, y, esc);   break;
    case 2: spriteOrco(x, y, esc);      break;
    case 3: spriteNigromante(x, y, esc); break;
    }
}

// ============================================================
// ENCICLOPEDIA — LIBRO ANTIGUO
// ============================================================
void enciclopedia() {
    fondoBiblioteca();
    float ox = encicloOffset;
    glColor4f(0, 0, 0, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(78 + ox, 65); glVertex2f(728 + ox, 65); glVertex2f(728 + ox, 558); glVertex2f(78 + ox, 558);
    glEnd();
    glColor3f(0.22f, 0.13f, 0.04f);
    glBegin(GL_QUADS);
    glVertex2f(75 + ox, 68); glVertex2f(725 + ox, 68); glVertex2f(725 + ox, 555); glVertex2f(75 + ox, 555);
    glEnd();
    glColor3f(0.65f, 0.5f, 0.08f); glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glVertex2f(75 + ox, 68); glVertex2f(725 + ox, 68); glVertex2f(725 + ox, 555); glVertex2f(75 + ox, 555);
    glEnd();
    float qxs[2] = { 75 + ox,725 + ox }, qys[2] = { 68,555 };
    for (int ci = 0; ci < 2; ci++) for (int cj = 0; cj < 2; cj++) {
        glColor3f(0.75f, 0.6f, 0.1f); glLineWidth(2);
        circuloLinea(qxs[ci], qys[cj], 10, 8);
    }
    glColor3f(0.15f, 0.08f, 0.02f);
    glBegin(GL_QUADS);
    glVertex2f(390 + ox, 68); glVertex2f(410 + ox, 68); glVertex2f(410 + ox, 555); glVertex2f(390 + ox, 555);
    glEnd();
    glColor3f(0.55f, 0.4f, 0.07f); glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(390 + ox, 68); glVertex2f(390 + ox, 555);
    glVertex2f(410 + ox, 68); glVertex2f(410 + ox, 555);
    glEnd();
    for (int i = 0; i < 5; i++) {
        float sy = 110 + i * 88;
        glColor3f(0.65f, 0.5f, 0.08f);
        circuloLinea(400 + ox, sy, 8, 12);
    }
    glColor3f(0.82f, 0.75f, 0.58f);
    glBegin(GL_QUADS);
    glVertex2f(85 + ox, 78); glVertex2f(388 + ox, 78); glVertex2f(388 + ox, 545); glVertex2f(85 + ox, 545);
    glEnd();
    glColor3f(0.65f, 0.58f, 0.42f); glLineWidth(0.5f);
    for (int l = 0; l < 18; l++) {
        float ly = 100 + l * 24;
        glBegin(GL_LINES); glVertex2f(100 + ox, ly); glVertex2f(375 + ox, ly); glEnd();
    }
    glColor3f(0.5f, 0.35f, 0.07f); glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(92 + ox, 85); glVertex2f(381 + ox, 85); glVertex2f(381 + ox, 538); glVertex2f(92 + ox, 538);
    glEnd();
    glColor3f(0.2f, 0.1f, 0.02f);
    texto18(130 + ox, 510, "CODEX ARCHON");
    glColor3f(0.45f, 0.3f, 0.06f); glLineWidth(1);
    glBegin(GL_LINES); glVertex2f(100 + ox, 500); glVertex2f(375 + ox, 500); glEnd();
    float flotacion = sin(tiempo * 0.012f) * 4;
    dibujarSprite(paginaLibro, 230 + ox, 350 + flotacion, 1.4f);
    DatosTropa& tropa = tropas[paginaLibro];
    bool esLuz = (tropa.bando == "Fuerzas de Luz");
    glColor3f(esLuz ? 0.7f : 0.5f, esLuz ? 0.55f : 0.1f, esLuz ? 0.1f : 0.4f);
    texto12(115 + ox, 248, tropa.bando);
    glColor3f(0.25f, 0.15f, 0.03f);
    texto12(115 + ox, 225, tropa.stat1);
    texto12(115 + ox, 207, tropa.stat2);
    texto12(115 + ox, 189, tropa.stat3);
    glColor3f(0.35f, 0.22f, 0.04f);
    texto12(220 + ox, 92, std::to_string(paginaLibro * 2 + 1));
    glColor3f(0.80f, 0.73f, 0.56f);
    glBegin(GL_QUADS);
    glVertex2f(412 + ox, 78); glVertex2f(715 + ox, 78); glVertex2f(715 + ox, 545); glVertex2f(412 + ox, 545);
    glEnd();
    glColor3f(0.63f, 0.56f, 0.40f); glLineWidth(0.5f);
    for (int l = 0; l < 18; l++) {
        float ly = 100 + l * 24;
        glBegin(GL_LINES); glVertex2f(425 + ox, ly); glVertex2f(702 + ox, ly); glEnd();
    }
    glColor3f(0.5f, 0.35f, 0.07f); glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(419 + ox, 85); glVertex2f(708 + ox, 85); glVertex2f(708 + ox, 538); glVertex2f(419 + ox, 538);
    glEnd();
    glColor3f(0.15f, 0.07f, 0.01f);
    texto24(430 + ox, 505, tropa.nombre);
    glColor3f(0.45f, 0.3f, 0.06f); glLineWidth(1);
    glBegin(GL_LINES); glVertex2f(425 + ox, 497); glVertex2f(702 + ox, 497); glEnd();
    glColor3f(0.2f, 0.12f, 0.03f);
    for (int i = 0; i < (int)tropa.descripcion.size(); i++)
        texto12(430 + ox, 470 - i * 22, tropa.descripcion[i]);
    float selX = 565 + ox, selY = 280;
    glColor3f(0.5f, 0.35f, 0.07f);
    circuloLinea(selX, selY, 45, 24);
    circuloLinea(selX, selY, 38, 24);
    if (esLuz) {
        glColor3f(0.7f, 0.6f, 0.15f);
        for (int k = 0; k < 2; k++) {
            glBegin(GL_LINE_LOOP);
            for (int i = 0; i < 6; i++) {
                float a = i * 2 * M_PI / 6 + k * M_PI / 6 - M_PI / 2;
                glVertex2f(selX + cos(a) * 30, selY + sin(a) * 30);
            }
            glEnd();
        }
    }
    else {
        glColor3f(0.5f, 0.05f, 0.05f);
        for (int i = 0; i < 5; i++) {
            float a1 = i * 2 * M_PI / 5 + M_PI / 2;
            float a2 = ((i + 2) % 5) * 2 * M_PI / 5 + M_PI / 2;
            glBegin(GL_LINES);
            glVertex2f(selX + cos(a1) * 30, selY + sin(a1) * 30);
            glVertex2f(selX + cos(a2) * 30, selY + sin(a2) * 30);
            glEnd();
        }
    }
    glColor3f(0.35f, 0.22f, 0.04f);
    texto12(555 + ox, 92, std::to_string(paginaLibro * 2 + 2));
    float alf = 0.5f + 0.5f * sin(tiempo * 0.015f);
    if (paginaLibro > 0) {
        glColor3f(alf * 0.7f, alf * 0.5f, alf * 0.2f);
        glBegin(GL_TRIANGLES);
        glVertex2f(95 + ox, 310); glVertex2f(110 + ox, 320); glVertex2f(110 + ox, 300);
        glEnd();
    }
    if (paginaLibro < TOTAL_PAGINAS - 1) {
        glColor3f(alf * 0.7f, alf * 0.5f, alf * 0.2f);
        glBegin(GL_TRIANGLES);
        glVertex2f(705 + ox, 310); glVertex2f(690 + ox, 320); glVertex2f(690 + ox, 300);
        glEnd();
    }
    float tpuls = 1.0f + sin(tiempo * 0.008f) * 0.02f;
    glPushMatrix();
    glTranslatef(400 + ox, 578, 0); glScalef(tpuls, tpuls, 1);
    glColor3f(0.75f, 0.6f, 0.15f);
    texto24(-75, -8, "ENCICLOPEDIA");
    glPopMatrix();
    instrucciones(245 + ox, 50, "A/D o clic: Cambiar pagina    ESC: Volver");
}

// ============================================================
// MENÚ PRINCIPAL
// ============================================================
void menuPrincipal() {
    fondoCastillo();
    dibujarParticulas();
    logo();
    antorcha(100, 300); antorcha(700, 300);
    for (int i = 0; i < (int)opcionesMenu.size(); i++) {
        float y = 350 - i * 70;
        float offX = std::min(menuOffset + i * 20, 0.0f);
        bool sel = (i == opcionMenuSel);
        glPushMatrix(); glTranslatef(offX, 0, 0);
        if (sel) {
            glColor3f(1, 0.1f, 0.2f); glLineWidth(2);
            float bx = sin(tiempo * 0.01f) * 5, by = sin(tiempo * 0.01f) * 3;
            glBegin(GL_LINE_LOOP);
            glVertex2f(250 + bx, y + 25 + by); glVertex2f(600 + bx, y + 25 + by);
            glVertex2f(640 + bx, y - 25 + by); glVertex2f(290 + bx, y - 25 + by);
            glEnd();
            panelMenu(y);
            float esc = 1.3f + sin(tiempo * 0.005f) * 0.05f;
            glPushMatrix(); glTranslatef(320, y, 0); glScalef(esc, esc, 1);
            glColor3f(0, 0, 0); texto18(2, -7, opcionesMenu[i]);
            glColor3f(1, 1, 1); texto18(0, -5, opcionesMenu[i]);
            glPopMatrix();
            cursor(290, y);
        }
        else {
            glColor3f(0.75f, 0.75f, 0.75f);
            texto18(320, y - 5, opcionesMenu[i]);
        }
        glColor3f(sel ? 1.0f : 0.7f, sel ? 1.0f : 0.7f, sel ? 1.0f : 0.7f);
        if (i == 0) iconoEspada(300, y);
        if (i == 1) iconoOpciones(300, y);
        if (i == 2) iconoLibro(300, y);
        glPopMatrix();
    }
    instrucciones(260, 50, "W/S: Navegar    ENTER: Confirmar");
}

// ============================================================
// MENÚ JUGAR
// ============================================================
void menuJugar() {
    fondoBatalla();
    antorchaGrande(80, 230); antorchaGrande(720, 230);
    float pt = 1.0f + sin(tiempo * 0.006f) * 0.03f;
    glPushMatrix();
    glTranslatef(400 + jugarOffset, 520, 0); glScalef(pt, pt, 1);
    glColor3f(0.5f, 0.0f, 0.0f); texto24(-128, -7, "ELIGE MODO DE BATALLA");
    glColor3f(0.95f, 0.82f, 0.2f); texto24(-130, -5, "ELIGE MODO DE BATALLA");
    glPopMatrix();
    float ly = 497;
    glColor3f(0.7f, 0.5f, 0.1f); glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(150 + jugarOffset, ly); glVertex2f(650 + jugarOffset, ly);
    glEnd();
    float dx = 400 + jugarOffset;
    glBegin(GL_LINE_LOOP);
    glVertex2f(dx, ly + 5); glVertex2f(dx + 5, ly); glVertex2f(dx, ly - 5); glVertex2f(dx - 5, ly);
    glEnd();
    for (int i = 0; i < (int)opcionesJugar.size(); i++) {
        float y = 390 - i * 100;
        float offX = std::min(jugarOffset + i * 30, 0.0f);
        bool sel = (i == opcionJugarSel);
        glPushMatrix(); glTranslatef(offX, 0, 0);
        if (sel) {
            glColor3f(1, 0.15f, 0.1f); glLineWidth(2);
            float bx = sin(tiempo * 0.01f) * 4, by = sin(tiempo * 0.013f) * 3;
            glBegin(GL_LINE_LOOP);
            glVertex2f(148 + bx, y + 32 + by); glVertex2f(658 + bx, y + 32 + by);
            glVertex2f(698 + bx, y - 32 + by); glVertex2f(188 + bx, y - 32 + by);
            glEnd();
        }
        panelBatalla(y, sel);
        if (sel) {
            float esc = 1.25f + sin(tiempo * 0.007f) * 0.04f;
            glPushMatrix(); glTranslatef(310, y + 5, 0); glScalef(esc, esc, 1);
            glColor3f(0, 0, 0); texto24(2, -7, opcionesJugar[i]);
            glColor3f(1, 1, 1); texto24(0, -5, opcionesJugar[i]);
            glPopMatrix();
            glColor3f(0.9f, 0.85f, 0.7f);
            std::string desc = descJugar[i]; float dy2 = y - 12; std::string linea;
            for (int ci = 0; ci <= (int)desc.size(); ci++) {
                if (ci == (int)desc.size() || desc[ci] == '\n') {
                    texto12(310, dy2, linea); dy2 -= 14; linea.clear();
                }
                else linea += desc[ci];
            }
            cursor(168, y);
        }
        else {
            glColor3f(0.7f, 0.5f, 0.5f);
            texto24(310, y - 8, opcionesJugar[i]);
        }
        glColor3f(sel ? 1.0f : 0.7f, sel ? 1.0f : 0.7f, sel ? 1.0f : 0.7f);
        if (i == 0) iconoEscudos(230, y);
        if (i == 1) iconoIA(230, y);
        glPopMatrix();
    }
    dibujarParticulas();
    instrucciones(240, 50, "W/S: Seleccionar    ENTER: Confirmar    ESC: Volver");
}

// ============================================================
// MENÚ OPCIONES — TEATRO
// ============================================================
void barraSlider(float x, float y, float w, int valor, int vmax, bool esBool) {
    glColor3f(0.1f, 0.05f, 0.05f);
    glBegin(GL_QUADS);
    glVertex2f(x, y - 5); glVertex2f(x + w, y - 5); glVertex2f(x + w, y + 5); glVertex2f(x, y + 5);
    glEnd();
    float fill = (float)valor / vmax * w;
    if (esBool) glColor3f(valor ? 0.1f : 0.4f, valor ? 0.6f : 0.1f, 0.05f);
    else { float t2 = (float)valor / vmax; glColor3f(0.6f + t2 * 0.3f, 0.2f + t2 * 0.3f, 0.05f); }
    glBegin(GL_QUADS);
    glVertex2f(x, y - 4); glVertex2f(x + fill, y - 4); glVertex2f(x + fill, y + 4); glVertex2f(x, y + 4);
    glEnd();
    glColor3f(0.6f, 0.45f, 0.08f); glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y - 5); glVertex2f(x + w, y - 5); glVertex2f(x + w, y + 5); glVertex2f(x, y + 5);
    glEnd();
    glColor3f(0.9f, 0.8f, 0.3f);
    glBegin(GL_QUADS);
    glVertex2f(x + fill - 3, y - 7); glVertex2f(x + fill + 3, y - 7);
    glVertex2f(x + fill + 3, y + 7); glVertex2f(x + fill - 3, y + 7);
    glEnd();
}

void menuOpciones() {
    fondoTeatro();
    float ox = opcionesOffset;
    glColor4f(0, 0, 0, 0.6f);
    glBegin(GL_QUADS);
    glVertex2f(108 + ox, 148); glVertex2f(698 + ox, 148); glVertex2f(698 + ox, 532); glVertex2f(108 + ox, 532);
    glEnd();
    glColor3f(0.07f, 0.04f, 0.02f);
    glBegin(GL_QUADS);
    glVertex2f(105 + ox, 150); glVertex2f(695 + ox, 150); glVertex2f(695 + ox, 530); glVertex2f(105 + ox, 530);
    glEnd();
    glColor3f(0.65f, 0.5f, 0.08f); glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glVertex2f(105 + ox, 150); glVertex2f(695 + ox, 150); glVertex2f(695 + ox, 530); glVertex2f(105 + ox, 530);
    glEnd();
    glColor3f(0.4f, 0.3f, 0.05f); glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(112 + ox, 157); glVertex2f(688 + ox, 157); glVertex2f(688 + ox, 523); glVertex2f(112 + ox, 523);
    glEnd();
    float pt = 1.0f + sin(tiempo * 0.007f) * 0.025f;
    glPushMatrix();
    glTranslatef(400 + ox, 508, 0); glScalef(pt, pt, 1);
    glColor3f(0.4f, 0.0f, 0.0f);   texto24(-72, -7, "OPCIONES");
    glColor3f(0.88f, 0.78f, 0.18f); texto24(-74, -5, "OPCIONES");
    glPopMatrix();
    glColor3f(0.55f, 0.4f, 0.07f); glLineWidth(2);
    glBegin(GL_LINES); glVertex2f(120 + ox, 492); glVertex2f(680 + ox, 492); glEnd();
    float dm = 400 + ox;
    glBegin(GL_LINE_LOOP);
    glVertex2f(dm, 497); glVertex2f(dm + 6, 492); glVertex2f(dm, 487); glVertex2f(dm - 6, 492);
    glEnd();

    // Botón especial: AJUSTES DE MÚSICA (primera fila destacada)
    float btnY = 468;
    bool btnSel = (opcionOpcionesSel == -1); // nunca -1, pero lo dibujamos siempre
    // Panel del botón musica
    float btnPulse = 0.12f + 0.06f * sin(tiempo * 0.012f);
    glColor4f(0.15f, 0.05f, 0.3f, btnPulse);
    glBegin(GL_QUADS);
    glVertex2f(115 + ox, btnY + 16); glVertex2f(685 + ox, btnY + 16);
    glVertex2f(685 + ox, btnY - 20); glVertex2f(115 + ox, btnY - 20);
    glEnd();
    glColor3f(0.6f, 0.3f, 0.9f); glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
    glVertex2f(115 + ox, btnY + 16); glVertex2f(685 + ox, btnY + 16);
    glVertex2f(685 + ox, btnY - 20); glVertex2f(115 + ox, btnY - 20);
    glEnd();
    glColor3f(0.8f, 0.6f, 1.0f);
    iconoNota(140 + ox, btnY + 2);
    texto18(160 + ox, btnY, "AJUSTES DE MUSICA");
    glColor3f(0.55f, 0.35f, 0.8f);
    texto12(590 + ox, btnY, "ENTER");

    // Separador
    glColor3f(0.3f, 0.15f, 0.45f); glLineWidth(1);
    glBegin(GL_LINES); glVertex2f(120 + ox, btnY - 22); glVertex2f(680 + ox, btnY - 22); glEnd();

    // Controles normales (desplazados hacia abajo)
    const float startY = 435, stepY = 46;
    for (int i = 0; i < (int)controlesOpciones.size(); i++) {
        float y = startY - i * stepY;
        OpcionControl& op = controlesOpciones[i];
        bool sel = (i == opcionOpcionesSel);
        if (sel) {
            float al = 0.15f + 0.08f * sin(tiempo * 0.015f);
            glColor4f(0.8f, 0.1f, 0.05f, al);
            glBegin(GL_QUADS);
            glVertex2f(115 + ox, y + 16); glVertex2f(685 + ox, y + 16);
            glVertex2f(685 + ox, y - 24); glVertex2f(115 + ox, y - 24);
            glEnd();
            glColor3f(0.9f, 0.2f, 0.05f); glLineWidth(1.5f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(115 + ox, y + 16); glVertex2f(685 + ox, y + 16);
            glVertex2f(685 + ox, y - 24); glVertex2f(115 + ox, y - 24);
            glEnd();
        }
        if (sel) glColor3f(1.0f, 0.9f, 0.5f); else glColor3f(0.65f, 0.55f, 0.35f);
        texto18(130 + ox, y, op.nombre);
        std::string valStr;
        if (op.esBool) valStr = (op.nombre == "Idioma") ? (op.valor == 0 ? "ES" : "EN") : (op.valor ? "ON" : "OFF");
        else valStr = std::to_string(op.valor);
        if (sel) glColor3f(1, 1, 0.4f); else glColor3f(0.6f, 0.5f, 0.3f);
        texto18(638 + ox, y, valStr);
        barraSlider(320 + ox, y + 4, 300, op.valor, op.max, op.esBool);
        if (sel) cursor(118 + ox, y + 4);
    }

    instrucciones(148 + ox, 163, "W/S: Navegar    A/D: Cambiar valor    ENTER: Musica    ESC: Volver");
}

// ============================================================
// MENÚ MÚSICA — SALA DE ÓRGANO GÓTICO
// ============================================================

// Botones de control de reproducción
void botonControl(float cx, float cy, float r, bool activo, float pr, float pg, float pb) {
    // Fondo
    glColor4f(pr * 0.3f, pg * 0.3f, pb * 0.3f, 0.7f);
    circulo(cx, cy, r, 20);
    // Borde
    if (activo) {
        glColor3f(pr, pg, pb);
    }
    else {
        glColor3f(pr * 0.6f, pg * 0.6f, pb * 0.6f);
    }
    glLineWidth(activo ? 2.5f : 1.5f);
    circuloLinea(cx, cy, r, 20);
}

void menuMusica() {
    fondoOrgano();
    float ox = musicaOffset;
    Cancion& cact = canciones[cancionActual];

    // ---- PANEL PRINCIPAL ----
    glColor4f(0.0f, 0.0f, 0.0f, 0.75f);
    glBegin(GL_QUADS);
    glVertex2f(48 + ox, 118); glVertex2f(758 + ox, 118);
    glVertex2f(758 + ox, 530); glVertex2f(48 + ox, 530);
    glEnd();
    glColor3f(0.35f, 0.15f, 0.55f); glLineWidth(3);
    glBegin(GL_LINE_LOOP);
    glVertex2f(45 + ox, 115); glVertex2f(755 + ox, 115);
    glVertex2f(755 + ox, 532); glVertex2f(45 + ox, 532);
    glEnd();
    // Borde interior dorado tenue
    glColor3f(cact.r * 0.5f, cact.g * 0.4f, cact.b * 0.6f); glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(52 + ox, 121); glVertex2f(748 + ox, 121);
    glVertex2f(748 + ox, 526); glVertex2f(52 + ox, 526);
    glEnd();

    // ---- TÍTULO ----
    float pt = 1.0f + sin(tiempo * 0.007f) * 0.02f;
    glPushMatrix();
    glTranslatef(400 + ox, 513, 0); glScalef(pt, pt, 1);
    glColor3f(cact.r * 0.5f, cact.g * 0.4f, cact.b * 0.5f);
    texto24(-82, -7, "SALA DE MUSICA");
    glColor4f(cact.r, cact.g, cact.b, 1.0f);
    texto24(-84, -5, "SALA DE MUSICA");
    glPopMatrix();

    // Línea separadora decorativa
    glColor3f(cact.r * 0.6f, cact.g * 0.5f, cact.b * 0.7f); glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(60 + ox, 497); glVertex2f(740 + ox, 497);
    glEnd();
    // Diamante en centro de la línea
    float dlx = 400 + ox;
    glBegin(GL_LINE_LOOP);
    glVertex2f(dlx, 503); glVertex2f(dlx + 7, 497); glVertex2f(dlx, 491); glVertex2f(dlx - 7, 497);
    glEnd();

    // ====================================================
    // PANEL IZQUIERDO: INFO DE CANCIÓN + REPRODUCCIÓN
    // ====================================================
    // Disco / vinilo giratorio
    float diskX = 180 + ox, diskY = 340, diskR = 65;
    float diskRot = reproduciendo ? tiempo * 0.8f : 0;
    glPushMatrix();
    glTranslatef(diskX, diskY, 0);
    glRotatef(diskRot, 0, 0, 1);
    // Disco base
    glColor3f(0.08f, 0.05f, 0.12f); circulo(0, 0, diskR, 40);
    // Surcos del vinilo
    for (int s = 1; s <= 5; s++) {
        glColor4f(0.18f, 0.1f, 0.25f, 0.6f);
        circuloLinea(0, 0, diskR * s / 6.0f, 36);
    }
    // Etiqueta central coloreada
    glColor3f(cact.r * 0.7f, cact.g * 0.6f, cact.b * 0.8f);
    circulo(0, 0, diskR * 0.32f, 20);
    // Cruz de centrado
    glColor3f(0.05f, 0.02f, 0.08f); glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(-diskR * 0.08f, 0); glVertex2f(diskR * 0.08f, 0);
    glVertex2f(0, -diskR * 0.08f); glVertex2f(0, diskR * 0.08f);
    glEnd();
    // Brillo del disco
    glColor4f(1, 1, 1, 0.08f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(-diskR * 0.2f, diskR * 0.3f);
    for (int i = 0; i <= 20; i++) {
        float a = i * M_PI / 20 + M_PI * 0.7f;
        glVertex2f(cos(a) * diskR, sin(a) * diskR);
    }
    glEnd();
    glPopMatrix();
    // Borde del disco
    glColor3f(cact.r * 0.5f, cact.g * 0.4f, cact.b * 0.7f); glLineWidth(2);
    circuloLinea(diskX, diskY, diskR, 40);

    // Aguja del tocadiscos
    float agujaA = -0.4f + sin(tiempo * 0.001f) * 0.05f;
    glColor3f(0.7f, 0.65f, 0.75f); glLineWidth(2);
    glBegin(GL_LINES);
    glVertex2f(diskX + diskR - 5, diskY + diskR + 15);
    glVertex2f(diskX + cos(agujaA) * (diskR * 0.5f), diskY + sin(agujaA) * (diskR * 0.5f));
    glEnd();
    glColor3f(0.8f, 0.7f, 0.85f);
    circulo(diskX + diskR - 5, diskY + diskR + 15, 5, 12);

    // Nombre de la canción
    glColor3f(cact.r, cact.g, cact.b);
    texto18(60 + ox, 265, cact.titulo);
    // Artista
    glColor3f(0.6f, 0.55f, 0.7f);
    texto12(60 + ox, 248, cact.artista);
    // Contexto
    glColor3f(0.4f, 0.35f, 0.5f);
    texto12(60 + ox, 232, "Suena en: " + cact.contexto);
    // Duracion
    glColor3f(0.5f, 0.45f, 0.6f);
    texto12(60 + ox, 216, "Duracion: " + cact.duracion);

    // Barra de progreso
    float progX = 60 + ox, progY = 200, progW = 290;
    // Fondo barra
    glColor3f(0.1f, 0.06f, 0.18f);
    glBegin(GL_QUADS);
    glVertex2f(progX, progY - 4); glVertex2f(progX + progW, progY - 4);
    glVertex2f(progX + progW, progY + 4); glVertex2f(progX, progY + 4);
    glEnd();
    // Relleno progreso
    float progFill = progresoCancion * progW;
    glColor3f(cact.r * 0.8f, cact.g * 0.7f, cact.b * 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(progX, progY - 3); glVertex2f(progX + progFill, progY - 3);
    glVertex2f(progX + progFill, progY + 3); glVertex2f(progX, progY + 3);
    glEnd();
    // Marcador de progreso
    glColor3f(1, 1, 1);
    circulo(progX + progFill, progY, 5, 10);
    // Borde
    glColor3f(cact.r * 0.6f, cact.g * 0.5f, cact.b * 0.7f); glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(progX, progY - 4); glVertex2f(progX + progW, progY - 4);
    glVertex2f(progX + progW, progY + 4); glVertex2f(progX, progY + 4);
    glEnd();
    // Tiempos
    glColor3f(0.5f, 0.45f, 0.6f);
    texto12(progX, progY - 15, "0:00");
    texto12(progX + progW - 25, progY - 15, cact.duracion);

    // ---- CONTROLES DE REPRODUCCIÓN ----
    float ctrlY = 165, ctrlCX = 200 + ox;
    float ctrlSpacing = 42;

    // Botón: Anterior |<
    botonControl(ctrlCX - ctrlSpacing * 2, ctrlY, 16, false, 0.6f, 0.5f, 0.8f);
    glColor3f(0.7f, 0.6f, 0.9f); glLineWidth(2);
    glBegin(GL_TRIANGLES);
    glVertex2f(ctrlCX - ctrlSpacing * 2 - 6, ctrlY);
    glVertex2f(ctrlCX - ctrlSpacing * 2 + 5, ctrlY + 7);
    glVertex2f(ctrlCX - ctrlSpacing * 2 + 5, ctrlY - 7);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(ctrlCX - ctrlSpacing * 2 - 7, ctrlY - 7);
    glVertex2f(ctrlCX - ctrlSpacing * 2 - 7, ctrlY + 7);
    glEnd();

    // Botón: Retroceder <<
    botonControl(ctrlCX - ctrlSpacing, ctrlY, 16, false, 0.6f, 0.5f, 0.8f);
    glColor3f(0.7f, 0.6f, 0.9f); glLineWidth(2);
    glBegin(GL_TRIANGLES);
    glVertex2f(ctrlCX - ctrlSpacing + 2, ctrlY);
    glVertex2f(ctrlCX - ctrlSpacing - 6, ctrlY + 6);
    glVertex2f(ctrlCX - ctrlSpacing - 6, ctrlY - 6);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(ctrlCX - ctrlSpacing + 8, ctrlY);
    glVertex2f(ctrlCX - ctrlSpacing, ctrlY + 6);
    glVertex2f(ctrlCX - ctrlSpacing, ctrlY - 6);
    glEnd();

    // Botón: Play/Pause (grande)
    botonControl(ctrlCX, ctrlY, 22, true, cact.r, cact.g, cact.b);
    glColor3f(cact.r, cact.g, cact.b); glLineWidth(2);
    if (reproduciendo) {
        // Pausa: dos barras
        glBegin(GL_QUADS);
        glVertex2f(ctrlCX - 8, ctrlY - 9); glVertex2f(ctrlCX - 3, ctrlY - 9);
        glVertex2f(ctrlCX - 3, ctrlY + 9); glVertex2f(ctrlCX - 8, ctrlY + 9);
        glEnd();
        glBegin(GL_QUADS);
        glVertex2f(ctrlCX + 3, ctrlY - 9); glVertex2f(ctrlCX + 8, ctrlY - 9);
        glVertex2f(ctrlCX + 8, ctrlY + 9); glVertex2f(ctrlCX + 3, ctrlY + 9);
        glEnd();
    }
    else {
        // Play: triángulo
        glBegin(GL_TRIANGLES);
        glVertex2f(ctrlCX - 6, ctrlY - 10);
        glVertex2f(ctrlCX - 6, ctrlY + 10);
        glVertex2f(ctrlCX + 10, ctrlY);
        glEnd();
    }

    // Botón: Avanzar >>
    botonControl(ctrlCX + ctrlSpacing, ctrlY, 16, false, 0.6f, 0.5f, 0.8f);
    glColor3f(0.7f, 0.6f, 0.9f); glLineWidth(2);
    glBegin(GL_TRIANGLES);
    glVertex2f(ctrlCX + ctrlSpacing - 2, ctrlY);
    glVertex2f(ctrlCX + ctrlSpacing + 6, ctrlY + 6);
    glVertex2f(ctrlCX + ctrlSpacing + 6, ctrlY - 6);
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(ctrlCX + ctrlSpacing - 8, ctrlY);
    glVertex2f(ctrlCX + ctrlSpacing, ctrlY + 6);
    glVertex2f(ctrlCX + ctrlSpacing, ctrlY - 6);
    glEnd();

    // Botón: Siguiente >|
    botonControl(ctrlCX + ctrlSpacing * 2, ctrlY, 16, false, 0.6f, 0.5f, 0.8f);
    glColor3f(0.7f, 0.6f, 0.9f); glLineWidth(2);
    glBegin(GL_TRIANGLES);
    glVertex2f(ctrlCX + ctrlSpacing * 2 + 6, ctrlY);
    glVertex2f(ctrlCX + ctrlSpacing * 2 - 5, ctrlY + 7);
    glVertex2f(ctrlCX + ctrlSpacing * 2 - 5, ctrlY - 7);
    glEnd();
    glBegin(GL_LINES);
    glVertex2f(ctrlCX + ctrlSpacing * 2 + 7, ctrlY - 7);
    glVertex2f(ctrlCX + ctrlSpacing * 2 + 7, ctrlY + 7);
    glEnd();

    // Botón REPETIR
    float repX = ctrlCX - ctrlSpacing * 3 + 8, repY = ctrlY;
    botonControl(repX, repY, 12, repetir, 0.5f, 0.5f, 0.9f);
    glColor3f(repetir ? 0.8f : 0.45f, repetir ? 0.8f : 0.45f, 1.0f); glLineWidth(1.5f);
    // Símbolo ↺
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 20; i++) {
        float a = i * 2 * M_PI / 20;
        glVertex2f(repX + cos(a) * 7, repY + sin(a) * 7);
    }
    glEnd();
    glBegin(GL_TRIANGLES);
    glVertex2f(repX + 7, repY + 1); glVertex2f(repX + 7, repY - 4); glVertex2f(repX + 4, repY - 1);
    glEnd();
    glColor3f(0.5f, 0.45f, 0.65f);
    texto12(repX - 8, repY - 20, "REP");

    // Botón ALEATORIO
    float rndX = ctrlCX + ctrlSpacing * 3 - 8, rndY = ctrlY;
    botonControl(rndX, rndY, 12, aleatorio, 0.5f, 0.9f, 0.5f);
    glColor3f(0.45f, aleatorio ? 0.9f : 0.45f, 0.45f); glLineWidth(1.5f);
    // Símbolo shuffle (dos líneas cruzadas)
    glBegin(GL_LINES);
    glVertex2f(rndX - 6, rndY - 4); glVertex2f(rndX + 6, rndY + 4);
    glVertex2f(rndX - 6, rndY + 4); glVertex2f(rndX + 6, rndY - 4);
    glEnd();
    glColor3f(0.4f, 0.55f, 0.4f);
    texto12(rndX - 8, rndY - 20, "RND");

    // Volumen (barra lateral)
    float volX = 60 + ox, volY = 130, volW = 180;
    glColor3f(0.45f, 0.35f, 0.6f);
    texto12(volX, volY + 12, "VOL");
    barraSlider(volX + 28, volY + 8, volW, volumenMusica, 10, false);

    // ====================================================
    // PANEL DERECHO: LISTA DE CANCIONES
    // ====================================================
    float listX = 370 + ox, listY = 475, listW = 365, listH = 30;
    // Fondo lista
    glColor4f(0.04f, 0.02f, 0.08f, 0.8f);
    glBegin(GL_QUADS);
    glVertex2f(listX, listY - (int)canciones.size() * listH - 5);
    glVertex2f(listX + listW, listY - (int)canciones.size() * listH - 5);
    glVertex2f(listX + listW, listY + 5);
    glVertex2f(listX, listY + 5);
    glEnd();
    glColor3f(0.3f, 0.15f, 0.5f); glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(listX, listY - (int)canciones.size() * listH - 5);
    glVertex2f(listX + listW, listY - (int)canciones.size() * listH - 5);
    glVertex2f(listX + listW, listY + 5);
    glVertex2f(listX, listY + 5);
    glEnd();

    // Cabecera lista
    glColor3f(0.5f, 0.3f, 0.75f);
    texto12(listX + 8, listY - 8, "#  TITULO                    DURACION");
    glColor3f(0.25f, 0.12f, 0.4f); glLineWidth(1);
    glBegin(GL_LINES);
    glVertex2f(listX, listY - 18); glVertex2f(listX + listW, listY - 18);
    glEnd();

    for (int i = 0; i < (int)canciones.size(); i++) {
        float fy = listY - 30 - i * listH;
        bool esActual = (i == cancionActual);
        bool esHover = (i == cancionHover);

        // Fondo fila
        if (esActual) {
            glColor4f(canciones[i].r * 0.3f, canciones[i].g * 0.25f, canciones[i].b * 0.4f, 0.7f);
        }
        else if (esHover) {
            glColor4f(0.15f, 0.08f, 0.25f, 0.5f);
        }
        else {
            glColor4f(0, 0, 0, 0);
        }
        glBegin(GL_QUADS);
        glVertex2f(listX + 2, fy - 8); glVertex2f(listX + listW - 2, fy - 8);
        glVertex2f(listX + listW - 2, fy + 14); glVertex2f(listX + 2, fy + 14);
        glEnd();

        // Borde fila activa
        if (esActual) {
            glColor3f(canciones[i].r * 0.8f, canciones[i].g * 0.7f, canciones[i].b * 0.9f);
            glLineWidth(1);
            glBegin(GL_LINE_LOOP);
            glVertex2f(listX + 2, fy - 8); glVertex2f(listX + listW - 2, fy - 8);
            glVertex2f(listX + listW - 2, fy + 14); glVertex2f(listX + 2, fy + 14);
            glEnd();
        }

        // Número + icono reproducción
        if (esActual && reproduciendo) {
            // Indicador animado de notas
            float barH = 4 + 3 * sin(tiempo * 0.03f + i);
            glColor3f(canciones[i].r, canciones[i].g, canciones[i].b);
            glBegin(GL_QUADS);
            glVertex2f(listX + 6, fy); glVertex2f(listX + 8, fy);
            glVertex2f(listX + 8, fy + barH); glVertex2f(listX + 6, fy + barH);
            glEnd();
            barH = 4 + 3 * sin(tiempo * 0.04f + i + 1);
            glBegin(GL_QUADS);
            glVertex2f(listX + 9, fy); glVertex2f(listX + 11, fy);
            glVertex2f(listX + 11, fy + barH); glVertex2f(listX + 9, fy + barH);
            glEnd();
            barH = 4 + 3 * sin(tiempo * 0.035f + i + 2);
            glBegin(GL_QUADS);
            glVertex2f(listX + 12, fy); glVertex2f(listX + 14, fy);
            glVertex2f(listX + 14, fy + barH); glVertex2f(listX + 12, fy + barH);
            glEnd();
        }
        else {
            if (esActual) glColor3f(canciones[i].r, canciones[i].g, canciones[i].b);
            else          glColor3f(0.4f, 0.35f, 0.5f);
            texto12(listX + 6, fy, std::to_string(i + 1));
        }

        // Título
        if (esActual) glColor3f(canciones[i].r, canciones[i].g, canciones[i].b);
        else if (esHover) glColor3f(0.75f, 0.65f, 0.9f);
        else glColor3f(0.6f, 0.55f, 0.7f);
        texto12(listX + 25, fy, canciones[i].titulo);

        // Duración (alineada a la derecha)
        if (esActual) glColor3f(canciones[i].r * 0.8f, canciones[i].g * 0.7f, canciones[i].b * 0.9f);
        else          glColor3f(0.4f, 0.35f, 0.5f);
        texto12(listX + listW - 38, fy, canciones[i].duracion);
    }

    // ====================================================
    // VISUALIZADOR DE ONDA (parte inferior)
    // ====================================================
    float vizX = 370 + ox, vizY = 128, vizW = 365, vizH = 60;
    // Fondo visualizador
    glColor4f(0.02f, 0.01f, 0.06f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(vizX, vizY); glVertex2f(vizX + vizW, vizY);
    glVertex2f(vizX + vizW, vizY + vizH); glVertex2f(vizX, vizY + vizH);
    glEnd();
    glColor3f(0.25f, 0.1f, 0.4f); glLineWidth(1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(vizX, vizY); glVertex2f(vizX + vizW, vizY);
    glVertex2f(vizX + vizW, vizY + vizH); glVertex2f(vizX, vizY + vizH);
    glEnd();
    // Línea central
    glColor4f(0.3f, 0.15f, 0.5f, 0.3f);
    glBegin(GL_LINES);
    glVertex2f(vizX, vizY + vizH / 2); glVertex2f(vizX + vizW, vizY + vizH / 2);
    glEnd();
    // Barras del espectro
    float bw = (vizW - 4) / NUM_BARRAS;
    for (int b = 0; b < NUM_BARRAS; b++) {
        float bx = vizX + 2 + b * bw;
        float bh = barrasViz[b];
        float t2 = (float)b / NUM_BARRAS;
        float br2 = cact.r * (0.5f + t2 * 0.5f);
        float bg2 = cact.g * (0.3f + t2 * 0.4f);
        float bb2 = cact.b * (0.6f + t2 * 0.4f);
        float alpha = reproduciendo ? 0.85f : 0.35f;
        glColor4f(br2, bg2, bb2, alpha);
        glBegin(GL_QUADS);
        glVertex2f(bx, vizY + vizH / 2 - bh);
        glVertex2f(bx + bw * 0.8f, vizY + vizH / 2 - bh);
        glVertex2f(bx + bw * 0.8f, vizY + vizH / 2 + bh);
        glVertex2f(bx, vizY + vizH / 2 + bh);
        glEnd();
        // Pico
        glColor4f(br2 + 0.2f, bg2 + 0.1f, bb2 + 0.1f, alpha * 0.7f);
        glBegin(GL_LINES);
        glVertex2f(bx, vizY + vizH / 2 - bh - 2);
        glVertex2f(bx + bw * 0.8f, vizY + vizH / 2 - bh - 2);
        glEnd();
    }
    // Label visualizador
    glColor3f(0.3f, 0.2f, 0.5f);
    texto12(vizX + 3, vizY + 2, "ESPECTRO");

    instrucciones(200 + ox, 120, "W/S: Cancion    ESPACIO: Play/Pause    R: Repetir    X: Aleatorio    A/D: Vol    ESC: Volver");
}

// ============================================================
// SLASH DE TRANSICIÓN
// ============================================================
void espadaSlash() {
    if (!slashActivo) return;
    glPushMatrix();
    glTranslatef(slashX, 300, 0); glRotatef(45, 0, 0, 1);
    glColor3f(0.9f, 0.9f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(-10, -200); glVertex2f(10, -200); glVertex2f(10, 200); glVertex2f(-10, 200);
    glEnd();
    glColor3f(0.4f, 0.2f, 0.0f);
    glBegin(GL_QUADS);
    glVertex2f(-20, -200); glVertex2f(20, -200); glVertex2f(20, -220); glVertex2f(-20, -220);
    glEnd();
    glPopMatrix();
    float alpha = std::min((slashX + 1000) / 900.0f, 1.0f);
    glColor4f(0, 0, 0, alpha);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f(800, 0); glVertex2f(800, 600); glVertex2f(0, 600);
    glEnd();
}

// ============================================================
// DISPLAY
// ============================================================
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    switch (estadoActual) {
    case MENU:         menuPrincipal(); break;
    case JUGAR:        menuJugar();     break;
    case OPCIONES:     menuOpciones();  break;
    case MUSICA:       menuMusica();    break;
    case ENCICLOPEDIA: enciclopedia();  break;
    default: break;
    }
    espadaSlash();
    glutSwapBuffers();
}

// ============================================================
// INPUT
// ============================================================
void iniciarTransicion(Estado destino) {
    if (slashActivo) return;
    slashActivo = true;
    siguienteEstado = destino;
}

void teclado(unsigned char key, int x, int y) {
    if (slashActivo) return;

    if (estadoActual == MENU) {
        if (key == 'w' || key == 'W') { opcionMenuSel--; if (opcionMenuSel < 0) opcionMenuSel = (int)opcionesMenu.size() - 1; }
        if (key == 's' || key == 'S') { opcionMenuSel++; if (opcionMenuSel >= (int)opcionesMenu.size()) opcionMenuSel = 0; }
        if (key == 13) {
            if (opcionMenuSel == 0) iniciarTransicion(JUGAR);
            if (opcionMenuSel == 1) iniciarTransicion(OPCIONES);
            if (opcionMenuSel == 2) iniciarTransicion(ENCICLOPEDIA);
        }
    }
    else if (estadoActual == JUGAR) {
        if (key == 'w' || key == 'W') { opcionJugarSel--; if (opcionJugarSel < 0) opcionJugarSel = (int)opcionesJugar.size() - 1; }
        if (key == 's' || key == 'S') { opcionJugarSel++; if (opcionJugarSel >= (int)opcionesJugar.size()) opcionJugarSel = 0; }
        if (key == 13) iniciarTransicion(opcionJugarSel == 0 ? JUGAR_PVP : JUGAR_IA);
        if (key == 27) iniciarTransicion(MENU);
    }
    else if (estadoActual == OPCIONES) {
        if (key == 'w' || key == 'W') { opcionOpcionesSel--; if (opcionOpcionesSel < 0) opcionOpcionesSel = (int)controlesOpciones.size() - 1; }
        if (key == 's' || key == 'S') { opcionOpcionesSel++; if (opcionOpcionesSel >= (int)controlesOpciones.size()) opcionOpcionesSel = 0; }
        if (key == 'a' || key == 'A') { OpcionControl& op = controlesOpciones[opcionOpcionesSel]; op.valor = std::max(op.min, op.valor - 1); }
        if (key == 'd' || key == 'D') { OpcionControl& op = controlesOpciones[opcionOpcionesSel]; op.valor = std::min(op.max, op.valor + 1); }
        if (key == 13)  iniciarTransicion(MUSICA);  // ENTER desde opciones abre música
        if (key == 27)  iniciarTransicion(MENU);
    }
    else if (estadoActual == MUSICA) {
        // Navegar canciones
        if (key == 'w' || key == 'W') {
            cancionHover--;
            if (cancionHover < 0) cancionHover = (int)canciones.size() - 1;
            cancionActual = cancionHover;
            progresoCancion = 0; reproduciendo = true;
        }
        if (key == 's' || key == 'S') {
            cancionHover++;
            if (cancionHover >= (int)canciones.size()) cancionHover = 0;
            cancionActual = cancionHover;
            progresoCancion = 0; reproduciendo = true;
        }
        // Play / Pause
        if (key == ' ') reproduciendo = !reproduciendo;
        // Repetir
        if (key == 'r' || key == 'R') repetir = !repetir;
        // Aleatorio
        if (key == 'x' || key == 'X') aleatorio = !aleatorio;
        // Volumen
        if (key == 'a' || key == 'A') { volumenMusica = std::max(0, volumenMusica - 1); controlesOpciones[0].valor = volumenMusica; }
        if (key == 'd' || key == 'D') { volumenMusica = std::min(10, volumenMusica + 1); controlesOpciones[0].valor = volumenMusica; }
        if (key == 27) iniciarTransicion(OPCIONES);
    }
    else if (estadoActual == ENCICLOPEDIA) {
        if (key == 'a' || key == 'A') { paginaLibro--; if (paginaLibro < 0) paginaLibro = TOTAL_PAGINAS - 1; }
        if (key == 'd' || key == 'D') { paginaLibro++; if (paginaLibro >= TOTAL_PAGINAS) paginaLibro = 0; }
        if (key == 27) iniciarTransicion(MENU);
    }
}

// Teclas especiales (flechas)
void tecladoEspecial(int key, int x, int y) {
    if (slashActivo) return;
    if (estadoActual == MUSICA) {
        if (key == GLUT_KEY_UP) { cancionHover--; if (cancionHover < 0) cancionHover = (int)canciones.size() - 1; cancionActual = cancionHover; progresoCancion = 0; }
        if (key == GLUT_KEY_DOWN) { cancionHover++; if (cancionHover >= (int)canciones.size()) cancionHover = 0; cancionActual = cancionHover; progresoCancion = 0; }
        if (key == GLUT_KEY_LEFT) { progresoCancion = std::max(0.0f, progresoCancion - 0.05f); }
        if (key == GLUT_KEY_RIGHT) { progresoCancion = std::min(1.0f, progresoCancion + 0.05f); }
    }
}

void mouse(int b, int s, int mx, int my) {
    my = 600 - my;
    if (b != GLUT_LEFT_BUTTON || s != GLUT_DOWN) return;

    if (estadoActual == MENU) {
        for (int i = 0; i < (int)opcionesMenu.size(); i++) {
            float oy = 350 - i * 70;
            if (mx > 250 && mx<640 && my>oy - 25 && my < oy + 25) {
                opcionMenuSel = i;
                if (i == 0) iniciarTransicion(JUGAR);
                if (i == 1) iniciarTransicion(OPCIONES);
                if (i == 2) iniciarTransicion(ENCICLOPEDIA);
            }
        }
    }
    else if (estadoActual == JUGAR) {
        for (int i = 0; i < (int)opcionesJugar.size(); i++) {
            float oy = 390 - i * 100;
            if (mx > 150 && mx<700 && my>oy - 32 && my < oy + 32) {
                opcionJugarSel = i;
                iniciarTransicion(i == 0 ? JUGAR_PVP : JUGAR_IA);
            }
        }
    }
    else if (estadoActual == OPCIONES) {
        // Clic en botón MUSICA
        if (mx > 115 && mx < 685 && my>448 && my < 484) {
            iniciarTransicion(MUSICA);
            return;
        }
        // Clic en sliders
        for (int i = 0; i < (int)controlesOpciones.size(); i++) {
            float y = 435 - i * 46;
            if (mx > 320 && mx<620 && my>y - 4 && my < y + 14) {
                opcionOpcionesSel = i;
                OpcionControl& op = controlesOpciones[i];
                float t2 = (float)(mx - 320) / 300.0f;
                op.valor = (int)(t2 * (op.max - op.min) + op.min);
                op.valor = std::max(op.min, std::min(op.max, op.valor));
            }
        }
    }
    else if (estadoActual == MUSICA) {
        // Clic en lista de canciones
        float listX = 370, listY = 475;
        for (int i = 0; i < (int)canciones.size(); i++) {
            float fy = listY - 30 - i * 30;
            if (mx > listX && mx<listX + 365 && my>fy - 8 && my < fy + 14) {
                cancionActual = i; cancionHover = i;
                progresoCancion = 0; reproduciendo = true;
            }
        }
        // Clic en barra de progreso
        if (mx > 60 && mx < 350 && my>196 && my < 204) {
            progresoCancion = (float)(mx - 60) / 290.0f;
            progresoCancion = std::max(0.0f, std::min(1.0f, progresoCancion));
        }
        // Clic en Play/Pause (centro)
        float ctrlCX = 200, ctrlY = 165;
        float dx2 = mx - ctrlCX, dy2 = my - ctrlY;
        if (dx2 * dx2 + dy2 * dy2 < 22 * 22) reproduciendo = !reproduciendo;
        // Anterior
        dx2 = mx - (ctrlCX - 42 * 2); dy2 = my - ctrlY;
        if (dx2 * dx2 + dy2 * dy2 < 16 * 16) {
            cancionActual--; if (cancionActual < 0) cancionActual = (int)canciones.size() - 1;
            cancionHover = cancionActual; progresoCancion = 0;
        }
        // Siguiente
        dx2 = mx - (ctrlCX + 42 * 2); dy2 = my - ctrlY;
        if (dx2 * dx2 + dy2 * dy2 < 16 * 16) {
            cancionActual++; if (cancionActual >= (int)canciones.size()) cancionActual = 0;
            cancionHover = cancionActual; progresoCancion = 0;
        }
        // Repetir
        dx2 = mx - (ctrlCX - 42 * 3 + 8); dy2 = my - ctrlY;
        if (dx2 * dx2 + dy2 * dy2 < 12 * 12) repetir = !repetir;
        // Aleatorio
        dx2 = mx - (ctrlCX + 42 * 3 - 8); dy2 = my - ctrlY;
        if (dx2 * dx2 + dy2 * dy2 < 12 * 12) aleatorio = !aleatorio;
        // Barra de volumen
        if (mx > 88 && mx < 268 && my>122 && my < 140) {
            volumenMusica = (int)((float)(mx - 88) / 180 * 10);
            volumenMusica = std::max(0, std::min(10, volumenMusica));
            controlesOpciones[0].valor = volumenMusica;
        }
    }
    else if (estadoActual == ENCICLOPEDIA) {
        if (mx < 300) { paginaLibro--; if (paginaLibro < 0) paginaLibro = TOTAL_PAGINAS - 1; }
        if (mx > 500) { paginaLibro++; if (paginaLibro >= TOTAL_PAGINAS) paginaLibro = 0; }
    }
}

// ============================================================
// IDLE
// ============================================================
void idle() {
    tiempo += 0.3f;

    // Offsets de entrada
    if (estadoActual == MENU && menuOffset < 0) menuOffset += 2.0f;
    if (estadoActual == JUGAR && jugarOffset < 0) jugarOffset += 2.0f;
    if (estadoActual == ENCICLOPEDIA && encicloOffset < 0) encicloOffset += 3.0f;
    if (estadoActual == OPCIONES && opcionesOffset < 0) opcionesOffset += 3.0f;
    if (estadoActual == MUSICA && musicaOffset < 0) musicaOffset += 3.0f;

    // Partículas fondo (estrellas)
    for (auto& p : particulas) {
        p.angulo += p.velocidadAngular;
        if (p.angulo > 2 * M_PI) p.angulo -= 2 * M_PI;
    }
    // Fuego
    for (auto& p : particulasFuego) {
        p.vida += p.vy;
        if (p.vida > p.vidaMax) {
            p.x = 50 + rand() % 700; p.y = rand() % 60;
            p.vida = 0; p.vidaMax = 80 + rand() % 60; p.size = 1.5f + (rand() % 4);
        }
        p.y += p.vy * 0.8f;
        p.x += sin(tiempo * 0.01f + p.x * 0.05f) * 0.3f;
    }
    // Teatro
    for (auto& p : particulasTeatro) {
        p.vida += 0.4f; p.x += p.vx; p.y += p.vy;
        if (p.vida > p.vidaMax) {
            p.x = 90 + rand() % 620; p.y = 130 + rand() % 390;
            p.vx = ((rand() % 100) - 50) * 0.005f;
            p.vy = ((rand() % 100) - 50) * 0.005f - 0.05f;
            p.vida = 0; p.vidaMax = 150 + rand() % 150;
        }
    }
    // Polvo libro
    for (auto& p : particulasPolvo) {
        p.vida += 0.3f;
        p.x += p.vx + sin(tiempo * 0.005f + p.y * 0.01f) * 0.1f;
        p.y += p.vy;
        if (p.vida > p.vidaMax) {
            p.x = 85 + rand() % 630; p.y = 80 + rand() % 460;
            p.vx = ((rand() % 100) - 50) * 0.003f;
            p.vy = 0.03f + rand() % 100 * 0.001f;
            p.vida = 0; p.vidaMax = 120 + rand() % 200;
        }
    }
    // Notas musicales
    for (auto& p : particulasNotas) {
        p.vida += 0.3f; p.x += p.vx; p.y += p.vy;
        p.rot += 0.5f;
        if (p.vida > p.vidaMax) {
            p.x = 160 + rand() % 480; p.y = 110 + rand() % 420;
            p.vx = ((rand() % 100) - 50) * 0.007f;
            p.vy = 0.1f + (rand() % 100) * 0.008f;
            p.vida = 0; p.vidaMax = 100 + rand() % 200;
            p.tipo = rand() % 3;
            p.size = 3.0f + (rand() % 4);
        }
    }

    // Visualizador — interpolación suave hacia targets aleatorios
    for (int i = 0; i < NUM_BARRAS; i++) {
        if (reproduciendo) {
            // Target animado con frecuencias simuladas
            float fase = tiempo * 0.02f * (0.5f + i * 0.08f);
            float base = 8 + sin(fase) * 5 + sin(fase * 0.7f + i) * 4 + sin(fase * 1.3f) * 3;
            base *= (float)volumenMusica / 10.0f;
            if (base < 1) base = 1;
            barraTarget[i] = base;
        }
        else {
            barraTarget[i] = 2.0f; // silencio
        }
        barrasViz[i] += (barraTarget[i] - barrasViz[i]) * 0.12f;
    }

    // Progreso de la canción (simulado)
    if (reproduciendo && estadoActual == MUSICA) {
        progresoCancion += velocidadProg;
        if (progresoCancion >= 1.0f) {
            progresoCancion = 0.0f;
            if (aleatorio) {
                cancionActual = rand() % (int)canciones.size();
            }
            else if (repetir) {
                // misma canción
            }
            else {
                cancionActual = (cancionActual + 1) % (int)canciones.size();
            }
            cancionHover = cancionActual;
        }
    }

    // Slash
    if (slashActivo) {
        slashX += 12;
        if (slashX > 900) {
            slashActivo = false; slashX = -1000.0f;
            estadoActual = siguienteEstado;
            if (estadoActual == MENU)         menuOffset = -400.0f;
            if (estadoActual == JUGAR)        jugarOffset = -600.0f;
            if (estadoActual == ENCICLOPEDIA) encicloOffset = -800.0f;
            if (estadoActual == OPCIONES)     opcionesOffset = -800.0f;
            if (estadoActual == MUSICA)       musicaOffset = -800.0f;
        }
    }

    glutPostRedisplay();
}

// ============================================================
// INIT
// ============================================================
void init() {
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluOrtho2D(0, 800, 0, 600);
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; i < 80; i++) {
        Particula p;
        p.x = rand() % 800; p.y = rand() % 600;
        p.radio = 5 + rand() % 15;
        p.angulo = (rand() % 360) * M_PI / 180.0f;
        p.velocidadAngular = 0.001f + (rand() % 5) / 5000.0f;
        particulas.push_back(p);
    }
    for (int i = 0; i < 40; i++) {
        ParticulaFuego p;
        p.x = 50 + rand() % 700; p.y = rand() % 100;
        p.vy = 0.3f + (rand() % 10) / 10.0f;
        p.vida = rand() % 100; p.vidaMax = 80 + rand() % 60;
        p.size = 1.5f + (rand() % 4);
        particulasFuego.push_back(p);
    }
    for (int i = 0; i < 60; i++) {
        ParticulaTeatro p;
        p.x = 90 + rand() % 620; p.y = 130 + rand() % 390;
        p.vx = ((rand() % 100) - 50) * 0.005f;
        p.vy = ((rand() % 100) - 50) * 0.005f - 0.05f;
        p.vida = rand() % 200; p.vidaMax = 150 + rand() % 150;
        p.size = 1.5f;
        p.r = 0.5f + (rand() % 5) * 0.08f;
        p.g = 0.1f + (rand() % 3) * 0.05f;
        p.b = 0.05f;
        particulasTeatro.push_back(p);
    }
    for (int i = 0; i < 50; i++) {
        ParticulaPolvo p;
        p.x = 85 + rand() % 630; p.y = 80 + rand() % 460;
        p.vx = ((rand() % 100) - 50) * 0.003f;
        p.vy = 0.03f + rand() % 100 * 0.001f;
        p.vida = rand() % 200; p.vidaMax = 120 + rand() % 200;
        p.size = 1;
        particulasPolvo.push_back(p);
    }
    for (int i = 0; i < 35; i++) {
        ParticulaNota p;
        p.x = 160 + rand() % 480; p.y = 110 + rand() % 420;
        p.vx = ((rand() % 100) - 50) * 0.007f;
        p.vy = 0.1f + (rand() % 100) * 0.008f;
        p.vida = rand() % 200; p.vidaMax = 100 + rand() % 200;
        p.size = 3.0f + (rand() % 4);
        p.rot = rand() % 360;
        p.tipo = rand() % 3;
        particulasNotas.push_back(p);
    }

    inicializarVisualizador();
}

// ============================================================
// MAIN
// ============================================================
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ALPHA);
    glutInitWindowSize(800, 600);
    glutCreateWindow("ARCHON");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(teclado);
    glutSpecialFunc(tecladoEspecial);
    glutMouseFunc(mouse);
    glutIdleFunc(idle);
    glutMainLoop();
    return 0;
}