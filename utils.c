#include "utils.h"

#include "GBT/gbt.h"

//las imagenes que se dibujan no van a admitir estos colores
//es para transparencia
unsigned char utils_pixel_mascara = 1;

void utils_dibujar_imagen(int posx,int posy, unsigned w,unsigned h, unsigned char (*pixeles)[w])
{
    unsigned char p;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            p = pixeles[y][x];
            if (p != utils_pixel_mascara)
                gbt_dibujar_pixel(posx+x,posy+y, p);
        }
    }
}

//describir que pixeles se usan para transparencia
//al dibujar una imagen
void utils_set_pixel_mascara(unsigned char pixel)
{
    utils_pixel_mascara = pixel;
}
