#include "utils.h"

#include <ctype.h>
#include <string.h>
#include "GBT/gbt.h"

#include "imagenes.h"

//las imagenes que se dibujan no van a admitir estos colores
//es para transparencia
unsigned char utils_pixel_mascara = 1;

//------IMAGENES------
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
void utils_dibujar_imagen_sombra(int posx,int posy, unsigned w,unsigned h, unsigned char (*pixeles)[w])
{
    unsigned char p;

    for (int y = 0; y < h; y++)
    {
        for (int x = 0; x < w; x++)
        {
            p = pixeles[y][x];
            if (p != utils_pixel_mascara)
                gbt_dibujar_pixel(posx+x,posy+y, 9);
        }
    }
}

//-------TEXTO-------
void utils_dibujar_char(int px,int py, unsigned char *cdata, unsigned char color)
{
    for (int y = 0; y < 8; y++)
    {
        for (int x = 0; x < 8; x++)
        {
            unsigned char p =  cdata[y] & (1<<x);
            if (p != 0)
                gbt_dibujar_pixel(px+x,py+y, color);
        }
    }
}
void utils_dibujar_texto(int px,int py, const char *texto, unsigned char color)
{

    int tlen = strlen(texto);
    unsigned char cactual;
    for (int i = 0; i < tlen; i++)
    {
        cactual = toupper(texto[i]);
        if (cactual == ' ') continue; //saltar en espacio
        utils_dibujar_char(
            px+i*6,py,
            Imagenes_ObtenerPixeles(IMAGENES_ID_FUENTE, cactual-'0'),
            color
        );
    }

}

//dibuja las lineas de un cuadrado
void utils_dibujar_cuadradolineas(int px,int py, int w,int h, unsigned char c)
{
    for (int x = 0; x < w; x++)
    {
        if ((x % 4) == 0) continue;
        gbt_dibujar_pixel(px+x,py,c);
        gbt_dibujar_pixel(px+x,py+h,c);
    }
    for (int y = 0; y < h; y++)
    {
        if ((y % 4) == 0) continue;
        gbt_dibujar_pixel(px,py+y,c);
        gbt_dibujar_pixel(px+w,py+y,c);
    }

}
void utils_dibujar_lineah(int py, int xdesde, int xhasta, unsigned char c)
{
    for (int x = xdesde; x < xhasta; x++)
    {
        if ((x%4) == 0) continue;
        gbt_dibujar_pixel(x,py, c);
    }
}

//describir que pixeles se usan para transparencia
//al dibujar una imagen
void utils_set_pixel_mascara(unsigned char pixel)
{
    utils_pixel_mascara = pixel;
}
