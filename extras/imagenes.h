#ifndef IMAGENES_H_INCLUDED
#define IMAGENES_H_INCLUDED

enum {
    IMAGENES_ID_BLOQUES,
    IMAGENES_ID_FUENTE8x8,
    IMAGENES_ID_FUENTE8x16,
    IMAGENES_ID_PALETAS
};

unsigned char* Imagenes_ObtenerPixeles(unsigned char imagen_id, unsigned char offset_id);
unsigned char Imagenes_OscurecerPixel (unsigned char color);

#endif // IMAGENES_H_INCLUDED
