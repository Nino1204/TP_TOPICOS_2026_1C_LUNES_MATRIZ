#include "estado_global.h"

#include <stdio.h>

struct {

    puntajereg_t puntaje_actual;
    pantalla_id p_actual, p_siguiente;

    global_config_t config;

    unsigned char res_actual;
    int vancho, valto;

    unsigned char salida_en_fila;
    unsigned char usa_savefile; //si el juego va a ser continuado o no

} global_estado;

void global_leer_config();

void global_iniciar(pantalla_id p_inicial)
{

    global_estado.salida_en_fila = 0;

    global_estado.p_actual = PANTALLA_NADA;
    global_estado.p_siguiente = p_inicial;

    global_estado.puntaje_actual.puntaje = 0;
    global_estado.puntaje_actual.nombre[0] = '?';
    global_estado.puntaje_actual.nombre[1] = '?';
    global_estado.puntaje_actual.nombre[2] = '?';

    global_estado.usa_savefile = 0;

    global_leer_config();

    if (global_estado.config.res == RESTIPO_CGA)
    {
        global_estado.vancho = RESCGA_ANCHO;
        global_estado.valto = RESCGA_ALTO;
    }
    else
    {
        global_estado.vancho = RESVGA_ANCHO;
        global_estado.valto = RESVGA_ALTO;
    }

    global_actualizar_paleta();

}

//dice la pantalla en la que se encuentra el programa
pantalla_id global_obtener_pantalla_actual()
{
    return global_estado.p_actual;
}

//dice la pantalla a la que tendria que ir despues
pantalla_id global_obtener_pantalla_siguiente()
{
    return global_estado.p_siguiente;
}

//guarda la p_siguiente como la siguiente pantalla
void global_siguiente_pantalla(pantalla_id p_siguiente)
{
    global_estado.p_siguiente = p_siguiente;
}
void global_cambiar_pantallas()
{
    global_estado.p_actual = global_estado.p_siguiente;
    global_estado.p_siguiente = PANTALLA_NADA;
}

global_config_t *global_obtener_config_ptr()
{

    return &global_estado.config;

}

int global_salida_es_pedida()
{

    return global_estado.salida_en_fila;

}
void global_pedir_salida()
{
    global_estado.salida_en_fila = 1;
}

puntajereg_t* global_obtener_puntaje_ptr()
{
    return &global_estado.puntaje_actual;
}

int global_ventana_ancho()
{
    return global_estado.vancho;
}
int global_ventana_alto()
{
    return global_estado.valto;
}
int global_ventana_escala()
{
    return (global_estado.config.res == RESTIPO_CGA) ? RESCGA_ESCALA : RESVGA_ESCALA;
}

void global_cambiar_resolucion()
{

    unsigned vancho, valto;
    unsigned char vescala;

    unsigned char res = global_estado.config.res;
    if (res == RESTIPO_VGA)
    {
        vancho = RESVGA_ANCHO; valto = RESVGA_ALTO;
        vescala = RESVGA_ESCALA;
    }
    else
    {
        vancho = RESCGA_ANCHO; valto = RESCGA_ALTO;
        vescala = RESCGA_ESCALA;
    }

    global_estado.vancho = vancho;
    global_estado.valto = valto;

    gbt_destruir_ventana();
    gbt_crear_ventana("TRABAJO MATRIZ", vancho,valto,vescala);

}
unsigned char global_obtener_res()
{
    return global_estado.res_actual;
}

void global_actualizar_paleta()
{
    unsigned char paleta_id = global_estado.config.paleta;
    if (global_estado.config.modo_juego == MODOJUEGO_DX) paleta_id += 3;
    utils_aplicar_paleta(paleta_id);
}

void global_leer_config()
{

    FILE *file = fopen("config.dat", "rb");

    if (file == NULL)
    {
        global_config_t *conf = &global_estado.config;
        conf->paleta = 0;
        conf->velocidad = 500; //0.5s
        conf->mw = 10;
        conf->mh = 20;
        conf->modo_juego = MODOJUEGO_CLASICO;
        conf->res = RESTIPO_CGA;
        conf->dificultad = DIF_NORMAL;
        return;
    }

    fread(&global_estado.config, sizeof(global_config_t), 1, file);

    fclose(file);

}
void global_guardar_config()
{

    FILE *file = fopen("config.dat", "wb");

    if (file == NULL)
        return;

    fwrite(&global_estado.config, sizeof(global_config_t), 1, file);

    fclose(file);

}

void global_usar_savefile(int si)
{
    global_estado.usa_savefile = si;
}
int global_usa_savefile()
{
    return global_estado.usa_savefile;
}

unsigned char global_dificultad()
{
    return global_estado.config.dificultad;
}
