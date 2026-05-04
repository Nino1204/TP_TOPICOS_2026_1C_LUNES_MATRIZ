#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

//archivo para describir utilidades generales

void utils_dibujar_imagen(int posx,int posy, unsigned w,unsigned h, unsigned char (*pixeles)[w]);
void utils_set_pixel_mascara(unsigned char pixel);

#endif // UTILS_H_INCLUDED
