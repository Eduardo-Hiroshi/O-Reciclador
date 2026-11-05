
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#include "menu.h"

#define LARGURA 800
#define ALTURA 600
#define FPS 60.0

int main(void)
{
    /* 1. Inicializacao */

    if (!al_init()) {
        fprintf(stderr, "Falha ao iniciar Allegro.\n");
        return 1;
    }

    if (!al_install_keyboard() ||
        !al_init_primitives_addon() ||
        !al_init_font_addon()) {
        fprintf(stderr, "Falha ao iniciar modulos.\n");
        return 1;
    }

    /* 2. Criacao dos recursos */

    ALLEGRO_DISPLAY* janela = al_create_display(LARGURA, ALTURA);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_FONT* fonte = al_create_builtin_font();

    if (!janela || !timer || !fila || !fonte) {
        fprintf(stderr, "Falha ao criar recursos.\n");
        return 1;
    }

    al_set_window_title(janela, "O Reciclador");

    al_register_event_source(fila, al_get_display_event_source(janela));
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_register_event_source(fila, al_get_keyboard_event_source());

    srand((unsigned int)time(NULL));

    /* 3. Estados do jogo */

    int estadoJogo = MENU;
    int opcaoMenu = 0;

    bool teclas[ALLEGRO_KEY_MAX] = { false };
    bool rodando = true;
    bool redesenhar = true;
    bool solicitouPulo = false;
    bool reiniciar = false;

    /* 4. Personagem */

    const float TAM = 40.0f;
    const float VEL = 4.0f;

    float pisoY = 500.0f;

    float x = LARGURA / 2.0f;
    float y = pisoY - TAM / 2;

    float velocidadeY = 0;
    float gravidade = 0.5f;
    float forcaPulo = -11.0f;

    bool noChao = true;

    /* 5. Plataforma azul */

    float blocoX = 500;
    float blocoY = 400;
    float blocoLargura = 100;
    float blocoAltura = 100;

    /* 6. Lixo caindo */

    float lixoX = 400;
    float lixoY = -40;

    float lixoLargura = 40;
    float lixoAltura = 40;
    float velocidadeLixo = 3;

    int pontuacao = 0;

    /* 7. Vidas */

    int vidas = 3;
    int tempoProtecao = 0;

    /* 8. Inimigo */

    float inimigoX = 100;
    float inimigoY = pisoY - 40;
    float inimigoTAM = 40;

    float velocidadeInimigo = 2;
    int direcaoInimigo = 1;

    /* 9. Loop principal */

    al_start_timer(timer);

    while (rodando) {

        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        switch (ev.type) {

        case ALLEGRO_EVENT_DISPLAY_CLOSE:

            rodando = false;
            break;

            /* TECLAS PRESSIONADAS */

        case ALLEGRO_EVENT_KEY_DOWN:

            teclas[ev.keyboard.keycode] = true;

            /* MENU */

            if (estadoJogo == MENU) {

                if (ev.keyboard.keycode == ALLEGRO_KEY_UP) {
                    opcaoMenu--;

                    if (opcaoMenu < 0)
                        opcaoMenu = 2;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    opcaoMenu++;

                    if (opcaoMenu > 2)
                        opcaoMenu = 0;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {

                    if (opcaoMenu == 0) {
                        estadoJogo = JOGANDO;
                    }

                    else if (opcaoMenu == 1) {
                        estadoJogo = CONFIGURACOES;
                    }

                    else if (opcaoMenu == 2) {
                        rodando = false;
                    }
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    rodando = false;
            }

            /* CONFIGURACOES */

            else if (estadoJogo == CONFIGURACOES) {

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE ||
                    ev.keyboard.keycode == ALLEGRO_KEY_ENTER) {

                    estadoJogo = MENU;
                }
            }

            /* JOGO */

            else if (estadoJogo == JOGANDO) {

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    estadoJogo = MENU;
                    solicitouPulo = false;
                }

                if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE ||
                    ev.keyboard.keycode == ALLEGRO_KEY_UP) {

                    solicitouPulo = true;
                }
            }

            /* GAME OVER */

            else if (estadoJogo == GAME_OVER) {

                if (ev.keyboard.keycode == ALLEGRO_KEY_R)
                    reiniciar = true;

                if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                    estadoJogo = MENU;
            }

            redesenhar = true;
            break;

            /* TECLAS SOLTAS */

        case ALLEGRO_EVENT_KEY_UP:

            teclas[ev.keyboard.keycode] = false;
            break;

            /* ATUALIZACAO DO JOGO */

        case ALLEGRO_EVENT_TIMER: {

            if (estadoJogo == JOGANDO || estadoJogo == GAME_OVER) {

                /* Reiniciar partida */

                if (reiniciar) {

                    x = LARGURA / 2.0f;
                    y = pisoY - TAM / 2;

                    velocidadeY = 0;
                    noChao = true;
                    solicitouPulo = false;

                    vidas = 3;
                    pontuacao = 0;

                    tempoProtecao = 0;

                    lixoX = 400;
                    lixoY = -40;

                    inimigoX = 100;
                    direcaoInimigo = 1;

                    for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
                        teclas[i] = false;

                    estadoJogo = JOGANDO;
                    reiniciar = false;
                }

                if (estadoJogo == JOGANDO) {

                    float xAnterior = x;
                    float yAnterior = y;

                    /* Movimento horizontal */

                    if (teclas[ALLEGRO_KEY_LEFT])
                        x -= VEL;

                    if (teclas[ALLEGRO_KEY_RIGHT])
                        x += VEL;

                    /* Limites da tela */

                    if (x < TAM / 2)
                        x = TAM / 2;

                    if (x > LARGURA - TAM / 2)
                        x = LARGURA - TAM / 2;

                    /* Pulo */

                    if (solicitouPulo && noChao) {
                        velocidadeY = forcaPulo;
                        noChao = false;
                    }

                    solicitouPulo = false;

                    /* Colisao lateral com a plataforma */

                    if (x + TAM / 2 > blocoX &&
                        x - TAM / 2 < blocoX + blocoLargura &&
                        y + TAM / 2 > blocoY &&
                        y - TAM / 2 < blocoY + blocoAltura) {

                        x = xAnterior;
                    }

                    /* Gravidade */

                    y += velocidadeY;
                    velocidadeY += gravidade;

                    noChao = false;

                    /* Pousar no chao */

                    if (y + TAM / 2 >= pisoY) {

                        y = pisoY - TAM / 2;
                        velocidadeY = 0;
                        noChao = true;
                    }

                    /* Pousar em cima da plataforma */

                    if (velocidadeY >= 0 &&
                        yAnterior + TAM / 2 <= blocoY &&
                        y + TAM / 2 >= blocoY &&
                        x + TAM / 2 > blocoX &&
                        x - TAM / 2 < blocoX + blocoLargura) {

                        y = blocoY - TAM / 2;
                        velocidadeY = 0;
                        noChao = true;
                    }

                    /* Bater na parte de baixo da plataforma */

                    if (velocidadeY < 0 &&
                        yAnterior - TAM / 2 >= blocoY + blocoAltura &&
                        y - TAM / 2 <= blocoY + blocoAltura &&
                        x + TAM / 2 > blocoX &&
                        x - TAM / 2 < blocoX + blocoLargura) {

                        y = blocoY + blocoAltura + TAM / 2;
                        velocidadeY = 0;
                    }

                    /* Atualizacao do lixo */

                    lixoY += velocidadeLixo;

                    if (lixoY > ALTURA) {

                        lixoY = -lixoAltura;
                        lixoX = 50 + rand() % (LARGURA - 100);
                    }

                    /* Coletar lixo */

                    if (x - TAM / 2 < lixoX + lixoLargura &&
                        x + TAM / 2 > lixoX &&
                        y - TAM / 2 < lixoY + lixoAltura &&
                        y + TAM / 2 > lixoY) {

                        pontuacao++;

                        lixoY = -lixoAltura;
                        lixoX = 50 + rand() % (LARGURA - 100);
                    }

                    /* Movimento do inimigo */

                    inimigoX += velocidadeInimigo * direcaoInimigo;

                    if (inimigoX <= 0) {

                        inimigoX = 0;
                        direcaoInimigo = 1;
                    }

                    if (inimigoX + inimigoTAM >= LARGURA) {

                        inimigoX = LARGURA - inimigoTAM;
                        direcaoInimigo = -1;
                    }

                    /* Colisao com o inimigo */

                    if (tempoProtecao > 0)
                        tempoProtecao--;

                    if (tempoProtecao == 0 &&
                        x + TAM / 2 > inimigoX &&
                        x - TAM / 2 < inimigoX + inimigoTAM &&
                        y + TAM / 2 > inimigoY &&
                        y - TAM / 2 < inimigoY + inimigoTAM) {

                        vidas--;
                        tempoProtecao = 60;
                    }

                    /* Game Over */

                    if (vidas <= 0) {

                        vidas = 0;
                        estadoJogo = GAME_OVER;
                    }
                }
            }

            redesenhar = true;
            break;
        }
        }

        /* 10. DESENHO DAS TELAS */

        if (redesenhar && al_is_event_queue_empty(fila)) {

            redesenhar = false;

            al_clear_to_color(al_map_rgb(220, 235, 245));

            /* TELA DO MENU */

            if (estadoJogo == MENU) {

                desenharMenu(fonte, opcaoMenu);
            }

            /* TELA DE CONFIGURACOES */

            else if (estadoJogo == CONFIGURACOES) {

                desenharConfiguracoes(fonte);
            }

            /* TELA DO JOGO */

            else if (estadoJogo == JOGANDO ||
                estadoJogo == GAME_OVER) {

                /* Piso */

                al_draw_filled_rectangle(
                    0, pisoY,
                    LARGURA, ALTURA,
                    al_map_rgb(110, 150, 90)
                );

                /* Lixo */

                al_draw_filled_rectangle(
                    lixoX, lixoY,
                    lixoX + lixoLargura,
                    lixoY + lixoAltura,
                    al_map_rgb(150, 75, 0)
                );

                /* Plataforma azul */

                al_draw_filled_rectangle(
                    blocoX, blocoY,
                    blocoX + blocoLargura,
                    blocoY + blocoAltura,
                    al_map_rgb(0, 100, 255)
                );

                /* Inimigo */

                al_draw_filled_rectangle(
                    inimigoX, inimigoY,
                    inimigoX + inimigoTAM,
                    inimigoY + inimigoTAM,
                    al_map_rgb(200, 40, 40)
                );

                /* Personagem */

                ALLEGRO_COLOR corPersonagem =
                    al_map_rgb(155, 210, 190);

                if (tempoProtecao > 0 &&
                    (tempoProtecao / 5) % 2 == 0) {

                    corPersonagem = al_map_rgb(255, 255, 255);
                }

                al_draw_filled_rectangle(
                    x - TAM / 2,
                    y - TAM / 2,
                    x + TAM / 2,
                    y + TAM / 2,
                    corPersonagem
                );

                /* Texto de movimentacao */

                al_draw_text(
                    fonte,
                    al_map_rgb(30, 30, 30),
                    10, 10, 0,
                    "Setas: andar | ESPACO: pular | ESC: menu"
                );

                /* Pontuacao */

                char texto[50];

                sprintf_s(
                    texto,
                    sizeof(texto),
                    "Pontuacao: %d",
                    pontuacao
                );

                al_draw_text(
                    fonte,
                    al_map_rgb(0, 0, 0),
                    10, 35, 0,
                    texto
                );

                /* Tres coracoes */

                for (int i = 0; i < vidas; i++) {

                    float coracaoX = 680 + i * 35;
                    float coracaoY = 20;

                    al_draw_filled_circle(
                        coracaoX + 7,
                        coracaoY + 7,
                        7,
                        al_map_rgb(230, 0, 0)
                    );

                    al_draw_filled_circle(
                        coracaoX + 17,
                        coracaoY + 7,
                        7,
                        al_map_rgb(230, 0, 0)
                    );

                    al_draw_filled_triangle(
                        coracaoX,
                        coracaoY + 9,
                        coracaoX + 24,
                        coracaoY + 9,
                        coracaoX + 12,
                        coracaoY + 25,
                        al_map_rgb(230, 0, 0)
                    );
                }

                /* GAME OVER */

                if (estadoJogo == GAME_OVER) {

                    al_draw_filled_rectangle(
                        200, 220,
                        600, 370,
                        al_map_rgb(30, 30, 30)
                    );

                    al_draw_text(
                        fonte,
                        al_map_rgb(255, 50, 50),
                        400, 250,
                        ALLEGRO_ALIGN_CENTER,
                        "GAME OVER"
                    );

                    al_draw_text(
                        fonte,
                        al_map_rgb(255, 255, 255),
                        400, 280,
                        ALLEGRO_ALIGN_CENTER,
                        "Voce perdeu todas as vidas!"
                    );

                    al_draw_text(
                        fonte,
                        al_map_rgb(255, 255, 0),
                        400, 320,
                        ALLEGRO_ALIGN_CENTER,
                        "Pressione R para jogar novamente"
                    );

                    al_draw_text(
                        fonte,
                        al_map_rgb(255, 255, 255),
                        400, 345,
                        ALLEGRO_ALIGN_CENTER,
                        "ESC para voltar ao menu"
                    );
                }
            }

            al_flip_display();
        }
    }

    /* 11. Liberacao dos recursos */

    al_destroy_font(fonte);
    al_destroy_event_queue(fila);
    al_destroy_timer(timer);
    al_destroy_display(janela);

    return 0;
}
