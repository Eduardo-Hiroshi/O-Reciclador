#include "cenarios.h"

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#define LARGURA 800
#define ALTURA 600

/* Uma variavel para cada imagem */
ALLEGRO_BITMAP* imgPapel = NULL;
ALLEGRO_BITMAP* imgPlastico = NULL;
ALLEGRO_BITMAP* imgVidro = NULL;
ALLEGRO_BITMAP* imgMetal = NULL;

bool cenariosCarregar(void)
{
    if (!al_init_image_addon()) {
        fprintf(stderr, "Falha ao iniciar o addon de imagem.\n");
        return false;
    }

    imgPapel = al_load_bitmap("imagens/papel.png");
    imgPlastico = al_load_bitmap("imagens/plastico.png");
    imgVidro = al_load_bitmap("imagens/vidro.png");
    imgMetal = al_load_bitmap("imagens/metal.png");

    if (!imgPapel || !imgPlastico || !imgVidro || !imgMetal) {
        fprintf(stderr, "Falha ao carregar as imagens da pasta imagens.\n");
        cenariosLiberar();
        return false;
    }

    return true;
}

void cenarios(int tipo)
{
    /* Escolhe a imagem de acordo com o cenario */

    ALLEGRO_BITMAP* img = imgPapel;

    if (tipo == CENARIO_PLASTICO)
        img = imgPlastico;

    if (tipo == CENARIO_VIDRO)
        img = imgVidro;

    if (tipo == CENARIO_METAL)
        img = imgMetal;

    if (!img)
        return;

    /* Desenha ocupando a tela toda (800x600), mesmo que a imagem tenha outro tamanho */

    al_draw_scaled_bitmap(
        img,
        0, 0,
        al_get_bitmap_width(img),
        al_get_bitmap_height(img),
        0, 0,
        LARGURA, ALTURA,
        0
    );
}

void cenariosLiberar(void)
{
    al_destroy_bitmap(imgPapel);
    al_destroy_bitmap(imgPlastico);
    al_destroy_bitmap(imgVidro);
    al_destroy_bitmap(imgMetal);
}