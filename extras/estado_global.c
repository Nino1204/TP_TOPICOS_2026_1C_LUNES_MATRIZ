#include "estado_global.h"

struct {

    puntajereg_t puntajes_registros[ESTADO_GLOBAL_MAXPUNTS];
    puntajereg_t puntaje_actual;
    pantalla_id p_actual, p_siguiente;

    global_config_t config;

    unsigned char salida_en_fila;

} global_estado;

void global_iniciar(pantalla_id p_inicial)
{

    global_estado.salida_en_fila = 0;

    global_estado.p_actual = PANTALLA_NADA;
    global_estado.p_siguiente = p_inicial;

    global_estado.puntaje_actual.puntaje = 0;
    global_estado.puntaje_actual.nombre[0] = '?';
    global_estado.puntaje_actual.nombre[1] = '?';
    global_estado.puntaje_actual.nombre[2] = '?';

    for (int i = 0; i < ESTADO_GLOBAL_MAXPUNTS; i++)
    {
        global_estado.puntajes_registros[i].puntaje = 0;
        //nombre = '---'
        global_estado.puntajes_registros[i].nombre[0] = '?';
        global_estado.puntajes_registros[i].nombre[1] = '?';
        global_estado.puntajes_registros[i].nombre[2] = '?';
    }

    global_config_t *cnf = &global_estado.config;
    cnf->paleta = 0;
    cnf->velocidad = 400; //0.4s
    cnf->mw = 10;
    cnf->mh = 20;

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

void global_registar_puntaje(unsigned puntaje, char nombre[3])
{

    puntajereg_t *reg1;
    puntajereg_t *reg2;

    for (int i = 0; i < ESTADO_GLOBAL_MAXPUNTS; i++)
    {
        puntajereg_t *reg1 = global_estado.puntajes_registros + i;
        if (puntaje < reg1->puntaje) continue;
        for (int j = ESTADO_GLOBAL_MAXPUNTS-1; j > i; j--)
        {
            reg2 = global_estado.puntajes_registros + j;
            *reg2 = *(reg2 - 1);
        }
        reg1->puntaje = puntaje;
        strcpy_s(reg1->nombre, sizeof(char) * 3, nombre);
    }

}
puntajereg_t* global_obtener_puntajes()
{

    return global_estado.puntajes_registros;

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
