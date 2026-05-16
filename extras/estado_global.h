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
    char nombre[3];

} puntajereg_t;
#define ESTADO_GLOBAL_MAXPUNTS 5

void global_registar_puntaje(pantalla_id puntaje, char nombre[3]);
puntajereg_t* global_obtener_puntajes();

#endif // ESTADO_GLOBAL_H_INCLUDED
