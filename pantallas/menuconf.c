#include "menuconf.h"

#include "GBT/gbt.h"
#include "../extras/utils.h"
#include "../extras/estado_global.h"

#include "../entidades/mapa.h"
#include "../entidades/formas.h"

//se necesita para el struct de menu_boton_t
#include "menuprinc.h"

enum {
    CONFIG_VEL,
    CONFIG_PAL,
    CONFIG_MAPW,
    CONFIG_MAPH,
    CONFIG_ATRAS, //no es una configuracion, es el boton de volver, pero que se yo
    CONFIG_CANT
};

struct {

    menu_boton_t botones[CONFIG_CANT];
    char desc[CONFIG_CANT][128];
    int origen_y; //desde donde se empiezan a dibujar los botones (porque estan centrados)
    float desc_px;
    unsigned char boton_actual;

} menuconf_estado;

void menuconf_confcambiada(unsigned char config_id, signed char dir); //la funcion que se llama cuando se cambia una conf (dir={-1,1})
void menuconf_describir_boton(unsigned char config_id); //setup de los botones
#define CONFIG_DESCRIPCION(c_id, str) strcpy_s(menuconf_estado.desc[c_id], 128, str)

//actualiza la posicion del texto de descripcion
#define CONFIG_NUEVADESC menuconf_estado.desc_px = VENTANA_ANCHO/2-utils_ancho_de_texto(menuconf_estado.desc[menuconf_estado.boton_actual])/2

void menuconf_iniciar()
{

    int config_alto = 4+CONFIG_CANT*20;
    menuconf_estado.origen_y = 8 + VENTANA_ALTO/2-config_alto/2;

    for (int i = 0; i < CONFIG_CANT; i++)
        menuconf_describir_boton(i);

    CONFIG_DESCRIPCION(CONFIG_VEL, "el tiempo de caida inicial de la pieza");
    CONFIG_DESCRIPCION(CONFIG_PAL, "la paleta de colores que se usa en el juego");
    CONFIG_DESCRIPCION(CONFIG_MAPH, "la cantidad de columnas que tiene el tablero");
    CONFIG_DESCRIPCION(CONFIG_MAPW, "la cantidad de filas que tiene el tablero");
    CONFIG_DESCRIPCION(CONFIG_ATRAS, "volver al menu principal");

    menuconf_estado.boton_actual = 0;
    CONFIG_NUEVADESC;

}
void menuconf_actualizar()
{

    eGBT_Tecla tecla = gbt_obtener_tecla_presionada();

    switch (tecla)
    {

    case GBTK_ARRIBA:
        menuconf_estado.boton_actual = (menuconf_estado.boton_actual == 0) ?
            CONFIG_CANT-1 :
            menuconf_estado.boton_actual-1
        ;
        CONFIG_NUEVADESC;
        break;
    case GBTK_ABAJO:
        menuconf_estado.boton_actual = (menuconf_estado.boton_actual+1)%CONFIG_CANT;
        CONFIG_NUEVADESC;
        break;
    case GBTK_ENTER:
        if (menuconf_estado.boton_actual == CONFIG_ATRAS)
            global_siguiente_pantalla(PANTALLA_MENUPRINC);
        break;
    case GBTK_IZQUIERDA:
        menuconf_confcambiada(menuconf_estado.boton_actual, -1);
        break;
    case GBTK_DERECHA:
        menuconf_confcambiada(menuconf_estado.boton_actual, 1);
        break;
    }

    menu_boton_t *boton;
    for (int i = 0; i < CONFIG_CANT; i++)
    {

        boton = menuconf_estado.botones + i;

        float px = boton->x;
        px += ((float)boton->objx - px) * 0.12f;
        boton->x = px;

    }

}
void menuconf_dibujar()
{

    utils_dibujar_texto(VENTANA_ANCHO/2-14*3,9, "configuracion:", 9);
    utils_dibujar_texto(VENTANA_ANCHO/2-14*3,8, "configuracion:", 15);

    for (int i = 0; i < CONFIG_CANT; i++)
    {

        menu_boton_t boton = menuconf_estado.botones[i];
        utils_dibujar_texto(boton.x,boton.y+1, boton.nombre, 9);
        utils_dibujar_texto(boton.x,boton.y, boton.nombre, 15);

        if (menuconf_estado.boton_actual == i)
        {
            utils_dibujar_cuadradolineas(
                boton.x-4,boton.y-4, boton.w+8,16,
                15
            );
        }

    }

    utils_dibujar_texto(
                        menuconf_estado.desc_px, VENTANA_ALTO-11,
                        menuconf_estado.desc[menuconf_estado.boton_actual],
                        9
                        );
    utils_dibujar_texto(
                        menuconf_estado.desc_px, VENTANA_ALTO-12,
                        menuconf_estado.desc[menuconf_estado.boton_actual],
                        15
                        );

}
void menuconf_cerrar()
{

}

void menuconf_describir_boton(unsigned char config_id)
{

    global_config_t configuracion = *(global_config_t*) global_obtener_config_ptr();
    menu_boton_t *boton = menuconf_estado.botones + config_id;

    switch (config_id)
    {
    case CONFIG_VEL:
        sprintf_s( boton->nombre, sizeof(char)*32, "< velocidad:%ums >", configuracion.velocidad );
        break;
    case CONFIG_PAL:
        sprintf_s( boton->nombre, sizeof(char)*32, "< paleta n%u >", configuracion.paleta+1 );
        break;
    case CONFIG_MAPW:
        sprintf_s( boton->nombre, sizeof(char)*32, "< columnas:%u >", configuracion.mw );
        break;
    case CONFIG_MAPH:
        sprintf_s( boton->nombre, sizeof(char)*32, "< filas:%u >", configuracion.mh );
        break;
    case CONFIG_ATRAS:
        strcpy(boton->nombre, "volver");
        break;
    }

    int texto_ancho = utils_ancho_de_texto(boton->nombre);
    boton->x = VENTANA_ANCHO;
    boton->objx = VENTANA_ANCHO/2-texto_ancho/2;
    boton->y = 4+menuconf_estado.origen_y + config_id*20;
    boton->w = texto_ancho;
    boton->h = 16;

}

void menuconf_confcambiada(unsigned char config_id, signed char dir)
{

    global_config_t *c_ptr = global_obtener_config_ptr();

    int nuevo; //nuevo valor - variante auxiliar

    switch (config_id)
    {

    case CONFIG_VEL:
        nuevo = (int)c_ptr->velocidad + (int)dir * 50;
        if (nuevo < 0) nuevo = 0;
        if (nuevo > 10000) nuevo = 10000;
        c_ptr->velocidad = nuevo;
        break;
    case CONFIG_PAL:
        nuevo = (int)c_ptr->paleta + (int)dir;
        if (nuevo < 0) nuevo += 3;
        nuevo = nuevo%3;
        c_ptr->paleta = nuevo;
        utils_aplicar_paleta(c_ptr->paleta);
        break;
    case CONFIG_MAPW:
        nuevo = (int)c_ptr->mw + (int)dir;
        if (nuevo < 5) nuevo = 5;
        if (nuevo > 30) nuevo = 30;
        c_ptr->mw = (unsigned)nuevo;
        break;
    case CONFIG_MAPH:
        nuevo = (int)c_ptr->mh + (int)dir;
        if (nuevo < 5) nuevo = 5;
        if (nuevo > 30) nuevo = 30;
        c_ptr->mh = (unsigned)nuevo;
        break;

    }

    menu_boton_t *boton = menuconf_estado.botones + config_id;
    float px_viejo = boton->x;

    menuconf_describir_boton(config_id); //actualiza el texto

    //despues de la funcion se resetea la funcion, esto lo evita
    //"+30*dir" hace la animacion boluda de mover el boton
    boton->x = px_viejo + 30 * dir;

}
