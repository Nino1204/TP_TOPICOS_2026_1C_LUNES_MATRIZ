#include "mapa.h"
#include <stdlib.h>
#include <memory.h>
#include "utils.h"
#include "imagenes.h"

mapa_t mapa_crear(unsigned _ancho, unsigned _alto)
{

    mapa_t m;

    m.ancho = _ancho;
    m.alto = _alto;
    m.tablero = NULL;

    unsigned mapa_tam = _ancho * _alto;

    //si por alguna razon se llama a esta funcion con
    //_ancho == 0 o _alto == 0, ni intentamos crear el tablero
    if (mapa_tam > 0)
    {
        m.tablero = malloc(sizeof(unsigned char) * m.ancho*m.alto);

        //si el malloc da NULL, se deja el resto de la estructura
        //m a 0, dejandola como invalida
        if (m.tablero == NULL)
        {
            m.ancho = 0;
            m.alto = 0;
        }
        else
        {
            //poner todo el tablero como vacio
            memset(m.tablero, 0, sizeof(unsigned char) * mapa_tam);
        }
    }

    return m;

}
int mapa_es_valido(mapa_t mapa)
{
    return mapa.ancho > 0 && mapa.alto > 0 && mapa.tablero != NULL;
}
int mapa_posicion_es_vacia(mapa_t mapa, unsigned posx,unsigned posy)
{
    //devuelve 0 si no es vacio, 1 si es vacio y 2 si no existe la pos
    if (posx >= mapa.ancho && posy >= mapa.alto)
        return 2;
    return mapa.tablero[posy*mapa.ancho+posx] == 0;
}
void mapa_poner_bloque(mapa_t mapa, unsigned posx,unsigned posy, unsigned char bloque_id)
{

    //si la posicion esta fuera del tablero
    if (posx >= mapa.ancho || posy >= mapa.alto)
        return;

    mapa.tablero[posy*mapa.ancho+posx] = bloque_id;

}
unsigned char mapa_obtener_bloque(mapa_t mapa, unsigned posx,unsigned posy)
{

    //si la posicion esta fuera del tablero devolver un numero invalido
    if (posx >= mapa.ancho || posy >= mapa.alto)
        return MAPA_POS_VACIA;

    //NOTA:
    //cuando una posicion en el tablero esta vacio se le pone 0.
    //por lo tanto si una posicion no esta vacia, y se quiere
    //saber el id del bloque en la pos se tiene que hacer tablero[index]-1

    return mapa.tablero[posy*mapa.ancho+posx];

}
void mapa_dibujar(mapa_t mapa, int ox,int oy)
{

    //convertir el puntero en una matriz, para poder
    //recorrerlo mas facil
    unsigned char (*tablero)[mapa.ancho] = mapa.tablero;

    unsigned char bloque_id;

    //paso por cada bloque y si no esta vacio lo dibujo
    for (int y = 0; y < mapa.alto; y++)
    {
        for (int x = 0; x < mapa.ancho; x++)
        {
            bloque_id = tablero[y][x];

            if (bloque_id > 0)
            {
                bloque_id--;
                utils_dibujar_imagen(
                    ox+x*MAPA_BLOQUE_TAM,
                    oy+y*MAPA_BLOQUE_TAM,
                    MAPA_BLOQUE_TAM,MAPA_BLOQUE_TAM,
                    Imagenes_ObtenerPixeles(IMAGENES_ID_BLOQUES, bloque_id)
                );
            }
        }
    }

}
int mapa_linea_llena(mapa_t mapa, unsigned linea)
{
    unsigned char (*tablero)[mapa.ancho] = mapa.tablero;
    for (int x = 0; x < mapa.ancho; x++)
    {
        if (tablero[linea][x] == 0)
            return 0;
    }
    return 1;
}
void mapa_revisar_lineas(mapa_t mapa)
{

    unsigned char (*tablero)[mapa.ancho] = mapa.tablero;

    for (int y = 0; y < mapa.alto; y++)
    {
        if (!mapa_linea_llena(mapa, y))
            continue;
        for (int j = y; j > 0; j--)
        {
            for (int x = 0; x < mapa.ancho; x++)
                tablero[j][x] = tablero[j-1][x];
        }
    }

}
void mapa_destruir(mapa_t mapa)
{

    //no hacer nada si el mapa no es valido
    if (mapa_es_valido(mapa))
        free(mapa.tablero);

}
