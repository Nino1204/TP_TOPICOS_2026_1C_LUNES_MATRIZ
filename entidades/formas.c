#include "formas.h"

#include "../extras/utils.h"
#include "../extras/imagenes.h"

unsigned char forma_desc_l[4][4] = {
    { 0,0,0,0 },
    { 0,2,0,0 },
    { 0,2,0,0 },
    { 0,2,2,0 }
};
unsigned char forma_desc_l2[4][4] = {
    { 0,0,0,0 },
    { 0,0,2,0 },
    { 0,0,2,0 },
    { 0,2,2,0 }
};
unsigned char forma_desc_I[4][4] = {
    { 0,3,0,0 },
    { 0,3,0,0 },
    { 0,3,0,0 },
    { 0,3,0,0 }
};
unsigned char forma_desc_S[4][4] = {
    { 0,0,0,0 },
    { 0,0,4,4 },
    { 0,4,4,0 },
    { 0,0,0,0 }
};
unsigned char forma_desc_S2[4][4] = {
    { 0,0,0,0 },
    { 0,4,4,0 },
    { 0,0,4,4 },
    { 0,0,0,0 }
};
unsigned char forma_desc_O[4][4] = {
    { 0,0,0,0 },
    { 0,5,5,0 },
    { 0,5,5,0 },
    { 0,0,0,0 }
};
unsigned char forma_desc_T[4][4] = {
    { 0,0,0,0 },
    { 0,0,6,0 },
    { 0,6,6,6 },
    { 0,0,0,0 }
};

unsigned char forma_desc_C[4][4] = {
    { 0,0,0,0 },
    { 0,5,5,0 },
    { 0,5,0,0 },
    { 0,5,5,0 }
};
unsigned char forma_desc_P[4][4] = {
    { 0,0,0,0 },
    { 0,5,5,0 },
    { 0,5,5,0 },
    { 0,5,0,0 }
};
unsigned char forma_desc_X[4][4] = {
    { 0,0,0,0 },
    { 0,2,0,0 },
    { 0,0,0,0 },
    { 0,0,0,0 }
};
unsigned char forma_desc_N[4][4] = {
    { 0,0,0,0 },
    { 0,0,3,0 },
    { 0,3,0,0 },
    { 0,0,0,0 }
};

forma_t forma_crear(int px, int py, unsigned char forma_id)
{

    forma_t forma;

    forma.px = px;
    forma.py = py;
    forma.f_id = forma_id;

    unsigned char (*forma_desc)[4] = {0};
    switch (forma_id)
    {
    case FORMA_ID_L:
        forma_desc = forma_desc_l;
        break;
    case FORMA_ID_L2:
        forma_desc = forma_desc_l2;
        break;
    case FORMA_ID_I:
        forma_desc = forma_desc_I;
        break;
    case FORMA_ID_S:
        forma_desc = forma_desc_S;
        break;
    case FORMA_ID_S2:
        forma_desc = forma_desc_S2;
        break;
    case FORMA_ID_O:
        forma_desc = forma_desc_O;
        break;
    case FORMA_ID_T:
        forma_desc = forma_desc_T;
        break;
    case FORMA_ID_C:
        forma_desc = forma_desc_C;
        break;
    case FORMA_ID_P:
        forma_desc = forma_desc_P;
        break;
    case FORMA_ID_X:
        forma_desc = forma_desc_X;
        break;
    case FORMA_ID_N:
        forma_desc = forma_desc_N;
        break;
    }

    for (int y = 0; y < FORMA_ALTO; y++)
    {
        for (int x = 0; x < FORMA_ANCHO; x++)
            forma.desc[y][x] = forma_desc[y][x];
    }

    return forma;

}
void forma_poner_en_mapa(forma_t forma, mapa_t mapa)
{

    unsigned char bloque_id;
    int mapa_px;
    int mapa_py;

    for (int y = 0; y < FORMA_ALTO; y++)
    {
        for (int x = 0; x < FORMA_ANCHO; x++)
        {
            bloque_id = forma.desc[y][x];
            if (bloque_id == 0) continue;
            mapa_px = forma.px+x;
            mapa_py = forma.py+y;
            mapa_poner_bloque(mapa, mapa_px,mapa_py, bloque_id);
        }
    }

}
void forma_limpiar_de_mapa(forma_t forma, mapa_t mapa)
{

    unsigned char bloque_id;
    int mapa_px;
    int mapa_py;

    for (int y = 0; y < FORMA_ALTO; y++)
    {
        for (int x = 0; x < FORMA_ANCHO; x++)
        {
            bloque_id = forma.desc[y][x];
            if (bloque_id == 0) continue;
            mapa_px = forma.px+x;
            mapa_py = forma.py+y;
            mapa_poner_bloque(mapa, mapa_px,mapa_py, 0);
        }
    }

}
int forma_puede_bajar(forma_t forma, mapa_t mapa)
{

    //simular bajar (no afecta la posicion real
    forma.py++;

    /*
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            int b = forma.desc[y][x];
            unsigned mx = forma.px+x;
            unsigned my = forma.py+y;
            if (b == 0) continue;

            //no puede bajar mas si llega al final
            if (my >= mapa.alto)
                return 0;

            //no puede bajar mas si choca con algo
            if (!mapa_posicion_es_vacia(mapa,mx,my))
                return 0;
        }
    }
*/

    return !forma_tiene_colision(forma, mapa);

}
int forma_puede_deslizar(forma_t forma, mapa_t mapa, int dir)
{

    forma.px += dir;

    return !forma_tiene_colision(forma, mapa);

}
int forma_puede_rotar(forma_t forma, mapa_t mapa, signed char dir)
{

    //rotamos una forma duplicada y revisamos si hay algun problema
    forma_t forma_aux = forma;

    forma_rotar(&forma_aux, dir);

    return !forma_tiene_colision(forma_aux, mapa);

}
void forma_rotar(forma_t *forma, signed char dir)
{

    //copiar matriz desc a var aux
    int maux[4][4];
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
            maux[y][x] = forma->desc[y][x];
    }

    if (dir == 1)
    {
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                int px = 3-y;
                int py = x;
                forma->desc[py][px] = maux[y][x];
            }

        }
    }
    else
    {
        for (int y = 0; y < 4; y++)
        {
            for (int x = 0; x < 4; x++)
            {
                int px = y;
                int py = 3-x;
                forma->desc[py][px] = maux[y][x];
            }

        }
    }

}
void forma_dibujar_vistaprevia(forma_t forma)
{

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            int bloque_id = forma.desc[y][x];
            if (bloque_id == 0) continue;
            utils_dibujar_imagen(
                forma.px+x*8,
                forma.py+y*8, 8,8,(unsigned char(*)[8])
                Imagenes_ObtenerPixeles(IMAGENES_ID_BLOQUES, bloque_id-1)
            );
        }
    }

}
int forma_tiene_colision(forma_t forma, mapa_t mapa)
{

    unsigned char b_id;
    int mx,my;

    for (int y = 0; y < FORMA_ALTO; y++)
    {
        for (int x = 0; x < FORMA_ANCHO; x++)
        {

            b_id = forma.desc[y][x];

            if (b_id == 0) continue;

            mx = forma.px+x;
            my = forma.py+y;

            //if (mx >= mapa.ancho) return 1;
            if (my >= mapa.alto) return 1;

            if (!mapa_posicion_es_vacia(mapa, mx,my))
                return 1;

        }
    }

    return 0;

}
