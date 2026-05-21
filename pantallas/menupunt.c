#include "menupunt.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../extras/estado_global.h"
#include "../extras/utils.h"
#include "../extras/imagenes.h"

#include "GBT/gbt.h"

#define MENUPUNT_PUNTAJES_TOTALES 10

enum {
    MENUPUNT_INPUT,
    MENUPUNT_MOSTRAR
};

struct {

    puntajereg_t *regs;
    int board_origenx, board_origeny;

    unsigned temp; //para flash de caracter

    char nombre_actual[4];
    unsigned char est_actual;

    char info_texto[32];
    unsigned char ultimo_score; //el puntaje que se añade

    int esconder_titulo;

} menupunt_estado;

void menupunt_leer_puntajes();
void menupunt_guardar_puntajes();

void menupunt_iniciar()
{

    puntajereg_t reg_actual = *(puntajereg_t*)global_obtener_puntaje_ptr();

    sprintf_s(menupunt_estado.info_texto, 32, "su puntaje fue:%06u", reg_actual.puntaje);

    menupunt_leer_puntajes();

    menupunt_estado.board_origenx = VENTANA_ANCHO/2 - 12*3;
    menupunt_estado.board_origeny = VENTANA_ALTO/2 - (MENUPUNT_PUNTAJES_TOTALES-1)*5;

    strcpy(menupunt_estado.nombre_actual, "???");
    menupunt_estado.temp = 0;

    menupunt_estado.ultimo_score = 0xFF;

    //arrancar mostrando puntajes si el jugador no logro nada de puntos
    //si no, pedir nombre

    if (reg_actual.puntaje == 0)
        menupunt_estado.est_actual = MENUPUNT_MOSTRAR;
    else
        menupunt_estado.est_actual = MENUPUNT_INPUT;

}
void menupunt_actualizar()
{

    menupunt_estado.temp++;
    if (menupunt_estado.temp > 30)
        menupunt_estado.temp = 0;

    if (menupunt_estado.est_actual == MENUPUNT_INPUT)
    {
        menupunt_input_actualizar();
        return;
    }

    eGBT_Tecla tecla = gbt_obtener_tecla_presionada();
    if (tecla == GBTK_ESPACIO)
    {
        global_siguiente_pantalla(PANTALLA_MENUPRINC);
    }

}
void menupunt_dibujar()
{

    int texto_x, texto_y;

    if (!menupunt_estado.esconder_titulo)
    {

        texto_x = VENTANA_ANCHO/2 - utils_ancho_de_texto("GAME OVER")/2;
        utils_dibujar_texto(texto_x, 9, "GAME OVER", 9);
        utils_dibujar_texto(texto_x, 8, "GAME OVER", 15);

        texto_x = VENTANA_ANCHO/2-utils_ancho_de_texto(menupunt_estado.info_texto)/2;
        utils_dibujar_texto(texto_x,27, menupunt_estado.info_texto, 9);
        utils_dibujar_texto(texto_x,26, menupunt_estado.info_texto, 15);

    }
    else
    {
        texto_x = VENTANA_ANCHO/2 - utils_ancho_de_texto("mejores puntajes")/2;
        utils_dibujar_texto(texto_x,9, "mejores puntajes", 9);
        utils_dibujar_texto(texto_x,8, "mejores puntajes", 15);
    }

    if (menupunt_estado.est_actual == MENUPUNT_INPUT)
    {
        menupunt_input_dibujar();
        return;
    }

    char texto[6+6+1];
    for (int i = 0; i < MENUPUNT_PUNTAJES_TOTALES; i++)
    {
        sprintf_s(texto, 13, "%3s : %06u", menupunt_estado.regs[i].nombre, menupunt_estado.regs[i].puntaje);
        utils_dibujar_texto(
            menupunt_estado.board_origenx,menupunt_estado.board_origeny+i*12+1,
            texto, 9
        );
        utils_dibujar_texto(
            menupunt_estado.board_origenx,menupunt_estado.board_origeny+i*12,
            texto, (i == menupunt_estado.ultimo_score) ? 12 : 15
        );
    }

    utils_dibujar_texto(2,VENTANA_ALTO-9, "espacio: volver", 9);
    utils_dibujar_texto(2,VENTANA_ALTO-10, "espacio: volver", 15);

}
void menupunt_cerrar()
{
    menupunt_guardar_puntajes();
    free(menupunt_estado.regs);
}

void menupunt_leer_puntajes()
{

    menupunt_estado.regs = malloc(sizeof(puntajereg_t) * MENUPUNT_PUNTAJES_TOTALES);

    for (int i = 0; i < MENUPUNT_PUNTAJES_TOTALES; i++)
    {
        menupunt_estado.regs[i].nombre[0] = '?';
        menupunt_estado.regs[i].nombre[1] = '?';
        menupunt_estado.regs[i].nombre[2] = '?';
        menupunt_estado.regs[i].nombre[3] = '\0';
        menupunt_estado.regs[i].puntaje = 0;
    }

    FILE *f = fopen("puntajes.dat", "rb");

    if (f == NULL)
    {
        return;
    }

    fread(menupunt_estado.regs, sizeof(puntajereg_t), MENUPUNT_PUNTAJES_TOTALES, f);

    fclose(f);

}

void menupunt_guardar_puntajes()
{

    FILE *f = fopen("puntajes.dat", "wb");

    fwrite(menupunt_estado.regs, sizeof(puntajereg_t), MENUPUNT_PUNTAJES_TOTALES, f);

    fclose(f);

}

void menupunt_input_actualizar()
{

    eGBT_Tecla tecla = gbt_obtener_tecla_presionada();

    if (tecla == GBTK_RETROCESO)
    {
        for (int i = 2; i >= 0; i--)
        {
            if (menupunt_estado.nombre_actual[i] != '?')
            {
                menupunt_estado.nombre_actual[i] = '?';
                break;
            }
        }
    }

    if (tecla == GBTK_ENTER)
    {

        puntajereg_t reg_act = *(puntajereg_t*)global_obtener_puntaje_ptr();
        puntajereg_t *regs = menupunt_estado.regs;
        for (int i = 0; i < MENUPUNT_PUNTAJES_TOTALES; i++)
        {
            if (reg_act.puntaje <= regs[i].puntaje)
                continue;
            for (int j = MENUPUNT_PUNTAJES_TOTALES-1; j > i; j--)
                regs[j] = regs[j-1];
            strcpy(regs[i].nombre, menupunt_estado.nombre_actual);
            regs[i].puntaje = reg_act.puntaje;
            menupunt_estado.ultimo_score = i;
            break;
        }
        menupunt_estado.est_actual = MENUPUNT_MOSTRAR;
        return;
    }

    if (tecla < 'a' || tecla > 'z') return;

    for (int i = 0; i < 3; i++)
    {
        if (menupunt_estado.nombre_actual[i] == '?')
        {
            menupunt_estado.nombre_actual[i] = tecla;
            break;
        }
    }

}
void menupunt_input_dibujar()
{

    puntajereg_t reg = *(puntajereg_t*)global_obtener_puntaje_ptr();

    int texto_x, texto_y;

    texto_x = VENTANA_ANCHO/2 - utils_ancho_de_texto("GAME OVER")/2;
    utils_dibujar_texto(texto_x, 9, "GAME OVER", 9);
    utils_dibujar_texto(texto_x, 8, "GAME OVER", 15);

    char texto[64];
    sprintf_s(texto, 32, "su puntaje fue:%06u", reg.puntaje);
    texto_x = VENTANA_ANCHO/2-utils_ancho_de_texto(texto)/2;
    utils_dibujar_texto(texto_x,27, texto, 9);
    utils_dibujar_texto(texto_x,26, texto, 15);

    texto_x = VENTANA_ANCHO/2 - utils_ancho_de_texto("ingrese su nombre:")/2;
    utils_dibujar_texto(texto_x,VENTANA_ALTO/2-7, "ingrese su nombre:", 9);
    utils_dibujar_texto(texto_x,VENTANA_ALTO/2-8, "ingrese su nombre:", 15);

    texto_x = VENTANA_ANCHO/2 - 3*5;
    texto_y = VENTANA_ALTO/2+8;

    int es_vacio = 0;
    int es_primer_vacio = 1;

    for (int i = 0; i < 3; i++)
    {

        char cr = menupunt_estado.nombre_actual[i];

        es_vacio = cr == '?';

        if (!es_vacio)
        {
            utils_dibujar_char(
                texto_x+i*10, texto_y+1, 16,
                Imagenes_ObtenerPixeles(IMAGENES_ID_FUENTE8x16, cr-'a'), 9
            );
            utils_dibujar_char(
                texto_x+i*10, texto_y, 16,
                Imagenes_ObtenerPixeles(IMAGENES_ID_FUENTE8x16, cr-'a'), 15
            );
            continue;
        }

        if (es_primer_vacio)
        {
            es_primer_vacio = 0;
            if (menupunt_estado.temp > 15) continue;
        }


        utils_dibujar_char(
            texto_x+i*10, texto_y+9, 8,
            Imagenes_ObtenerPixeles(IMAGENES_ID_FUENTE8x8, '?' - '0'), 9
        );
        utils_dibujar_char(
            texto_x+i*10, texto_y+8, 8,
            Imagenes_ObtenerPixeles(IMAGENES_ID_FUENTE8x8, '?' - '0'), 15
        );

    }

    utils_dibujar_texto(2,VENTANA_ALTO-9, "enter: aceptar", 9);
    utils_dibujar_texto(2,VENTANA_ALTO-10, "enter: aceptar", 15);

}

void menupunt_esconder_titulo(int si)
{
    menupunt_estado.esconder_titulo = si;
}
