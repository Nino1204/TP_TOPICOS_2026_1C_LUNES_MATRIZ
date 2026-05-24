#ifndef JUEGO_H_INCLUDED
#define JUEGO_H_INCLUDED

#include "../entidades/formas.h"

typedef struct {

    forma_t forma_actual;
    forma_t forma_siguiente;
    unsigned score;
    unsigned tiempo_caida;
    int piezas_puestas;
    unsigned char modo;

} juego_savefile_t;

void juego_iniciar();
void juego_actualizar();
void juego_dibujar();
void juego_cerrar();

#endif // JUEGO_H_INCLUDED
