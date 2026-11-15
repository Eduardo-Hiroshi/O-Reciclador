#ifndef CENARIOS_H
#define CENARIOS_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

/* Um cenario para cada cor da coleta seletiva */
#define CENARIO_PAPEL    0   /* azul     */
#define CENARIO_PLASTICO 1   /* vermelho */
#define CENARIO_VIDRO    2   /* verde    */
#define CENARIO_METAL    3   /* amarelo  */
#define TOTAL_CENARIOS   4

/* Desenha o fundo, os enfeites e o piso do cenario escolhido.
   Chame depois de al_clear_to_color e antes de desenhar o jogo. */
void cenarios(int tipo, float pisoY);

#endif
