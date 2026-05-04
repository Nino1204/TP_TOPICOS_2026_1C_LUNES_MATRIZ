#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GBT/gbt.h"
#include "imagenes.h"

#include "mapa.h"
#include "formas.h"

#define MAPA_ANCHO 24
#define MAPA_ALTO 18

#define VENTANA_ANCHO 320
#define VENTANA_ALTO 200

int main()
{

    gbt_iniciar();

    gbt_crear_ventana("HOLA", VENTANA_ANCHO,VENTANA_ALTO, 4);

    int corriendo = 1;

    tGBT_Temporizador *temp = gbt_temporizador_crear(0.4);

    mapa_t mapa = mapa_crear(MAPA_ANCHO, MAPA_ALTO);

    //posicion en la que se dibuja el mapa
    int mapa_origenx = VENTANA_ANCHO/2 - MAPA_ANCHO*MAPA_BLOQUE_TAM/2;
    int mapa_origeny = VENTANA_ALTO - MAPA_ALTO*MAPA_BLOQUE_TAM;

    //poner las paredes verticales
    for (int y = 0; y < MAPA_ALTO; y++)
    {
        mapa_poner_bloque(mapa, 0,y, 1);
        mapa_poner_bloque(mapa, MAPA_ANCHO-1,y, 1);
    }

    forma_t forma = forma_crear(MAPA_ANCHO/2,0, FORMA_ID_O);

    while (corriendo)
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
            if (forma_puede_rotar(forma, mapa))
                forma_rotar(&forma);
            break;
        case GBTK_DERECHA:
            if (forma_puede_deslizar(forma, mapa, 1))
                forma.px++;
            break;
        case GBTK_IZQUIERDA:
            if (forma_puede_deslizar(forma, mapa, -1))
                forma.px--;
            break;
        }

        if (gbt_temporizador_consumir(temp) || tecla == GBTK_ABAJO)
        {

            if (forma_puede_bajar(forma, mapa))
            {
                forma.py++;
            }
            else
            {
                forma_poner_en_mapa(forma, mapa);
                forma = forma_crear(MAPA_ANCHO/2,0, rand()%FORMA_ID_CANTIDAD);
                forma_limpiar_de_mapa(forma, mapa);
                mapa_revisar_lineas(mapa);
            }


        }

        forma_poner_en_mapa(forma, mapa);
        mapa_dibujar(mapa, mapa_origenx,mapa_origeny);
        forma_limpiar_de_mapa(forma, mapa);

        gbt_volcar_backbuffer();
        gbt_esperar(16);

    }

    gbt_temporizador_destruir(temp);

    gbt_cerrar();

    return 0;

}

