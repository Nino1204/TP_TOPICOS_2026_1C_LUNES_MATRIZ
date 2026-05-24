/*
INTEGRANTES:

    Apellido: Espada Rodriguez, Federico
    DNI: 47165728
    Usuario: Feresparo
    Entrega: Sí

    Apellido: Montanaro, Nino
    DNI: 45870269
    Usuario: Nino1204
    Entrega: Sí

    Rios, Tobias Joel - 38913716
    Apellido: Rios, Tobias Joel
    DNI: 38913716
    Usuario: TobiasUnlam
    Entrega: No

*/

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

enum {
    MAINARGS_PATH,
    MAINARGS_RES,
    MAINARGS_ESC
};
//main.c(char[]) restipo(char[3] = CGA/VGA) escala(u8)

void main_leer_args(unsigned char *escala, int argc, char **argv);

int main(int argc, char **argv)
{

    if (gbt_iniciar())
        printf("ERROR AL INICIAL libGBT:\n%s\n", gbt_obtener_log());

    global_iniciar(PANTALLA_MENUPRINC);

    unsigned char v_escala;
    main_leer_args(&v_escala, argc, argv);

    //VENTANA_ALTO Y VENTANA_ANCHO definidos en estado_global.h
    printf("iniciando tetris (%ux%ux%u)...\n", VENTANA_ANCHO,VENTANA_ALTO, v_escala);

    if (gbt_crear_ventana("TRABAJO MATRIZ", VENTANA_ANCHO,VENTANA_ALTO, v_escala))
        printf("ERROR AL CREAR VENTANA:\n%s\n", gbt_obtener_log());

    int corriendo = 1;
    pantalla_id pantalla;
    pantalla_id pantalla_sig;

    utils_set_pixel_mascara(0xFF);

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

void main_leer_args(unsigned char *escala, int argc, char **argv)
{

    char res[4];
    strcpy_s(res, sizeof(char)*4, argv[MAINARGS_RES]);

    unsigned ancho,alto;

    if (!strcmp(res, "CGA"))
    {
        global_obtener_config_ptr()->res = RESTIPO_CGA;
        ancho = RESCGA_ANCHO;
        alto = RESCGA_ALTO;
    }
    else if (!strcmp(res, "VGA"))
    {
        global_obtener_config_ptr()->res = RESTIPO_VGA;
        ancho = RESVGA_ANCHO;
        alto = RESVGA_ALTO;
    }
    else
    {
        ancho = VENTANA_ANCHO;
        alto = VENTANA_ALTO;
    }

    char *e_fin;
    unsigned long esc = (unsigned long) strtoul(argv[MAINARGS_ESC], &e_fin, 10);
    if (*e_fin != '\0' || esc > 10) //si hubo un error con el arg, o es mas grande de 10
        esc = global_ventana_escala();

    *escala = (unsigned char)esc;

    global_describir_ventana(ancho, alto, esc);

    //el default es CGA - 2.0, se usan si hay mal argumentos

}
