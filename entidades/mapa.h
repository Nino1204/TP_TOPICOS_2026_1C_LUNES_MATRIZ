#ifndef MAPA_H_INCLUDED
#define MAPA_H_INCLUDED

#define MAPA_BLOQUE_TAM 8
#define MAPA_POS_VACIA 0xFF

typedef struct {

    unsigned char *tablero;
    unsigned ancho, alto;

} mapa_t;

mapa_t mapa_crear(unsigned _ancho, unsigned _alto);
void mapa_poner_bloque(mapa_t mapa, unsigned posx,unsigned posy, unsigned char bloque_id);
unsigned char mapa_obtener_bloque(mapa_t mapa, unsigned posx,unsigned posy);
int mapa_es_valido(mapa_t mapa);
int mapa_posicion_es_vacia(mapa_t mapa, unsigned posx,unsigned posy);
void mapa_dibujar(mapa_t mapa, int ox,int oy); //dibujar el mapa con un origen x e y
void mapa_dibujar_sombra(mapa_t mapa, int ox,int oy); //dibujar el mapa en negro con un origen x e y
int mapa_linea_llena(mapa_t mapa, unsigned linea);
void mapa_revisar_lineas(mapa_t mapa); //revisar si se completo una linea y bajar el resto
void mapa_destruir(mapa_t mapa);

#endif // MAPA_H_INCLUDED
