#ifndef CENARIOS_H
#define CENARIOS_H

#include <stdbool.h>
#include <allegro5/allegro.h>

#define CENARIO_PAPEL    0
#define CENARIO_PLASTICO 1
#define CENARIO_VIDRO    2
#define CENARIO_METAL    3
#define TOTAL_CENARIOS   4

/* Retorna false se alguma imagem nao puder ser carregada. */
bool cenariosCarregar(void);

/* Desenha a imagem do cenario escolhido. */
void cenarios(int tipo);

void cenariosLiberar(void);

#endif