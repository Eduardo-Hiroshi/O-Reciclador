
#ifndef MENU_H
#define MENU_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>

#define MENU 0
#define JOGANDO 1
#define CONFIGURACOES 2
#define GAME_OVER 3

void desenharMenu(ALLEGRO_FONT* fonte, int opcaoMenu);

void desenharConfiguracoes(ALLEGRO_FONT* fonte);

#endif
