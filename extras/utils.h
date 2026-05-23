#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

//archivo para describir utilidades generales

void utils_dibujar_imagen(int posx,int posy, unsigned w,unsigned h, unsigned char (*pixeles)[w]);
void utils_dibujar_imagen_sombra(int posx,int posy, unsigned w,unsigned h, unsigned char (*pixeles)[w]);

void utils_dibujar_texto(int px,int py, const char *texto, unsigned char color);

#define UTILS_TEXTO_SOMBREADO(px,py,t) utils_dibujar_texto(px,py+1,t,14); utils_dibujar_texto(px,py,t,13);

void utils_dibujar_lineah(int py, int xdesde, int xhasta, unsigned char c);

void utils_dibujar_cuadradolineas(int px,int py, int w,int h, unsigned char c);
void utils_set_pixel_mascara(unsigned char pixel);

void utils_aplicar_paleta(unsigned char paleta);

#endif // UTILS_H_INCLUDED
