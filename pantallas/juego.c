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
#define JUEGO_TICK_TEMP gbt_temporizador_crear(estado_j.tiempo_caida * 0.001f * 1.0f/(float)TICK_MOV_CANT)

//cuantas veces se tiene que repetir el temp para que baje la pieza
#define TICK_MOV_CANT 20

enum {
    JUEGO_ESTADO_PAUSADO, //el juego esta pausado
    JUEGO_ESTADO_NORMAL, //el juego se esta ejecutando normalmente
    JUEGO_ESTADO_ANIM_LINEAS, //se esta ejecutando la animacion de lineas eliminadas
    JUEGO_ESTADO_CHEATS //se ejecuta el menu de seleccion de cheats
};

enum {
    PAUSA_CONTINUAR,
    PAUSA_REINICIAR,
    PAUSA_SALIR,
    PAUSA_CANTIDAD
};
enum {
    CHEATS_VEL, //realentizar
    CHEATS_FOR, // re-elegir forma siguiente
    CHEATS_CLR, //limpiar mapa
    CHEATS_VOLVER, //volver al juego
    CHEATS_CANT
};

void juego_actualizar_normal();
void juego_actualizar_animlineas();
void juego_actualizar_pausa();
void juego_dibujar_pausa();
void juego_actualizar_cheats();
void juego_dibujar_cheats();

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
    int pausa_seleccion;
    int cheats_seleccion;

    //contador de algoritmo (para elegir siguiente forma), si se pasa de 255 intentos se termina
    //por las dudas de un stack-overflow supongo
    unsigned char alg_cont;

    unsigned score;
    char score_texto[16];
    char vel_texto[32];

} estado_j; //data del juego

juego_savefile_t juego_save;

//llamado cuando una forma choca (o al principio), actualiza la forma usada y la que viene despues
void juego_cambiar_formas();
//devuelve el id de la forma que tendria que seguir
int juego_siguiente_forma();

void juego_reiniciar();
int juego_revisar_mapa();
void juego_ganar_puntos(int puntos);

void juego_perdio();

void juego_cargar_savefile();
void juego_guardar_savefile();

void juego_iniciar()
{

    global_config_t conf = *(global_config_t*) global_obtener_config_ptr();

    if (conf.modo_juego == MODOJUEGO_CLASICO) conf.mw += 2;

    estado_j.mapa = mapa_crear(conf.mw, conf.mh);
    estado_j.mapa_origenx = VENTANA_ANCHO/2-conf.mw*MAPA_BLOQUE_TAM/2;
    estado_j.mapa_origeny = VENTANA_ALTO-conf.mh*MAPA_BLOQUE_TAM;
    estado_j.pausa_seleccion = PAUSA_CONTINUAR;

    if (conf.modo_juego == MODOJUEGO_CLASICO)
    {
        //poner las paredes verticales
        for (int y = 0; y < conf.mh; y++)
        {
            mapa_poner_bloque(estado_j.mapa, 0,y, 1);
            mapa_poner_bloque(estado_j.mapa, conf.mw-1,y, 1);
        }
    }

    //datos para el algoritmo de siguiente pieza
    estado_j.alg_cont = 0;
    memset(estado_j.formas_anteriores, 0xFF, sizeof(unsigned char) * JUEGO_MAX_FORMAS_USADAS);

    estado_j.forma_sig.f_id = juego_siguiente_forma(); //crea la primera forma a usar
    juego_cambiar_formas(); //utiliza esa forma y crea la siguiente

    estado_j.cant_lns_elmin = 0;
    estado_j.piezas_puestas = 0;
    estado_j.tiempo_caida = conf.velocidad;

    estado_j.tick_cont = 0;
    estado_j.tick_temp = JUEGO_TICK_TEMP;
    estado_j.lineas_temp = gbt_temporizador_crear(0.1);
    estado_j.por_caer = 0;

    //se empieza con el juego normal
    estado_j.estado_actual = JUEGO_ESTADO_NORMAL;

    estado_j.score = 0;
    juego_ganar_puntos(0); //actualiza el texto de score (sin ganar puntos)

    sprintf_s(estado_j.vel_texto, 32, VEL_TEXTO_FORMATO, estado_j.tiempo_caida);

    if (global_usa_savefile())
        juego_cargar_savefile();

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
        juego_actualizar_pausa();
        break;
    case JUEGO_ESTADO_CHEATS:
        juego_actualizar_cheats();
        break;

    }

}
void juego_dibujar()
{

    if (estado_j.estado_actual == JUEGO_ESTADO_PAUSADO)
    {
        juego_dibujar_pausa();
        return;
    }

    if (estado_j.estado_actual == JUEGO_ESTADO_CHEATS)
    {
        juego_dibujar_cheats();
        return;
    }

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
    char teclas[][12] = {"mover der", "mover izq", "rotar der", "rotar izq", "bajar", "colocar", "pausa", "cheats"};
    for (int i = 0; i < IMGTECLAS_CANT; i++)
    {
        ty = VENTANA_ALTO/2 + 10*(i-(int)IMGTECLAS_CANT/2);
        utils_dibujar_imagen(2,ty, 8,8, Imagenes_ObtenerPixeles(IMAGENES_ID_TECLAS, i));
        utils_dibujar_texto(10,ty, teclas[i], 13);
    }

}
void juego_cerrar()
{

    //guardar la partida, a menos que haya perdido
    if (global_obtener_pantalla_actual() != PANTALLA_GAMEOVER)
        juego_guardar_savefile();

    mapa_destruir(estado_j.mapa);
    gbt_temporizador_destruir(estado_j.tick_temp);
    gbt_temporizador_destruir(estado_j.lineas_temp);
}

void juego_reiniciar()
{

    global_usar_savefile(0);

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

//-------------------------ESTADOS-------------------------
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
        if (forma_puede_deslizar(estado_j.forma, estado_j.mapa, 1))
        {
            estado_j.forma.px++;
            if (estado_j.por_caer)
            {
                estado_j.tick_cont = TICK_MOV_CANT/(2+2*global_dificultad()); //menos tolerancia para piso en dificil
                estado_j.por_caer = forma_puede_bajar(estado_j.forma,estado_j.mapa);
            }
        }
        break;
    case GBTK_IZQUIERDA:
        if (forma_puede_deslizar(estado_j.forma, estado_j.mapa, -1))
        {
            estado_j.forma.px--;
            if (estado_j.por_caer)
            {
                estado_j.tick_cont = TICK_MOV_CANT/(2+2*global_dificultad()); //menos tolerancia para piso en dificil
                estado_j.por_caer = forma_puede_bajar(estado_j.forma,estado_j.mapa);
            }
        }
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
        estado_j.pausa_seleccion = PAUSA_CONTINUAR;
        break;
    case GBTK_c:
        estado_j.estado_actual = JUEGO_ESTADO_CHEATS;
        estado_j.cheats_seleccion = 0;
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
        int pmax = (global_dificultad() == DIF_NORMAL) ? 10 : 5; //tiempocaida cambia mas rapido en dificil
        if (estado_j.piezas_puestas >= pmax)
        {
            estado_j.tiempo_caida = (int)((float)estado_j.tiempo_caida * 0.97f);
            gbt_temporizador_destruir(estado_j.tick_temp);
            estado_j.tick_temp = JUEGO_TICK_TEMP;
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
void juego_actualizar_pausa()
{
    eGBT_Tecla tecla = gbt_obtener_tecla_presionada();

    unsigned char boton_presionado = 0;

    switch (tecla)
    {
    case GBTK_ARRIBA:
        estado_j.pausa_seleccion--;
        if (estado_j.pausa_seleccion < 0) estado_j.pausa_seleccion = PAUSA_CANTIDAD-1;
        break;
    case GBTK_ABAJO:
        estado_j.pausa_seleccion++;
        if (estado_j.pausa_seleccion >= PAUSA_CANTIDAD) estado_j.pausa_seleccion = 0;
        break;
    case GBTK_ENTER:
        boton_presionado = estado_j.pausa_seleccion+1;
        break;
    }

    if (boton_presionado == 0) return;
    boton_presionado--;

    switch (boton_presionado)
    {
    case PAUSA_CONTINUAR:
        estado_j.estado_actual = JUEGO_ESTADO_NORMAL;
        break;
    case PAUSA_REINICIAR:
        juego_reiniciar();
        break;
    case PAUSA_SALIR:
        global_siguiente_pantalla(PANTALLA_MENUPRINC);
        break;
    }

}
void juego_dibujar_pausa()
{

    mapa_dibujar_sombra(estado_j.mapa, estado_j.mapa_origenx,estado_j.mapa_origeny);

    char botones[PAUSA_CANTIDAD][16]  = {"reanudar","reiniciar","salir"};
    unsigned blen;
    int px,py;
    py = VENTANA_ALTO/2 - PAUSA_CANTIDAD*5;

    for (int i = 0; i < PAUSA_CANTIDAD; i++)
    {
        blen = utils_ancho_de_texto_nm(botones[i]);
        px = (int) (VENTANA_ANCHO*0.5f-blen*0.5f);
        utils_dibujar_texto_nm(px, py+i*10, botones[i], 13);
        if (i == estado_j.pausa_seleccion)
            utils_dibujar_cuadradolineas(px-2,py+i*10-2, blen+2,8, 13);
    }

    UTILS_TEXTO_SOMBREADO(1,1,"PAUSA")

}

void juego_actualizar_cheats()
{

    eGBT_Tecla tecla = gbt_obtener_tecla_presionada();

    unsigned char boton_presionado = 0;

    switch (tecla)
    {
    case GBTK_ARRIBA:
        estado_j.cheats_seleccion--;
        if (estado_j.cheats_seleccion < 0) estado_j.cheats_seleccion = CHEATS_CANT-1;
        break;
    case GBTK_ABAJO:
        estado_j.cheats_seleccion++;
        if (estado_j.cheats_seleccion >= CHEATS_CANT) estado_j.cheats_seleccion = 0;
        break;
    case GBTK_ENTER:
        boton_presionado = estado_j.cheats_seleccion+1;
        break;
    }

    if (boton_presionado == 0) return;
    boton_presionado--;

    switch (boton_presionado)
    {
    case CHEATS_VEL:
        estado_j.tiempo_caida += 200;
        gbt_temporizador_destruir(estado_j.tick_temp);
        estado_j.tick_temp = JUEGO_TICK_TEMP;
        sprintf_s(estado_j.vel_texto, sizeof(char) * 32, VEL_TEXTO_FORMATO, estado_j.tiempo_caida); //actualizar texto
        break;
    case CHEATS_CLR:
        for (int y = 0; y < estado_j.mapa.alto; y++)
        {
            unsigned char *tab = estado_j.mapa.tablero[y];
            for (int x = 0; x < estado_j.mapa.ancho; x++)
                if (tab[x] != 1) tab[x] = 0;
        }
        estado_j.estado_actual = JUEGO_ESTADO_NORMAL;
        break;
    case CHEATS_FOR:
        int fx = estado_j.forma_sig.px;
        int fy = estado_j.forma_sig.py;
        estado_j.forma_sig = forma_crear(fx,fy,juego_siguiente_forma());
        estado_j.estado_actual = JUEGO_ESTADO_NORMAL;
        break;
    case CHEATS_VOLVER:
        estado_j.estado_actual = JUEGO_ESTADO_NORMAL;
        break;
    }

}
void juego_dibujar_cheats()
{

    mapa_dibujar_sombra(estado_j.mapa, estado_j.mapa_origenx,estado_j.mapa_origeny);

    char botones[CHEATS_CANT][24]  = {"realentizar:200ms","re?elegir forma","limpiar mapa","volver"};
    unsigned blen;
    int px,py;
    py = VENTANA_ALTO/2 - CHEATS_CANT*5;

    for (int i = 0; i < CHEATS_CANT; i++)
    {
        blen = utils_ancho_de_texto_nm(botones[i]);
        px = (int) (VENTANA_ANCHO*0.5f-blen*0.5f);
        utils_dibujar_texto_nm(px, py+i*10, botones[i], 13);
        if (i == estado_j.cheats_seleccion)
            utils_dibujar_cuadradolineas(px-2,py+i*10-2, blen+2,8, 13);
    }

}

//-------------------------FORMAS-------------------------
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

    unsigned cant = FORMA_ID_CANTIDAD;
    if (global_obtener_config_ptr()->modo_juego == MODOJUEGO_DX)
        cant = FORMA_ID_CANTIDADDX;
    unsigned char sig_forma_id = rand()%cant;

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
            chances += (JUEGO_MAX_FORMAS_USADAS-i)*4;
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

void juego_perdio()
{

    puntajereg_t *reg_ptr = global_obtener_puntaje_ptr();

    reg_ptr->puntaje = estado_j.score;

    global_siguiente_pantalla(PANTALLA_GAMEOVER);
    menupunt_esconder_titulo(0); //que se muestre el "GAME OVER"

}

//-------------------------SAVEFILE-------------------------
void juego_cargar_savefile()
{

    FILE *f = fopen("save.dat", "rb");

    if (f == NULL)
        return;

    fread(&juego_save, sizeof(juego_savefile_t), 1, f);

    //si el savefile estaba en otro modo de juego, no usarlo
    //por si el jugador guarda en clasico, cambia a dx y lo carga
    if (juego_save.modo != global_obtener_config_ptr()->modo_juego)
    {
        fclose(f);
        return;
    }

    for (int i = 0; i < estado_j.mapa.alto; i++)
        fread(estado_j.mapa.tablero[i], sizeof(unsigned char), estado_j.mapa.ancho, f);

    estado_j.score = juego_save.score;
    juego_ganar_puntos(0);

    estado_j.tiempo_caida = juego_save.tiempo_caida;
    sprintf_s(estado_j.vel_texto, sizeof(unsigned char)*32, VEL_TEXTO_FORMATO, estado_j.tiempo_caida);

    estado_j.piezas_puestas = juego_save.piezas_puestas;
    estado_j.forma = juego_save.forma_actual;
    estado_j.forma_sig = juego_save.forma_siguiente;

    fclose(f);

}
void juego_guardar_savefile()
{

    juego_save.forma_actual = estado_j.forma;
    juego_save.forma_siguiente = estado_j.forma_sig;
    juego_save.piezas_puestas = estado_j.piezas_puestas;
    juego_save.score = estado_j.score;
    juego_save.tiempo_caida = estado_j.tiempo_caida;
    juego_save.modo = global_obtener_config_ptr()->modo_juego;

    FILE *file = fopen("save.dat", "wb");

    if (file == NULL) return;

    fwrite(&juego_save, sizeof(juego_savefile_t), 1, file);

    for (int y = 0; y < estado_j.mapa.alto; y++)
        fwrite(estado_j.mapa.tablero[y], sizeof(unsigned char), estado_j.mapa.ancho, file);

    fclose(file);

}

void juego_ganar_puntos(int puntos)
{

    global_config_t conf = *(global_config_t*)global_obtener_config_ptr();

    float val_inic = (global_dificultad() == DIF_NORMAL) ? 1000.0f : 500.0f; //se gana mas puntos si esta en dificil

    //nota: podria usarse la velocidad inicial para el calculo del bonus (conf.velocidad)
    //pero creo que el segundo estatico (1000.0f) es mejor, no se
    float mult = val_inic / (float)estado_j.tiempo_caida;
    //float mult = ((float)conf.velocidad*(1.0f+global_dificultad()) / (float)estado_j.tiempo_caida;

    estado_j.score += (int)( (float)puntos * mult );

    estado_j.score_texto[0] = '\0';
    sprintf_s(estado_j.score_texto, 16, SCORE_TEXTO_FORMATO, estado_j.score);

}
