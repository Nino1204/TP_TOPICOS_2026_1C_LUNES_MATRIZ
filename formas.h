#ifndef FORMAS_H_INCLUDED
#define FORMAS_H_INCLUDED

#include "mapa.h"

//las formas son figuras que caen del cielo
//en el tetris tienen un nombre pero me lo olvide

#define FORMA_ANCHO 4
#define FORMA_ALTO 4

typedef struct {
    int px,py;
    unsigned char f_id;
    unsigned char desc[FORMA_ALTO][FORMA_ANCHO];
} forma_t;

enum {
    FORMA_ID_L,
    FORMA_ID_I,
    FORMA_ID_S,
    FORMA_ID_S2,
    FORMA_ID_O,
    FORMA_ID_CANTIDAD //cantidad de tipos de formas
};

forma_t forma_crear(int px, int py, unsigned char forma_id);
void forma_poner_en_mapa(forma_t forma, mapa_t mapa);
void forma_limpiar_de_mapa(forma_t forma, mapa_t mapa);
int forma_puede_bajar(forma_t forma, mapa_t mapa);
int forma_puede_deslizar(forma_t forma, mapa_t mapa, int dir);
int forma_puede_rotar(forma_t forma, mapa_t mapa);
void forma_rotar(forma_t *forma);
void forma_dibujar_vistaprevia(forma_t forma);
void forma_dibujar_sombra(forma_t forma);
int forma_tiene_colision(forma_t forma, mapa_t mapa);

#endif // FORMAS_H_INCLUDED
