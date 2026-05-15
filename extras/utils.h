#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

//archivo para describir utilidades generales

#define VENTANA_ANCHO 320
#define VENTANA_ALTO 200

void utils_dibujar_imagen(int posx,int posy, unsigned w,unsigned h, unsigned char (*pixeles)[w]);
void utils_dibujar_imagen_sombra(int posx,int posy, unsigned w,unsigned h, unsigned char (*pixeles)[w]);

enum {
    CHAR_REGION_INVALIDA = 0,
    CHAR_REGION_NUM = 1,
    CHAR_REGION_MAY = 2,
    CHAR_REGION_MIN = 4
};
void utils_dibujar_char(int px,int py, unsigned h, unsigned char *cdata, unsigned char color);
void utils_dibujar_texto(int px,int py, const char *texto, unsigned char color);
int utils_ancho_de_texto(const char *texto);

void utils_dibujar_cuadradolineas(int px,int py, int w,int h, unsigned char c);
void utils_set_pixel_mascara(unsigned char pixel);

#endif // UTILS_H_INCLUDED
