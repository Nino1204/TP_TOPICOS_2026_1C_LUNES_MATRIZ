#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GBT/gbt.h"
#include "extras/imagenes.h"
#include "extras/utils.h"
#include "extras/estado_global.h"

#include "pantallas/juego.h"
#include "pantallas/menuprinc.h"
#include "pantallas/menuconf.h"
#include "pantallas/menupunt.h"

#define COLOR_FONDO 15

void pantalla_cambiada(pantalla_id p_anterior, pantalla_id p_nueva);

int main()
{

    gbt_iniciar();

    gbt_crear_ventana("TRABAJO MATRIZ", RESCGA_ANCHO,RESCGA_ALTO, RESCGA_ESCALA);

    global_iniciar(PANTALLA_MENUPRINC);

    int corriendo = 1;
    pantalla_id pantalla;
    pantalla_id pantalla_sig;

    utils_set_pixel_mascara(0xFF);
    utils_aplicar_paleta(0);

    while (corriendo)
    {

        gbt_procesar_entrada();

        eGBT_Tecla tecla = gbt_obtener_tecla_presionada();

        if (tecla == GBTK_ESCAPE)
            corriendo = 0;

        pantalla = global_obtener_pantalla_actual();

        switch (pantalla)
        {
        case PANTALLA_MENUPRINC:
            menuprinc_actualizar();
            break;
        case PANTALLA_JUEGO:
            juego_actualizar();
            break;
        case PANTALLA_MENUCONF:
            menuconf_actualizar();
            break;
        case PANTALLA_GAMEOVER:
            menupunt_actualizar();
            break;
        }

        gbt_borrar_backbuffer(COLOR_FONDO);

        switch (pantalla)
        {
        case PANTALLA_MENUPRINC:
            menuprinc_dibujar();
            break;
        case PANTALLA_JUEGO:
            juego_dibujar();
            break;
        case PANTALLA_MENUCONF:
            menuconf_dibujar();
            break;
        case PANTALLA_GAMEOVER:
            menupunt_dibujar();
            break;
        }

        pantalla_sig = global_obtener_pantalla_siguiente();

        if (pantalla_sig != PANTALLA_NADA)
        {
            global_cambiar_pantallas();
            pantalla_cambiada(pantalla, pantalla_sig);
        }

        gbt_volcar_backbuffer();
        gbt_esperar(16);

        corriendo = corriendo && !global_salida_es_pedida();

    }

    pantalla_cambiada(global_obtener_pantalla_actual(), PANTALLA_NADA);
    //juego_cerrar();
    gbt_cerrar();

    return 0;

}

void pantalla_cambiada(pantalla_id p_anterior, pantalla_id p_nueva)
{

    switch (p_anterior)
    {
    case PANTALLA_MENUPRINC:
        menuprinc_cerrar();
        break;
    case PANTALLA_JUEGO:
        juego_cerrar();
        break;
    case PANTALLA_MENUCONF:
        menuconf_cerrar();
        break;
    case PANTALLA_GAMEOVER:
        menupunt_cerrar();
        break;
    }

    switch (p_nueva)
    {
    case PANTALLA_MENUPRINC:
        menuprinc_iniciar();
        break;
    case PANTALLA_JUEGO:
        juego_iniciar();
        break;
    case PANTALLA_MENUCONF:
        menuconf_iniciar();
        break;
    case PANTALLA_GAMEOVER:
        menupunt_iniciar();
        break;
    }

}
