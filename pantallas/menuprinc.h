#ifndef MENUPRINC_H_INCLUDED
#define MENUPRINC_H_INCLUDED

#define MENU_BOTON_TEXTOMAX 32
typedef struct {

    float x,y;
    unsigned short w,h;
    unsigned short objx, objy;
    char nombre[MENU_BOTON_TEXTOMAX];

} menu_boton_t;

void menuprinc_iniciar();
void menuprinc_actualizar();
void menuprinc_dibujar();
void menuprinc_cerrar();

#endif // MENUPRINC_H_INCLUDED
