#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GBT/gbt.h"
#include "extras/imagenes.h"
#include "extras/utils.h"

#include "pantallas/juego.h"

#define COLOR_FONDO 79

int main()
{

    gbt_iniciar();

    gbt_crear_ventana("HOLA", VENTANA_ANCHO,VENTANA_ALTO, 4);

    int corriendo = 1;

    utils_set_pixel_mascara(COLOR_FONDO);

    utils_set_pixel_mascara(0);

    juego_iniciar();

    while (corriendo)
    {

        gbt_procesar_entrada();

        eGBT_Tecla tecla = gbt_obtener_tecla_presionada();

        if (tecla == GBTK_ESCAPE)
            corriendo = 0;

        juego_actualizar();

        gbt_borrar_backbuffer(COLOR_FONDO);

        juego_dibujar();

        gbt_volcar_backbuffer();
        gbt_esperar(16);

    }

    juego_cerrar();

    gbt_cerrar();

    return 0;

}

