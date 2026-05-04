#include "formas.h"

unsigned char forma_desc_l[4][4] = {
    { 0,2,0,0 },
    { 0,2,0,0 },
    { 0,2,0,0 },
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

forma_t forma_crear(int px, int py, unsigned char forma_id)
{

    forma_t forma;

    forma.px = px;
    forma.py = py;

    unsigned char (*forma_desc)[4];
    switch (forma_id)
    {
    case FORMA_ID_L:
        forma_desc = forma_desc_l;
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
    unsigned mapa_px;
    unsigned mapa_py;

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
    unsigned mapa_px;
    unsigned mapa_py;

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

    return 1;

}
int forma_puede_deslizar(forma_t forma, mapa_t mapa, int dir)
{

    forma.px += dir;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            int b = forma.desc[y][x];
            int mx = forma.px+x;
            int my = forma.py+y;
            if (b == 0) continue;
            if (mx < 0 || mx >= mapa.ancho) return 0;
            if (!mapa_posicion_es_vacia(mapa, mx,my)) return 0;
        }
    }

    return 1;

}
int forma_puede_rotar(forma_t forma, mapa_t mapa)
{

    //rotamos una forma duplicada y revisamos si hay algun problema
    forma_t forma_aux = forma;

    forma_rotar(&forma_aux);

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            int b = forma_aux.desc[y][x];
            if (b == 0) continue;
            int mx = forma.px+x;
            int my = forma.py+y;
            if (mx < 0 || mx >= mapa.ancho) return 0;
            if (my < 0 || my >= mapa.alto) return 0;
            if (!mapa_posicion_es_vacia(mapa, mx,my)) return 0;
        }
    }

    return 1;

}
void forma_rotar(forma_t *forma)
{

    //copiar matriz desc a var aux
    int maux[4][4];
    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
            maux[y][x] = forma->desc[y][x];
    }

    for (int x = 0; x < 4; x++)
    {
        for (int y = 3; y >= 0; y--)
        {
            int px = 3-y;
            int py = x;
            forma->desc[py][px] = maux[y][x];
        }

    }

}
