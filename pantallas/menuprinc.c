#include "menuprinc.h"

#include "GBT/gbt.h"
#include "../extras/utils.h"
#include "../extras/estado_global.h"

#include "../entidades/mapa.h"
#include "../entidades/formas.h"

#include <string.h>

enum {
    MENUPRINC_JUGAR,
    MENUPRINC_CONFIG,
    MENUPRINC_PUNTAJES,
    MENUPRINC_SALIR,
    MENUPRINC_OPCANT //cantidad de botones
};

struct {

    menu_boton_t botones[MENUPRINC_OPCANT];
    mapa_t mapa;
    float my;
    unsigned char b_actual;

} menuprinc_estado;

void menuprinc_describir_boton(unsigned char b_id, char nombre[MENU_BOTON_TEXTOMAX]);

void menuprinc_iniciar()
{

    menuprinc_estado.b_actual = MENUPRINC_JUGAR;

    menuprinc_describir_boton(MENUPRINC_JUGAR, "JUGAR");
    menuprinc_describir_boton(MENUPRINC_CONFIG, "CONFIGURACION");
    menuprinc_describir_boton(MENUPRINC_PUNTAJES, "PUNTAJES");
    menuprinc_describir_boton(MENUPRINC_SALIR, "SALIR");

    menuprinc_estado.mapa = mapa_crear(22,8);

    unsigned char formadesc_t[4][4] = { {2,2,2,0},{0,2,0,0},{0,2,0,0},{0,2,0,0} };
    unsigned char formadesc_e[4][4] = { {3,3,3,0},{3,3,0,0},{3,0,0,0},{3,3,3,0} };
    unsigned char formadesc_r[4][4] = { {6,6,6,0},{6,0,0,0},{6,0,0,0},{6,0,0,0} };
    unsigned char formadesc_i[4][4] = { {5,0,0,0},{0,0,0,0},{5,0,0,0},{5,0,0,0} };
    unsigned char formadesc_s[4][4] = { {0,4,4,4},{0,4,0,0},{0,0,4,0},{4,4,4,0} };

    forma_t f;
    f.px = 0; f.py = 0;
    memcpy(f.desc, formadesc_t, sizeof(unsigned char) * 4*4);
    forma_poner_en_mapa(f, menuprinc_estado.mapa);

    f.px = 8; f.py = 0;
    //memcpy(f.desc, formadesc_t, sizeof(unsigned char) * 4*4);
    forma_poner_en_mapa(f, menuprinc_estado.mapa);

    f.px = 4; f.py = 0;
    memcpy(f.desc, formadesc_e, sizeof(unsigned char) * 4*4);
    forma_poner_en_mapa(f, menuprinc_estado.mapa);

    f.px = 12; f.py = 0;
    memcpy(f.desc, formadesc_r, sizeof(unsigned char) * 4*4);
    forma_poner_en_mapa(f, menuprinc_estado.mapa);

    f.px = 16; f.py = 0;
    memcpy(f.desc, formadesc_i, sizeof(unsigned char) * 4*4);
    forma_poner_en_mapa(f, menuprinc_estado.mapa);

    f.px = 18; f.py = 0;
    memcpy(f.desc, formadesc_s, sizeof(unsigned char) * 4*4);
    forma_poner_en_mapa(f, menuprinc_estado.mapa);

}
void menuprinc_actualizar()
{

    eGBT_Tecla tecla = gbt_obtener_tecla_presionada();

    switch (tecla)
    {
    case GBTK_ARRIBA:
        menuprinc_estado.b_actual = (menuprinc_estado.b_actual == 0) ?
            MENUPRINC_OPCANT-1 :
            menuprinc_estado.b_actual-1;
        break;
    case GBTK_ABAJO:
        menuprinc_estado.b_actual = (menuprinc_estado.b_actual == MENUPRINC_OPCANT-1) ?
            0 : menuprinc_estado.b_actual+1;
        break;
    case GBTK_ENTER:
        global_siguiente_pantalla(PANTALLA_JUEGO);
        break;
    }

    menu_boton_t *boton;

    for (int i = 0; i < MENUPRINC_OPCANT; i++)
    {

        boton = menuprinc_estado.botones + i;

        float px = boton->x;
        px += ((float)boton->objx - px) * 0.12f;
        boton->x = px;

    }

    menuprinc_estado.my += (16.0f - menuprinc_estado.my) * 0.12f;

}
void menuprinc_dibujar()
{

    int mx = VENTANA_ANCHO/2 - 88;

    mapa_dibujar_sombra(
        menuprinc_estado.mapa,
        mx+1,(int)menuprinc_estado.my+1
    );
    mapa_dibujar(
        menuprinc_estado.mapa,
        mx,(int)menuprinc_estado.my
    );

    menu_boton_t *boton;

    for (int i = 0; i < MENUPRINC_OPCANT; i++)
    {

        boton = menuprinc_estado.botones + i;

        utils_dibujar_texto(boton->x,boton->y+1, boton->nombre, 9);
        utils_dibujar_texto(boton->x,boton->y, boton->nombre, 15);

        if (i == menuprinc_estado.b_actual)
        {
            utils_dibujar_cuadradolineas(
                boton->x-2, boton->y-2,
                boton->w+3, 19,
                15
            );
        }

    }

}
void menuprinc_cerrar()
{

}

void menuprinc_describir_boton(unsigned char b_id, char nombre[MENU_BOTON_TEXTOMAX])
{

    menu_boton_t *boton = menuprinc_estado.botones + b_id;

    float px, py;

    boton->w = utils_ancho_de_texto(nombre);

    px = VENTANA_ANCHO*0.5f - boton->w*0.5f;
    py = VENTANA_ALTO*0.5f + b_id*20.0f;

    boton->x = 0;
    boton->objx = (unsigned short)px;
    boton->y = (unsigned short)py;

    strcpy_s(
        boton->nombre,
        sizeof(char) * MENU_BOTON_TEXTOMAX,
        nombre
    );

}
