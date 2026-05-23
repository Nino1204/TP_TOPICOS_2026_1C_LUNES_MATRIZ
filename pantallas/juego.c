#include "juego.h"

#include <stdio.h>
#include <memory.h>

#include "GBT/gbt.h"

#include "../extras/imagenes.h"
#include "../extras/utils.h"
#include "../extras/estado_global.h"
#include "../entidades/mapa.h"
#include "../entidades/formas.h"

#include "menupunt.h"

//la cantidad maxima de formas anteriores usadas que se pueden guardar
//se usa en el algoritmo para elegir la proxima forma a dar
#define JUEGO_MAX_FORMAS_USADAS 6
#define SCORE_TEXTO_FORMATO "score:%08u"
#define VEL_TEXTO_FORMATO "tiempo caida:%ums"

//cuantas veces se tiene que repetir el temp para que baje la pieza
#define TICK_MOV_CANT 20

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

    int tick_cont;
    int piezas_puestas;
    int por_caer; //si la pieza esta por chocar contra el tablero
    unsigned tiempo_caida;

    unsigned char formas_anteriores[JUEGO_MAX_FORMAS_USADAS];

    unsigned char estado_actual;
    int pausa_primer_frame;
    int menu_seleccion;

    //contador de algoritmo (para elegir siguiente forma), si se pasa de 255 intentos se termina
    //por las dudas de un stack-overflow supongo
    unsigned char alg_cont;

    unsigned score;
    char score_texto[16];
    char vel_texto[32];

} estado_j; //data del juego

//llamado cuando una forma choca (o al principio), actualiza la forma usada y la que viene despues
void juego_cambiar_formas();
//devuelve el id de la forma que tendria que seguir
int juego_siguiente_forma();

void juego_reiniciar();
int juego_revisar_mapa();
void juego_ganar_puntos(int puntos);

void juego_perdio();

void juego_iniciar()
{

    global_config_t conf = *(global_config_t*) global_obtener_config_ptr();

    estado_j.mapa = mapa_crear(conf.mw, conf.mh);
    estado_j.mapa_origenx = VENTANA_ANCHO/2-conf.mw*MAPA_BLOQUE_TAM/2;
    estado_j.mapa_origeny = VENTANA_ALTO-conf.mh*MAPA_BLOQUE_TAM;
    estado_j.menu_seleccion = MENU_CONTINUAR;

    //poner las paredes verticales
    for (int y = 0; y < conf.mh; y++)
    {
        mapa_poner_bloque(estado_j.mapa, 0,y, 1);
        mapa_poner_bloque(estado_j.mapa, conf.mw-1,y, 1);
    }

    //datos para el algoritmo de siguiente pieza
    estado_j.alg_cont = 0;
    memset(estado_j.formas_anteriores, 0xFF, sizeof(unsigned char) * JUEGO_MAX_FORMAS_USADAS);

    juego_siguiente_forma(); //crea la primera forma a usar
    juego_cambiar_formas(); //utiliza esa forma y crea la siguiente

    estado_j.cant_lns_elmin = 0;
    estado_j.piezas_puestas = 0;
    estado_j.tiempo_caida = conf.velocidad;

    estado_j.tick_cont = 0;
    estado_j.tick_temp = gbt_temporizador_crear(estado_j.tiempo_caida * 0.001f * 1.0f/(float)TICK_MOV_CANT);
    estado_j.lineas_temp = gbt_temporizador_crear(0.1);
    estado_j.por_caer = 0;

    //se empieza con el juego normal
    estado_j.estado_actual = JUEGO_ESTADO_NORMAL;

    estado_j.score = 0;
    juego_ganar_puntos(0); //actualiza el texto de score (sin ganar puntos)

    sprintf_s(estado_j.vel_texto, 32, VEL_TEXTO_FORMATO, estado_j.tiempo_caida);

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
    utils_dibujar_cuadradolineas( f_vp.px+1,f_vp.py+1, 32,32, 14 ); //cuadrado sombra
    utils_dibujar_cuadradolineas( f_vp.px,f_vp.py, 32,32, 13 );
    forma_dibujar_vistaprevia(f_vp);

    utils_dibujar_lineah(estado_j.mapa_origeny-1, 0, VENTANA_ANCHO, 14);
    utils_dibujar_lineah(estado_j.mapa_origeny-2, 0, VENTANA_ANCHO, 13);

    UTILS_TEXTO_SOMBREADO(0,2, estado_j.score_texto)
    UTILS_TEXTO_SOMBREADO(0,12, estado_j.vel_texto)

    utils_dibujar_cuadradolineas(estado_j.mapa_origenx,estado_j.mapa_origeny, estado_j.mapa.ancho*MAPA_BLOQUE_TAM,estado_j.mapa.alto*MAPA_BLOQUE_TAM, 13);

    //dibujar teclas
    int ty = VENTANA_ALTO/2;
    char teclas[][12] = {"mover der", "mover izq", "rotar der", "rotar izq", "bajar", "colocar", "pausa"};
    for (int i = 0; i < IMGTECLAS_CANT; i++)
    {
        ty = VENTANA_ALTO/2 + 10*(i-(int)IMGTECLAS_CANT/2);
        utils_dibujar_imagen(2,ty, 8,8, Imagenes_ObtenerPixeles(IMAGENES_ID_TECLAS, i));
        utils_dibujar_texto(10,ty, teclas[i], 13);
    }

    if (estado_j.estado_actual == JUEGO_ESTADO_PAUSADO)
        juego_pausa();

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

    int forma_cayo = 0;

    //limpiar la forma anterior
    //por si se tiene que mover o rotar
    forma_limpiar_de_mapa(estado_j.forma, estado_j.mapa);

    eGBT_Tecla tecla = gbt_obtener_tecla_presionada();
    switch (tecla)
    {

    case GBTK_DERECHA:
        if (forma_puede_deslizar(estado_j.forma, estado_j.mapa, 1)) estado_j.forma.px++;
        if (estado_j.por_caer) estado_j.tick_cont = TICK_MOV_CANT/2;
        break;
    case GBTK_IZQUIERDA:
        if (forma_puede_deslizar(estado_j.forma, estado_j.mapa, -1)) estado_j.forma.px--;
        if (estado_j.por_caer) estado_j.tick_cont = TICK_MOV_CANT/2;
        break;
    case GBTK_a:
        if (forma_puede_rotar(estado_j.forma, estado_j.mapa, -1)) forma_rotar(&estado_j.forma, -1);
        break;
    case GBTK_d:
        if (forma_puede_rotar(estado_j.forma, estado_j.mapa, 1)) forma_rotar(&estado_j.forma, 1);
        break;
    case GBTK_ESPACIO:
        int bajados = 0;
        while (forma_puede_bajar(estado_j.forma, estado_j.mapa))
        {
            estado_j.forma.py++;
            bajados++;
        }
        forma_cayo = 1;
        estado_j.tick_cont = TICK_MOV_CANT;
        juego_ganar_puntos(bajados*20);
        break;
    case GBTK_p:
        estado_j.estado_actual = JUEGO_ESTADO_PAUSADO;
        estado_j.pausa_primer_frame = 1;
        estado_j.menu_seleccion = MENU_CONTINUAR;
        break;

    }

    if (gbt_temporizador_consumir(estado_j.tick_temp))
        estado_j.tick_cont--;

    if (estado_j.tick_cont == 0 || tecla == GBTK_ABAJO)
    {

        if (tecla == GBTK_ABAJO) juego_ganar_puntos(10);

        estado_j.por_caer = 0;
        if (forma_puede_bajar(estado_j.forma, estado_j.mapa))
        {
            estado_j.forma.py++;
            estado_j.por_caer = !forma_puede_bajar(estado_j.forma, estado_j.mapa);
        }
        else
            forma_cayo = 1;

        estado_j.tick_cont = TICK_MOV_CANT;
    }

    if (forma_cayo)
    {

        //aumentar la velocidad cada 10 piezas caidas (%3)
        estado_j.piezas_puestas++;
        if (estado_j.piezas_puestas >= 10)
        {
            estado_j.tiempo_caida = (int)((float)estado_j.tiempo_caida * 0.97f);
            gbt_temporizador_destruir(estado_j.tick_temp);
            estado_j.tick_temp = gbt_temporizador_crear(estado_j.tiempo_caida * 0.001f * 1.0f/(float)TICK_MOV_CANT);
            estado_j.piezas_puestas = 0;
            sprintf_s(estado_j.vel_texto, 32, VEL_TEXTO_FORMATO, estado_j.tiempo_caida);
        }

        forma_poner_en_mapa(estado_j.forma, estado_j.mapa);
        //revisar si se limpio una linea
        if (juego_revisar_mapa())
        {
            juego_ganar_puntos(500);
            for (int i = 0; i < estado_j.cant_lns_elmin; i++)
                juego_ganar_puntos( i*1000 );
            return;
        }
        //nota: si se limpio una linea no se va a cambiar de formas hasta que termine la animacion.
        //esto es para que se pueda ver la siguiente forma mientras la animacion se esta ejecutando
        juego_cambiar_formas();
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
        for (x = 0; x < estado_j.mapa.ancho; x++)
        {
            unsigned char bloque = mapa_obtener_bloque(mapa, x,linea);
            if (bloque == 1) continue;
            if (bloque != 0)
                break;
        }

        //se llego al final del mapa
        if (x == mapa.ancho)
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
        juego_cambiar_formas();
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
        juego_perdio();
        return;
    }

    //---grabar la forma usada (unas lineas arriba)---
    //empujo las anteriores hacia atras, asi queda la posicion 0 libre
    for (int i = JUEGO_MAX_FORMAS_USADAS-1; i > 0; i--)
        estado_j.formas_anteriores[i] = estado_j.formas_anteriores[i-1];
    //pongo la forma adelate
    estado_j.formas_anteriores[0] = estado_j.forma.f_id;

    //crear siguiente forma
    estado_j.forma_sig = forma_crear(
         VENTANA_ANCHO/2-16,2, juego_siguiente_forma()
    );

}
int juego_siguiente_forma()
{

    unsigned char sig_forma_id = rand()%FORMA_ID_CANTIDAD;

    //en el caso que haya estado intentando durante muchos frames,
    //directamente devuelvo la primera opcion
    if (estado_j.alg_cont == 255)
    {
        estado_j.alg_cont = 0;
        return sig_forma_id;
    }

    estado_j.alg_cont++;

    unsigned chances;
    unsigned char *f_anteriores = estado_j.formas_anteriores;

    chances = 1;
    for (int i = 0; i < JUEGO_MAX_FORMAS_USADAS; i++)
    {
        if (f_anteriores[i] == sig_forma_id)
            chances += (JUEGO_MAX_FORMAS_USADAS-i)*2;
    }

    //las posibilidades de que se acepte esta forma son 1 en [chances]
    //mientras mas se haya repetido la forma (y mas recientemente) menos chances hay
    //de que se elija

    int resuelto = (rand()%chances) == 0;
    if (!resuelto)
    {
        //porfavor, que se note la recursion
        sig_forma_id = juego_siguiente_forma();
    }

    estado_j.alg_cont = 0; //se termino el algoritmo, reseteo

    return sig_forma_id;

}

void juego_pausa()
{
    /*
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
*/
    //dibujo menu
    utils_dibujar_texto(VENTANA_ANCHO-6*7,0,"pausa",13);
    utils_dibujar_texto(VENTANA_ANCHO/2 -20,VENTANA_ALTO/2 -8,"reanudar",13);
    utils_dibujar_texto(VENTANA_ANCHO/2 -20,VENTANA_ALTO/2,"reiniciar",13);
    utils_dibujar_texto(VENTANA_ANCHO/2 -20,VENTANA_ALTO/2 + 8,"salir",13);

    //dibujo flecha de seleccion
    switch(estado_j.menu_seleccion)
        {
        case MENU_CONTINUAR:
            utils_dibujar_texto(VENTANA_ANCHO/2 -40,VENTANA_ALTO/2 -8,"0",13);
            break;
        case MENU_REINICIAR:
            utils_dibujar_texto(VENTANA_ANCHO/2 -40,VENTANA_ALTO/2,"0",13);
            break;
        case MENU_SALIR:
            utils_dibujar_texto(VENTANA_ANCHO/2 -40,VENTANA_ALTO/2 +8,"0",13);
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
                global_siguiente_pantalla(PANTALLA_MENUPRINC);
                break;
            }
        }
    }

}

void juego_perdio()
{

    puntajereg_t *reg_ptr = global_obtener_puntaje_ptr();

    reg_ptr->puntaje = estado_j.score;

    global_siguiente_pantalla(PANTALLA_GAMEOVER);
    menupunt_esconder_titulo(0); //que se muestre el "GAME OVER"

}

void juego_ganar_puntos(int puntos)
{

    global_config_t conf = *(global_config_t*)global_obtener_config_ptr();

    float div = (float)estado_j.tiempo_caida / (float)conf.velocidad;

    estado_j.score += (int)( (float)puntos / div );

    estado_j.score_texto[0] = '\0';
    sprintf_s(estado_j.score_texto, 16, SCORE_TEXTO_FORMATO, estado_j.score);

}
