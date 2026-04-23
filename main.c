#include <stdio.h>
#include <stdlib.h>

#include "GBT/gbt.h"

int main()
{

    gbt_iniciar();

    gbt_crear_ventana("TETRIS!", 256,150, 4);

    int corriendo = 1;

    while (corriendo)
    {
        gbt_procesar_entrada();
        eGBT_Tecla tecla = gbt_obtener_tecla_presionada();
        if (tecla == GBTK_ESCAPE)
            corriendo = 0;
        gbt_esperar(16);
    }

    gbt_cerrar();

    return 0;
}
