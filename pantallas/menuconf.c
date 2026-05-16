#include "menuconf.h"

#include "GBT/gbt.h"
#include "../extras/utils.h"
#include "../extras/estado_global.h"

#include "../entidades/mapa.h"
#include "../entidades/formas.h"

#include "menuprinc.h"

enum {
    CONFIG_VEL,
    CONFIG_PAL,
    CONFIG_RES,
    CONFIG_MAPW,
    CONFIG_MAPH,
    CONFIG_CANT
};

struct {

    menu_boton_t botones[CONFIG_CANT];
    char conf_formatos[CONFIG_CANT][32];
    unsigned char boton_actual;

} menuconf_estado;

#define CONFIG_FORMATO(c_id, str) strcpy_s(menuconf_estado.conf_formatos[c_id], 32, str)

void menuconf_iniciar()
{

    CONFIG_FORMATO(CONFIG_VEL, "< VELOCIDAD:%u >");
    CONFIG_FORMATO(CONFIG_PAL, "< PALETA DE COLORES:%u >");
    CONFIG_FORMATO(CONFIG_VEL, "< RESOLUCION:%ux%u >");
    CONFIG_FORMATO(CONFIG_MAPW, "< TABLERO COLUMNAS:%u >");
    CONFIG_FORMATO(CONFIG_MAPH, "< TABLERO FILAS:%u >");

}
void menuconf_actualizar()
{

}
void menuconf_dibujar()
{

}
void menuconf_cerrar()
{

}
