#include <stdio.h>
#include <stdlib.h>

#include "GBT/gbt.h"
#include "imagenes.h"

void dibujar_imagen(int px, int py, int w,int h, unsigned char *pixeles);

#define MAPA_ANCHO 16
#define MAPA_ALTO 18
#define MAPA_TAM MAPA_ANCHO*MAPA_ALTO
void mapa_dibujar(unsigned char mapa[][MAPA_ANCHO], int px,int py);
void mapa_revisar_lineas(unsigned char mapa[][MAPA_ANCHO]);

typedef struct {
    int px,py;
    unsigned char desc[4][4];
} forma_t;

void forma_poner_en_mapa(unsigned char mapa[][MAPA_ANCHO], forma_t forma);
void forma_limpiar_de_mapa(unsigned char mapa[][MAPA_ANCHO], forma_t forma);
int forma_puede_bajar(unsigned char mapa[][MAPA_ANCHO], forma_t forma);
int forma_puede_deslizar(unsigned char mapa[][MAPA_ANCHO], forma_t forma, int dir);
int forma_puede_rotar(unsigned char mapa[][MAPA_ANCHO], forma_t forma);
void forma_rotar(forma_t *forma);
forma_t forma_crear(int px, int py, unsigned char forma_id);

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
enum {
    FORMA_ID_L,
    FORMA_ID_I,
    FORMA_ID_S
};

int main()
{

    gbt_iniciar();

    gbt_crear_ventana("HOLA", 256,150, 4);

    int corriendo = 1;

    tGBT_Temporizador *temp = gbt_temporizador_crear(0.4);

    unsigned char mapa[MAPA_ALTO][MAPA_ANCHO];

    for (int y = 0; y < MAPA_ALTO; y++)
    {
        for (int x = 0; x < MAPA_ANCHO; x++)
        {
            mapa[y][x] = 0;
        }
    }
    for (int y = 0; y < MAPA_ALTO; y++)
    {
        mapa[y][0] = 1;
        mapa[y][MAPA_ANCHO-1] = 1;
    }

    int posx = 2;
    int posy = 0;

    forma_t forma = forma_crear(4,0, FORMA_ID_L);

    while (corriendo == 1)
    {

        gbt_borrar_backbuffer(1);
        gbt_procesar_entrada();

        eGBT_Tecla tecla = gbt_obtener_tecla_presionada();

        switch (tecla)
        {
        case GBTK_ESCAPE:
            corriendo = 0;
            break;
        case GBTK_ESPACIO:
            if (forma_puede_rotar(mapa, forma))
                forma_rotar(&forma);
            break;
        case GBTK_DERECHA:
            if (forma_puede_deslizar(mapa, forma, 1))
                forma.px++;
            break;
        case GBTK_IZQUIERDA:
            if (forma_puede_deslizar(mapa, forma, -1))
                forma.px--;
            break;
        }

        if (tecla == GBTK_ABAJO)
        {
            while (1)
            {
                if (forma_puede_bajar(mapa, forma) == 1)
                {
                    forma.py++;
                }
                else
                {
                    forma_poner_en_mapa(mapa, forma);
                    forma = forma_crear(4,0, rand()%3);
                    mapa_revisar_lineas(mapa);
                    break;
                }
            }
        }

        if (gbt_temporizador_consumir(temp))
        {

            if (forma_puede_bajar(mapa, forma) == 1)
            {
                forma.py++;
            }
            else
            {
                forma_poner_en_mapa(mapa, forma);
                forma = forma_crear(4,0, rand()%3);
                mapa_revisar_lineas(mapa);
            }

        }

        forma_poner_en_mapa(mapa, forma);
        mapa_dibujar(mapa, 128-64,6);
        forma_limpiar_de_mapa(mapa, forma);

        gbt_volcar_backbuffer();
        gbt_esperar(16);

    }

    gbt_temporizador_destruir(temp);

    gbt_cerrar();

    return 0;

}

void dibujar_imagen(int px, int py, int w,int h, unsigned char *pixeles)
{

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            unsigned char p = pixeles[y*w+x];
            if (p != 1)
                gbt_dibujar_pixel(px+x,py+y,p);
        }
    }

}

void mapa_dibujar(unsigned char mapa[][MAPA_ANCHO], int px,int py)
{

    for (int y = 0; y < MAPA_ALTO; y++)
    {
        for (int x = 0; x < MAPA_ANCHO; x++)
        {
            int b_id = mapa[y][x];
            if (b_id != 0)
            {
                dibujar_imagen(px+x*8,py+y*8, 8,8, Imagenes_ObtenerPixeles(0, b_id-1));
            }
        }
    }

}
int mapa_linea_llena(unsigned char mapa[][MAPA_ANCHO], int linea_id)
{
    for (int x = 0; x < MAPA_ANCHO; x++)
    {
        if (mapa[linea_id][x] == 0)
            return 0;
    }
    return 1;
}
void mapa_revisar_lineas(unsigned char mapa[][MAPA_ANCHO])
{

    for (int y = 0; y < MAPA_ALTO; y++)
    {

        if (!mapa_linea_llena(mapa, y))
            continue;

        for (int j = y; j > 0; j--)
        {
            for (int x = 0; x < MAPA_ANCHO; x++)
                mapa[j][x] = mapa[j-1][x];

        }

    }

}

void forma_poner_en_mapa(unsigned char mapa[][MAPA_ANCHO], forma_t forma)
{

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            int b = forma.desc[y][x];
            int mx = forma.px+x;
            int my = forma.py+y;
            if (b != 0)
            {
                mapa[my][mx] = b;
            }
        }
    }

}
void forma_limpiar_de_mapa(unsigned char mapa[][MAPA_ANCHO], forma_t forma)
{

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            int b = forma.desc[y][x];
            int mx = forma.px+x;
            int my = forma.py+y;
            if (b != 0)
                mapa[my][mx] = 0;
        }
    }
}
int forma_puede_bajar(unsigned char mapa[][MAPA_ANCHO], forma_t forma)
{

    //simular bajar
    forma.py++;

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
        {
            int b = forma.desc[y][x];
            int mx = forma.px+x;
            int my = forma.py+y;
            if (b == 0) continue;

            if (my >= MAPA_ALTO)
                return 0;

            if (mapa[my][mx] != 0)
                return 0;

        }
    }

    return 1;

}
int forma_puede_deslizar(unsigned char mapa[][MAPA_ANCHO], forma_t forma, int dir)
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
            if (mx < 0 || mx >= MAPA_ANCHO) return 0;
            if (mapa[my][mx] != 0) return 0;
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
        {
            maux[y][x] = forma->desc[y][x];
        }
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
int forma_puede_rotar(unsigned char mapa[][MAPA_ANCHO], forma_t forma)
{

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
            if (mx < 0 || mx >= MAPA_ANCHO) return 0;
            if (my < 0 || my >= MAPA_ALTO) return 0;
            if (mapa[my][mx] != 0) return 0;
        }
    }

    return 1;

}
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
    }

    for (int y = 0; y < 4; y++)
    {
        for (int x = 0; x < 4; x++)
            forma.desc[y][x] = forma_desc[y][x];
    }

    return forma;
}
