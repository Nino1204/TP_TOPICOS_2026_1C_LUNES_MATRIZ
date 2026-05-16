#ifndef IMAGENES_H_INCLUDED
#define IMAGENES_H_INCLUDED

enum {
    IMAGENES_ID_BLOQUES,
    IMAGENES_ID_FUENTE
};

unsigned char* Imagenes_ObtenerPixeles(unsigned char imagen_id, unsigned char offset_id);
unsigned char Imagenes_OscurecerPixel (unsigned char color);

#endif // IMAGENES_H_INCLUDED
