#ifndef ESTADO_GLOBAL_H_INCLUDED
#define ESTADO_GLOBAL_H_INCLUDED

typedef enum {
    PANTALLA_NADA,
    PANTALLA_MENUPRINC,
    PANTALLA_MENUCONF,
    PANTALLA_JUEGO,
    PANTALLA_GAMEOVER
} pantalla_id;

void global_iniciar(pantalla_id p_inicial);

//dice la pantalla en la que se encuentra el programa
pantalla_id global_obtener_pantalla_actual();

//dice la pantalla a la que tendria que ir despues
pantalla_id global_obtener_pantalla_siguiente();

//guarda "p_siguiente" como la siguiente pantalla
void global_siguiente_pantalla(pantalla_id p_siguiente);
void global_cambiar_pantallas(); //pasa a la siguiente pantalla

typedef struct {

    unsigned puntaje;
    char nombre[4];

} puntajereg_t;
#define ESTADO_GLOBAL_MAXPUNTS 5

enum {
    MODOJUEGO_CLASICO,
    MODOJUEGO_DX
};

enum {
    DIF_NORMAL,
    DIF_DIFICIL
};

typedef struct {

    unsigned velocidad;
    unsigned char paleta;
    unsigned char mw, mh; //cantidad de columnas y filas
    unsigned char modo_juego;
    unsigned char res;
    unsigned char dificultad;

} global_config_t;

void global_registar_puntaje(pantalla_id puntaje, char nombre[3]);
puntajereg_t* global_obtener_puntajes();

global_config_t *global_obtener_config_ptr(); //devuelve la direccion de la configuracion global

int global_salida_es_pedida(); //si se pidio cerrar el programa
void global_pedir_salida(); //pedir para cerrar el programa

puntajereg_t* global_obtener_puntaje_ptr();

enum {
    RESTIPO_CGA,
    RESTIPO_VGA
};

#define RESCGA_ANCHO 320
#define RESCGA_ALTO 200
#define RESCGA_ESCALA 3

#define RESVGA_ANCHO 640
#define RESVGA_ALTO 400
#define RESVGA_ESCALA 2

int global_ventana_ancho();
int global_ventana_alto();
int global_ventana_escala();

#define VENTANA_ANCHO global_ventana_ancho()
#define VENTANA_ALTO global_ventana_alto()

//cambia al otro tipo de resolucion
void global_cambiar_resolucion();
unsigned char global_obtener_res();

void global_actualizar_paleta();

void global_guardar_config();

//describe si usar o no el savefile al cargar el juego
void global_usar_savefile(int si);
int global_usa_savefile(); //devuelve si hay que usar el savefile o no

//devuelve la dificultad del juego, en funcion por conveniencia
unsigned char global_dificultad();

#endif // ESTADO_GLOBAL_H_INCLUDED
