#include "juego.h"

#include <stdio.h>

#include "GBT/gbt.h"

#include "../extras/imagenes.h"
#include "../extras/utils.h"
#include "../entidades/mapa.h"
#include "../entidades/formas.h"

#define MAPA_ANCHO 10
#define MAPA_ALTO 20

//la cantidad maxima de formas anteriores usadas que se pueden guardar
//se usa en el algoritmo para elegir la proxima forma a dar
#define JUEGO_MAX_FORMAS_USADAS 6
#define SCORE_TEXTO_FORMATO "score:%08u"

enum {
    JUEGO_ESTADO_PAUSADO, //el juego esta pausado
    JUEGO_ESTADO_NORMAL, //el juego se esta ejecutando normalmente
    JUEGO_ESTADO_ANIM_LINEAS //se esta ejecutando la animacion de lineas eliminadas
};

enum {
    MENU_CONTINUAR,
    MENU_REINICIAR,
    MENU_SALIR,
    MENU_CANTIDAD
};

void juego_actualizar_normal();
void juego_actualizar_animlineas();
void juego_pausa();

struct {

    mapa_t mapa;
    int mapa_origenx, mapa_origeny;
    forma_t forma, forma_sig;
    tGBT_Temporizador *tick_temp;
    tGBT_Temporizador *lineas_temp;
    unsigned lineas_eliminadas[4];
    unsigned cant_lns_elmin;

    unsigned char estado_actual;
    int pausa_primer_frame;
    int menu_seleccion;

    unsigned score;
    char score_texto[16];

} estado_j; //data del juego

//llamado cuando una forma choca (o al principio), actualiza la forma usada y la que viene despues
void juego_cambiar_formas();
//devuelve el id de la forma que tendria que seguir
void juego_siguiente_forma();

void juego_reiniciar();
int juego_revisar_mapa();

void juego_iniciar()
{

    estado_j.mapa = mapa_crear(MAPA_ANCHO, MAPA_ALTO);
    estado_j.mapa_origenx = VENTANA_ANCHO/2-MAPA_ANCHO*MAPA_BLOQUE_TAM/2;
    estado_j.mapa_origeny = VENTANA_ALTO-MAPA_ALTO*MAPA_BLOQUE_TAM;
    estado_j.menu_seleccion = MENU_CONTINUAR;

    //poner las paredes verticales
    for (int y = 0; y < MAPA_ALTO; y++)
    {
        mapa_poner_bloque(estado_j.mapa, 0,y, 1);
        mapa_poner_bloque(estado_j.mapa, MAPA_ANCHO-1,y, 1);
    }

    juego_siguiente_forma(); //crea la primera forma a usar
    juego_cambiar_formas(); //utiliza esa forma y crea la siguiente

    estado_j.tick_temp = gbt_temporizador_crear(0.4);
    estado_j.lineas_temp = gbt_temporizador_crear(0.1);

    estado_j.cant_lns_elmin = 0;

    //se empieza con el juego normal
    estado_j.estado_actual = JUEGO_ESTADO_NORMAL;

    estado_j.score = 0;

    sprintf(estado_j.score_texto, SCORE_TEXTO_FORMATO, estado_j.score);

}
void juego_actualizar()
{

    switch (estado_j.estado_actual)
    {

    case JUEGO_ESTADO_NORMAL:
        juego_actualizar_normal();
        break;
    case JUEGO_ESTADO_ANIM_LINEAS:
        juego_actualizar_animlineas();
        break;
    case JUEGO_ESTADO_PAUSADO:
        if (gbt_obtener_tecla_presionada() == GBTK_p)
            estado_j.estado_actual = JUEGO_ESTADO_NORMAL;
        break;

    }

}
void juego_dibujar()
{

    mapa_dibujar_sombra(estado_j.mapa, estado_j.mapa_origenx+1,estado_j.mapa_origeny+1);
    mapa_dibujar(estado_j.mapa, estado_j.mapa_origenx,estado_j.mapa_origeny);

    forma_t f_vp = estado_j.forma_sig;
    utils_dibujar_cuadradolineas( f_vp.px+1,f_vp.py+1, 32,32, 9 ); //cuadrado sombra
    utils_dibujar_cuadradolineas( f_vp.px,f_vp.py, 32,32, 15 );
    forma_dibujar_vistaprevia(f_vp);

    utils_dibujar_lineah(estado_j.mapa_origeny-2, 0, VENTANA_ANCHO, 15);
    utils_dibujar_lineah(estado_j.mapa_origeny-1, 0, VENTANA_ANCHO, 9);

    utils_dibujar_texto( 1,3, estado_j.score_texto,9 );
    utils_dibujar_texto( 0,2, estado_j.score_texto,15 );

    if (estado_j.estado_actual == JUEGO_ESTADO_PAUSADO)
    {
        juego_pausa();
    }

}
void juego_cerrar()
{
    mapa_destruir(estado_j.mapa);
    gbt_temporizador_destruir(estado_j.tick_temp);
    gbt_temporizador_destruir(estado_j.lineas_temp);
}

void juego_reiniciar()
{

    juego_cerrar();
    juego_iniciar();

}

int juego_revisar_mapa()
{

    mapa_t mapa = estado_j.mapa;

    estado_j.cant_lns_elmin = 0;

    for (int y = 0; y < mapa.alto; y++)
    {
        if (!mapa_linea_llena(mapa, y))
            continue;
        estado_j.lineas_eliminadas[ estado_j.cant_lns_elmin ] = y;
        estado_j.cant_lns_elmin++;
    }

    if (estado_j.cant_lns_elmin > 0)
    {
        estado_j.estado_actual = JUEGO_ESTADO_ANIM_LINEAS;
        return 1;
    }

    return 0;

}

void juego_actualizar_normal() //el juego normal
{

    //limpiar la forma anterior
    //porque si se tiene que mover o rotar
    forma_limpiar_de_mapa(estado_j.forma, estado_j.mapa);

    eGBT_Tecla tecla = gbt_obtener_tecla_presionada();
    switch (tecla)
    {

    case GBTK_DERECHA:
        if (forma_puede_deslizar(estado_j.forma, estado_j.mapa, 1))
            estado_j.forma.px++;
        break;
    case GBTK_IZQUIERDA:
        if (forma_puede_deslizar(estado_j.forma, estado_j.mapa, -1))
            estado_j.forma.px--;
        break;
    case GBTK_ESPACIO:
        if (forma_puede_rotar(estado_j.forma, estado_j.mapa))
            forma_rotar(&estado_j.forma);
        break;
    case GBTK_p:
        estado_j.estado_actual = JUEGO_ESTADO_PAUSADO;
        estado_j.pausa_primer_frame = 1;
        estado_j.menu_seleccion = MENU_CONTINUAR;
        break;

    }

    if (gbt_temporizador_consumir(estado_j.tick_temp) || tecla == GBTK_ABAJO)
    {
        if (forma_puede_bajar(estado_j.forma, estado_j.mapa))
        {
            estado_j.forma.py++;
        }
        else
        {
            forma_poner_en_mapa(estado_j.forma, estado_j.mapa);
            juego_cambiar_formas();
            //revisar si se limpio una linea
            if (juego_revisar_mapa())
            {
                for (int i = 0; i < estado_j.cant_lns_elmin; i++)
                {
                    estado_j.score += estado_j.cant_lns_elmin*1000 + i*200;
                }
                estado_j.score_texto[0] = '\0';
                sprintf(estado_j.score_texto, SCORE_TEXTO_FORMATO, estado_j.score);
                return;
            }
        }
    }

    forma_poner_en_mapa(estado_j.forma, estado_j.mapa);

}
void juego_actualizar_animlineas() //la animacion de lineas eliminadas
{

    if (!gbt_temporizador_consumir(estado_j.lineas_temp))
        return;

    unsigned linea;
    int anim_terminada = 0;

    mapa_t mapa = estado_j.mapa;

    for (int i = 0; i < estado_j.cant_lns_elmin; i++)
    {
        linea = estado_j.lineas_eliminadas[i];
        int x;
        for (x = 1; x < estado_j.mapa.ancho-1; x++)
        {
            if (!mapa_posicion_es_vacia(mapa, x,linea))
                break;
        }

        //se llego al final del mapa
        if (x == mapa.ancho-1)
        {
            mapa_eliminar_linea(mapa, linea);
            anim_terminada = 1;
        }
        else
            mapa_poner_bloque(mapa, x,linea, 0);

    }

    //resumir el juego cuando termina animacion
    if (anim_terminada)
    {
        mapa_revisar_lineas(mapa);
        estado_j.estado_actual = JUEGO_ESTADO_NORMAL;
    }

}

void juego_cambiar_formas()
{

    estado_j.forma = forma_crear(
        estado_j.mapa.ancho/2-2,0,
        estado_j.forma_sig.f_id
    );

    //si la forma que recien se crea arriba de todo
    //tiene colision con el mapa, significa que se perdio
    if (forma_tiene_colision(estado_j.forma, estado_j.mapa))
    {
        juego_reiniciar();
        return;
    }

    juego_siguiente_forma();

}
void juego_siguiente_forma()
{
    estado_j.forma_sig = forma_crear(VENTANA_ANCHO/2-16,2, rand()%FORMA_ID_CANTIDAD);
}

void juego_pausa()
{
    //Oscurezco el fondo primero
    for (int y = 0; y < VENTANA_ALTO; y++)
        {
            for (int x = 0; x < VENTANA_ANCHO; x++)
            {
                unsigned char color = gbt_obtener_color_pixel(x,y);
                unsigned char color_oscuro = Imagenes_OscurecerPixel(color);
                gbt_dibujar_pixel(x,y, color_oscuro);
            }
        }

    //dibujo menu
    utils_dibujar_texto(VENTANA_ANCHO-6*7,0,"pausa",15);
    utils_dibujar_texto(VENTANA_ANCHO/2 -20,VENTANA_ALTO/2 -8,"reanudar",15);
    utils_dibujar_texto(VENTANA_ANCHO/2 -20,VENTANA_ALTO/2,"reiniciar",15);
    utils_dibujar_texto(VENTANA_ANCHO/2 -20,VENTANA_ALTO/2 + 8,"salir",15);

    //dibujo flecha de seleccion
    switch(estado_j.menu_seleccion)
        {
        case MENU_CONTINUAR:
            utils_dibujar_texto(VENTANA_ANCHO/2 -40,VENTANA_ALTO/2 -8,"0",15);
            break;
        case MENU_REINICIAR:
            utils_dibujar_texto(VENTANA_ANCHO/2 -40,VENTANA_ALTO/2,"0",15);
            break;
        case MENU_SALIR:
            utils_dibujar_texto(VENTANA_ANCHO/2 -40,VENTANA_ALTO/2 +8,"0",15);
            break;
        }

    eGBT_Tecla tecla;

    if (estado_j.pausa_primer_frame)
    {
        tecla = 0;
        estado_j.pausa_primer_frame = 0;
    }
    else
    {
        //control del menu:
        tecla = gbt_obtener_tecla_presionada();
        switch(tecla)
        {
        case GBTK_ARRIBA:
            estado_j.menu_seleccion--;
            if (estado_j.menu_seleccion < 0)
            {
                estado_j.menu_seleccion = MENU_CANTIDAD - 1;
            }
            break;
        case GBTK_ABAJO:
            estado_j.menu_seleccion++;
            if (estado_j.menu_seleccion >= MENU_CANTIDAD)
            {
                estado_j.menu_seleccion = 0;
            }
            break;
        }

        // controla el la opcion seleccionada
        if (tecla == GBTK_ENTER)
        {
            switch(estado_j.menu_seleccion)
            {
            case MENU_CONTINUAR:
                estado_j.estado_actual = JUEGO_ESTADO_NORMAL;
                break;
            case MENU_REINICIAR:
                juego_reiniciar();
                break;
            case MENU_SALIR:
                juego_cerrar();
                gbt_cerrar();
            }
        }
    }

}
