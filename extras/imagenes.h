#ifndef IMAGENES_H_INCLUDED
#define IMAGENES_H_INCLUDED

enum {
    IMAGENES_ID_BLOQUES,
    IMAGENES_ID_FUENTE8x8,
    IMAGENES_ID_FUENTE8x16,
    IMAGENES_ID_PALETAS,
    IMAGENES_ID_TECLAS
};

enum {
    IMGTECLAS_DERECHA,
    IMGTECLAS_IZQUIERDA,
    IMGTECLAS_D,
    IMGTECLAS_A,
    IMGTECLAS_ABAJO,
    IMGTECLAS_ESPACIO,
    IMGTECLAS_P,
    IMGTECLAS_CANT
};

unsigned char* Imagenes_ObtenerPixeles(unsigned char imagen_id, unsigned char offset_id);
unsigned char Imagenes_OscurecerPixel (unsigned char color);

#endif // IMAGENES_H_INCLUDED
