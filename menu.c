
#include "menu.h"

void desenharMenu(ALLEGRO_FONT* fonte, int opcaoMenu)
{
    al_draw_text(
        fonte,
        al_map_rgb(30, 120, 50),
        400, 100,
        ALLEGRO_ALIGN_CENTER,
        "O RECICLADOR"
    );

    al_draw_text(
        fonte,
        al_map_rgb(0, 0, 0),
        400, 160,
        ALLEGRO_ALIGN_CENTER,
        "Ajude a salvar o planeta!"
    );

    const char* opcoes[] = {
        "Jogar",
        "Configuracoes",
        "Sair"
    };

    for (int i = 0; i < 3; i++)
    {
        ALLEGRO_COLOR cor;

        if (i == opcaoMenu)
            cor = al_map_rgb(30, 160, 50);
        else
            cor = al_map_rgb(0, 0, 0);

        char texto[100];

        if (i == opcaoMenu)
            sprintf_s(texto, sizeof(texto), "> %s", opcoes[i]);
        else
            sprintf_s(texto, sizeof(texto), "  %s", opcoes[i]);

        al_draw_text(
            fonte,
            cor,
            400, 260 + (i * 60),
            ALLEGRO_ALIGN_CENTER,
            texto
        );
    }
}

void desenharConfiguracoes(ALLEGRO_FONT* fonte)
{
    al_draw_text(
        fonte,
        al_map_rgb(30, 120, 50),
        400, 120,
        ALLEGRO_ALIGN_CENTER,
        "CONFIGURACOES"
    );

    al_draw_text(
        fonte,
        al_map_rgb(0, 0, 0),
        400, 230,
        ALLEGRO_ALIGN_CENTER,
        "Controles:"
    );

    al_draw_text(
        fonte,
        al_map_rgb(0, 0, 0),
        400, 280,
        ALLEGRO_ALIGN_CENTER,
        "Setas: movimentar"
    );

    al_draw_text(
        fonte,
        al_map_rgb(0, 0, 0),
        400, 320,
        ALLEGRO_ALIGN_CENTER,
        "Espaco: pular"
    );

    al_draw_text(
        fonte,
        al_map_rgb(0, 0, 0),
        400, 430,
        ALLEGRO_ALIGN_CENTER,
        "Pressione ESC para voltar"
    );
}
