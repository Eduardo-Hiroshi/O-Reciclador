#include <stdio.h>
#include <stdbool.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>

#define LARGURA 800
#define ALTURA 600
#define FPS 60.0

int main(void)
{
    /* 1. Inicializacao da biblioteca e dos modulos */
    if (!al_init()) {
        fprintf(stderr, "Falha ao iniciar o Allegro.\n");
        return 1;
    }
    if (!al_install_keyboard() || !al_init_primitives_addon() || !al_init_font_addon()) {
        fprintf(stderr, "Falha ao iniciar teclado ou add-ons.\n");
        return 1;
    }

    /* 2. Criacao dos recursos */
    ALLEGRO_DISPLAY* janela = al_create_display(LARGURA, ALTURA);
    ALLEGRO_TIMER* timer = al_create_timer(1.0 / FPS);
    ALLEGRO_EVENT_QUEUE* fila = al_create_event_queue();
    ALLEGRO_FONT* fonte = al_create_builtin_font();
    if (!janela || !timer || !fila || !fonte) {
        fprintf(stderr, "Falha ao criar janela, timer, fila ou fonte.\n");
        return 1;
    }
    al_set_window_title(janela, "Meu primeiro jogo em Allegro");

    /* 3. A fila recebe eventos da janela, do timer e do teclado */
    al_register_event_source(fila, al_get_display_event_source(janela));
    al_register_event_source(fila, al_get_timer_event_source(timer));
    al_register_event_source(fila, al_get_keyboard_event_source());

    /* 4. Estado do jogo */
    const float TAM = 40.0f, VEL = 4.0f;
    float pisoY = 500.0f;
    float x = LARGURA / 2.0f;
    float y = pisoY - TAM / 2;
    bool teclas[ALLEGRO_KEY_MAX] = { false };
    bool rodando = true, redesenhar = true;
    float blocoX = 500, blocoY = 400;
    float blocoLargura = 100, blocoAltura = 100;
    bool colidindo = false;

    /* 5. Laco principal */
    al_start_timer(timer);
    while (rodando) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(fila, &ev);

        switch (ev.type) {
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            rodando = false;
            break;
        case ALLEGRO_EVENT_KEY_DOWN:
            teclas[ev.keyboard.keycode] = true;
            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                rodando = false;
            break;
        case ALLEGRO_EVENT_KEY_UP:
            teclas[ev.keyboard.keycode] = false;
            break;
        case ALLEGRO_EVENT_TIMER: {
            /* Guarda a posicao antes de andar */
            float xAnterior = x;

            /* Movimento para os lados */
            if (teclas[ALLEGRO_KEY_LEFT]) x -= VEL;
            if (teclas[ALLEGRO_KEY_RIGHT]) x += VEL;

            /* Mantem o personagem sobre o piso */
            y = pisoY - TAM / 2;

            /* Impede que saia pelas laterais da janela */
            if (x < TAM / 2)
                x = TAM / 2;

            if (x > LARGURA - TAM / 2)
                x = LARGURA - TAM / 2;

            /* Verifica se encostou no bloco azul */
            if (x + TAM / 2 >= blocoX &&
                x - TAM / 2 <= blocoX + blocoLargura &&
                y + TAM / 2 >= blocoY &&
                y - TAM / 2 <= blocoY + blocoAltura) {

                colidindo = true;
                x = xAnterior; /* Volta para a posicao antes da colisao */

            }
            else {
                colidindo = false;
            }

            redesenhar = true;
            break;
        }
        }

        /* 6. Desenho: so quando a fila esvazia, para nao acumular atraso */
        if (redesenhar && al_is_event_queue_empty(fila)) {
            redesenhar = false;
            /* Fundo azul-claro */
            al_clear_to_color(al_map_rgb(220, 235, 245));

            /* Piso: ocupa toda a largura, da altura 500 ate o fim da tela */
            al_draw_filled_rectangle(
                0, pisoY,
                LARGURA, ALTURA,
                al_map_rgb(110, 150, 90)
            );
            /* Desenha o bloco fixo em azul */
            al_draw_filled_rectangle(
                blocoX, blocoY,
                blocoX + blocoLargura, blocoY + blocoAltura,
                al_map_rgb(0, 100, 255)
            );

            /* Escolhe a cor do personagem */
            ALLEGRO_COLOR cor;

            if (colidindo) {
                cor = al_map_rgb(255, 0, 0); /* verde */
            }
            else {
                cor = al_map_rgb(155, 210, 190); /* azul marinho */
            }

            /* Desenha o personagem */
            al_draw_filled_rectangle(
                x - TAM / 2, y - TAM / 2,
                x + TAM / 2, y + TAM / 2,
                cor
            );
            al_draw_text(fonte, al_map_rgb(30, 30, 30), 10, 10, 0,
                "seta esquerda e direita para andar. ESC para sair.");
            al_flip_display();
        }
    }

    /* 7. Liberacao dos recursos, na ordem inversa da criacao */
    al_destroy_font(fonte);
    al_destroy_event_queue(fila);
    al_destroy_timer(timer);
    al_destroy_display(janela);
    return 0;
}
