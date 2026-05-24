#include "utils.h"

#include <ctype.h>

#include "GBT/gbt.h"

#include "imagenes.h"

//las imagenes que se dibujan no van a admitir estos colores
//es para transparencia
unsigned char utils_pixel_mascara = 0xFF;
unsigned char utils_paleta_actual = 0;

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
                gbt_dibujar_pixel(posx+x,posy+y, 14);
        }
    }
}

void utils_dibujar_char(int px,int py, unsigned h, unsigned char *cdata, unsigned char color)
{
    for (int y = 0; y < h; y++)
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
    unsigned cursor = 0;

    int es_mayuscula;

    for (int i = 0; i < tlen; i++)
    {
        cactual = texto[i];

        es_mayuscula = cactual >= 'A' && cactual <= 'Z';
        cactual = toupper(cactual);

        if (cactual == ' '){
            cursor += 6;
            continue;
        }

        if (es_mayuscula)
        {
            utils_dibujar_char(
                px+cursor,py, 16,
                Imagenes_ObtenerPixeles(IMAGENES_ID_FUENTE8x16, cactual - 'A'),
                color
            );
            cursor+=10;
            continue;
        }

        utils_dibujar_char(
            px+cursor,py, 8,
            Imagenes_ObtenerPixeles(IMAGENES_ID_FUENTE8x8, cactual - '0'),
            color
        );
        cursor += 6;

    }

}
int utils_ancho_de_texto(const char *texto)
{
    int ancho = 0;
    int len = strlen(texto);
    char c;
    for (int i = 0; i < len; i++)
    {
        c = texto[i];
        ancho += (c >= 'A' && c <= 'Z') ? 10 : 6;
    }
    return ancho;
}
void utils_dibujar_texto_nm(int px,int py, const char *texto, unsigned char color)
{

    unsigned cursor=0;

    unsigned tlen = strlen(texto);
    char car;
    unsigned char *cdata;

    for (int i = 0; i < tlen; i++)
    {
        car = toupper( texto[i] );
        cdata = Imagenes_ObtenerPixeles(IMAGENES_ID_FUENTENMONO, car-'0');
        utils_dibujar_char(
            px+cursor,py, 5, cdata, color
        );
        cursor += Imagenes_NoMonoAncho(car)+1;
    }

}
int utils_ancho_de_texto_nm(const char *texto) //ancho de texto no monoespaciado
{
    unsigned res = 0;

    char car;
    unsigned tlen = strlen(texto);

    for (int i = 0; i < tlen; i++)
    {
        car = toupper(texto[i]);
        res += Imagenes_NoMonoAncho(car)+1;
    }

    return res;
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

void utils_aplicar_paleta(unsigned char paleta)
{
    tGBT_ColorRGB *pcolores = (tGBT_ColorRGB*) Imagenes_ObtenerPixeles(IMAGENES_ID_PALETAS, paleta);
    gbt_aplicar_paleta(pcolores, 18, GBT_FORMATO_888);
    utils_paleta_actual = paleta;
}

//describir que pixeles se usan para transparencia
//al dibujar una imagen
void utils_set_pixel_mascara(unsigned char pixel)
{
    utils_pixel_mascara = pixel;
}
