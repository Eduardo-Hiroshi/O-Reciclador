#include "cenarios.h"
#include <math.h>

#define CEN_LARGURA 800
#define PI_F 3.14159265f

/*
 * ESCALA DE REFERENCIA (em pixels)
 *   lixeira ........ ~87      cadeira plastica .. ~80
 *   estante ........ ~150     arvore (e = 2) .... ~250
 *   caixa de papelao ~44      garrafa ........... ~36
 *   lata ........... ~24      copo / pote ....... ~20 a 30
 * Todos os objetos seguem essa mesma regua, entao uma garrafa
 * fica sempre bem menor que uma arvore, uma cadeira ou uma lixeira.
 */

 /* ------------------------------------------------------------------ */
 /* Funcoes auxiliares (visiveis so neste arquivo)                      */
 /* ------------------------------------------------------------------ */

static int misturar(int a, int b, float t)
{
    return (int)(a + (b - a) * t);
}

/* Ceu em degrade vertical */
static void ceuDegrade(float pisoY,
    int r1, int g1, int b1,
    int r2, int g2, int b2)
{
    for (int y = 0; y < (int)pisoY; y += 5) {
        float t = y / pisoY;

        al_draw_filled_rectangle(
            0, (float)y, CEN_LARGURA, (float)y + 5,
            al_map_rgb(misturar(r1, r2, t),
                misturar(g1, g2, t),
                misturar(b1, b2, t))
        );
    }
}

/* Silhueta da nuvem: cinco "bolas" apoiadas numa base reta */
static void nuvemFormas(float x, float y, float e, ALLEGRO_COLOR cor)
{
    float base = y + 18 * e;

    al_draw_filled_circle(x, base - 16 * e, 16 * e, cor);
    al_draw_filled_circle(x + 26 * e, base - 24 * e, 24 * e, cor);
    al_draw_filled_circle(x + 54 * e, base - 30 * e, 30 * e, cor);
    al_draw_filled_circle(x + 84 * e, base - 22 * e, 22 * e, cor);
    al_draw_filled_circle(x + 106 * e, base - 16 * e, 16 * e, cor);
    al_draw_filled_rectangle(x, base - 16 * e, x + 106 * e, base, cor);
}

/* Nuvem com a barriga mais escura (use cores opacas) */
static void nuvemSombra(float x, float y, float e,
    ALLEGRO_COLOR cor, ALLEGRO_COLOR cs)
{
    nuvemFormas(x, y + 5 * e, e, cs);
    nuvemFormas(x, y, e, cor);
}

/* Nuvem com sombra azulada calculada a partir da cor */
static void nuvem(float x, float y, float e, ALLEGRO_COLOR cor)
{
    nuvemSombra(x, y, e, cor,
        al_map_rgb_f(cor.r * 0.86f, cor.g * 0.90f, cor.b * 0.97f));
}

/* ------------------------------------------------------------------ */
/* Pisos: degrade + padrao proprio de cada cenario                     */
/* ------------------------------------------------------------------ */

/* Degrade vertical do chao, de pisoY ate o fim da tela */
static void pisoDegrade(float pisoY, int r1, int g1, int b1,
    int r2, int g2, int b2)
{
    for (int y = (int)pisoY; y < 600; y += 4) {
        float t = (y - pisoY) / (600.0f - pisoY);

        al_draw_filled_rectangle(0, (float)y, CEN_LARGURA, (float)y + 4,
            al_map_rgb(misturar(r1, r2, t),
                misturar(g1, g2, t),
                misturar(b1, b2, t)));
    }
}

/* Altura da linha i (0 a 4) do chao: o espacamento cresce para a frente */
static float linhaPiso(float pisoY, int i)
{
    return pisoY + 8 + (600.0f - pisoY - 8) * powf(i / 4.0f, 1.6f);
}

/* Linhas horizontais do chao (juntas) */
static void pisoLinhas(float pisoY, ALLEGRO_COLOR cor)
{
    for (int i = 1; i < 4; i++) {
        float y = linhaPiso(pisoY, i);

        al_draw_line(0, y, CEN_LARGURA, y, cor, 1.5f);
    }
}

/* Linhas que convergem para um ponto de fuga (efeito de perspectiva) */
static void pisoConvergente(float pisoY, float alturaFuga, float passo,
    ALLEGRO_COLOR cor)
{
    float vy = pisoY - alturaFuga;
    float yTopo = pisoY + 8;

    for (int k = -8; k <= 8; k++) {
        float xb = 400.0f + k * passo;
        float xt = 400.0f + (xb - 400.0f) * (yTopo - vy) / (600.0f - vy);

        al_draw_line(xt, yTopo, xb, 600, cor, 1);
    }
}

/* Sombra suave sob um objeto (cor opaca, um pouco mais escura que o chao) */
static void sombraChao(float cx, float y, float rx, ALLEGRO_COLOR cor)
{
    al_draw_filled_ellipse(cx, y, rx, 4, cor);
}

/* Papel: tabuas de madeira azulada */
static void pisoPapel(float pisoY)
{
    ALLEGRO_COLOR linha = al_map_rgb(40, 75, 140);

    pisoDegrade(pisoY, 72, 114, 184, 50, 85, 155);
    al_draw_filled_rectangle(0, pisoY, CEN_LARGURA, pisoY + 8,
        al_map_rgb(100, 140, 210));

    for (int i = 0; i < 4; i++) {
        float ya = linhaPiso(pisoY, i);
        float yb = linhaPiso(pisoY, i + 1);
        float esp = 110.0f + i * 35.0f;
        float desl = (i % 2) * esp / 2.0f;

        al_draw_line(0, yb, CEN_LARGURA, yb, linha, 1.5f);

        for (float x = desl; x < CEN_LARGURA; x += esp)
            al_draw_line(x, ya, x, yb, linha, 1.5f);
    }
}

/* Plastico: calcada de lajotas em perspectiva */
static void pisoPlastico(float pisoY)
{
    ALLEGRO_COLOR junta = al_map_rgb(96, 27, 34);

    pisoDegrade(pisoY, 140, 44, 50, 108, 31, 39);
    pisoConvergente(pisoY, 120, 150, junta);
    pisoLinhas(pisoY, junta);
    al_draw_filled_rectangle(0, pisoY, CEN_LARGURA, pisoY + 8,
        al_map_rgb(185, 70, 70));
}

/* Vidro: gramado aparado em faixas, com tufos e florzinhas */
static void pisoVidro(float pisoY)
{
    static const float tufos[10][2] = {
        { 40, 40 }, { 130, 62 }, { 210, 30 }, { 300, 78 }, { 360, 30 },
        { 540, 36 }, { 610, 70 }, { 700, 44 }, { 770, 80 }, { 440, 78 }
    };
    static const float flores[4][2] = {
        { 90, 70 }, { 250, 52 }, { 570, 64 }, { 660, 34 }
    };
    static const int coresFlor[4][3] = {
        { 255, 255, 255 }, { 250, 215, 70 }, { 235, 110, 160 }, { 190, 140, 235 }
    };
    ALLEGRO_COLOR tufoCor = al_map_rgb(30, 100, 52);

    for (int i = 0; i < 4; i++) {
        float t = i / 3.0f;
        ALLEGRO_COLOR c = (i % 2)
            ? al_map_rgb(misturar(66, 48, t), misturar(140, 112, t), misturar(86, 66, t))
            : al_map_rgb(misturar(56, 40, t), misturar(128, 100, t), misturar(76, 58, t));

        al_draw_filled_rectangle(0, linhaPiso(pisoY, i), CEN_LARGURA,
            linhaPiso(pisoY, i + 1), c);
    }
    al_draw_filled_rectangle(0, pisoY, CEN_LARGURA, pisoY + 8,
        al_map_rgb(90, 175, 100));

    for (int i = 0; i < 10; i++) {
        float tx = tufos[i][0];
        float ty = pisoY + tufos[i][1];

        al_draw_line(tx, ty, tx - 4, ty - 7, tufoCor, 1.5f);
        al_draw_line(tx, ty, tx, ty - 9, tufoCor, 1.5f);
        al_draw_line(tx, ty, tx + 4, ty - 7, tufoCor, 1.5f);
    }

    for (int i = 0; i < 4; i++) {
        float fx = flores[i][0];
        float fy = pisoY + flores[i][1];

        al_draw_line(fx, fy, fx, fy - 8, al_map_rgb(30, 100, 52), 1.5f);
        al_draw_filled_circle(fx, fy - 9, 3.5f,
            al_map_rgb(coresFlor[i][0], coresFlor[i][1], coresFlor[i][2]));
        al_draw_filled_circle(fx, fy - 9, 1.2f, al_map_rgb(220, 150, 30));
    }
}

/* Metal: chapas de aco com faixa de seguranca amarela e preta */
static void pisoMetal(float pisoY)
{
    ALLEGRO_COLOR junta = al_map_rgb(108, 86, 22);
    float y0 = pisoY + 10;
    float y1 = pisoY + 16;

    pisoDegrade(pisoY, 172, 142, 46, 128, 102, 30);
    pisoConvergente(pisoY, 220, 170, junta);
    pisoLinhas(pisoY, junta);
    al_draw_filled_rectangle(0, pisoY, CEN_LARGURA, pisoY + 8,
        al_map_rgb(225, 190, 60));

    /* faixa de seguranca */
    al_draw_filled_rectangle(0, y0, CEN_LARGURA, y1, al_map_rgb(240, 200, 40));

    for (float x = 0; x < CEN_LARGURA; x += 24) {
        al_draw_filled_triangle(x, y1, x + 10, y1, x + 16, y0,
            al_map_rgb(40, 40, 40));
        al_draw_filled_triangle(x, y1, x + 16, y0, x + 6, y0,
            al_map_rgb(40, 40, 40));
    }
}

/* Lixeira de coleta seletiva com simbolo de reciclagem */
static void lixeira(float x, float pisoY, ALLEGRO_COLOR cor)
{
    ALLEGRO_COLOR escura = al_map_rgb(40, 40, 40);
    ALLEGRO_COLOR branco = al_map_rgb(255, 255, 255);

    /* corpo */
    al_draw_filled_rectangle(x, pisoY - 70, x + 50, pisoY, cor);
    al_draw_rectangle(x, pisoY - 70, x + 50, pisoY, escura, 2);

    /* tampa e alca */
    al_draw_filled_rectangle(x - 4, pisoY - 80, x + 54, pisoY - 70, cor);
    al_draw_rectangle(x - 4, pisoY - 80, x + 54, pisoY - 70, escura, 2);
    al_draw_filled_rectangle(x + 18, pisoY - 87, x + 32, pisoY - 80, escura);

    /* simbolo de reciclagem simplificado (triangulo + setas) */
    al_draw_triangle(x + 25, pisoY - 58,
        x + 10, pisoY - 28,
        x + 40, pisoY - 28,
        branco, 3);
    al_draw_filled_circle(x + 25, pisoY - 58, 3, branco);
    al_draw_filled_circle(x + 10, pisoY - 28, 3, branco);
    al_draw_filled_circle(x + 40, pisoY - 28, 3, branco);
}

/* Garrafa generica: (x, yBase) e o canto inferior esquerdo */
static void garrafa(float x, float yBase, float w, float h,
    ALLEGRO_COLOR cor, ALLEGRO_COLOR tampa)
{
    float corpoAltura = h * 0.65f;
    float gargaloLargura = w * 0.4f;
    float gargaloX = x + (w - gargaloLargura) / 2;

    /* corpo */
    al_draw_filled_rectangle(x, yBase - corpoAltura, x + w, yBase, cor);

    /* ombro (triangulo ligando corpo e gargalo) */
    al_draw_filled_triangle(
        x, yBase - corpoAltura,
        x + w, yBase - corpoAltura,
        x + w / 2, yBase - corpoAltura - h * 0.12f,
        cor
    );

    /* gargalo */
    al_draw_filled_rectangle(gargaloX, yBase - h * 0.95f,
        gargaloX + gargaloLargura, yBase - corpoAltura, cor);

    /* tampa */
    al_draw_filled_rectangle(gargaloX - 1, yBase - h,
        gargaloX + gargaloLargura + 1, yBase - h * 0.95f, tampa);

    /* brilho */
    al_draw_filled_rectangle(x + w * 0.15f, yBase - corpoAltura + 4,
        x + w * 0.30f, yBase - 4, al_map_rgba(255, 255, 255, 70));
}

static void engrenagem(float x, float y, float r,
    ALLEGRO_COLOR cor, ALLEGRO_COLOR furo)
{
    for (int i = 0; i < 8; i++) {
        float a = i * PI_F / 4.0f;

        al_draw_line(x, y,
            x + cosf(a) * (r + 7),
            y + sinf(a) * (r + 7),
            cor, 8);
    }

    al_draw_filled_circle(x, y, r, cor);
    al_draw_filled_circle(x, y, r / 3, furo);
}

static void lata(float x, float yBase, float w, float h, ALLEGRO_COLOR cor)
{
    al_draw_filled_rectangle(x, yBase - h, x + w, yBase, cor);
    al_draw_rectangle(x, yBase - h, x + w, yBase, al_map_rgb(90, 70, 0), 2);

    /* aros da lata */
    al_draw_line(x, yBase - h + 4, x + w, yBase - h + 4, al_map_rgb(90, 70, 0), 1.5f);
    al_draw_line(x, yBase - 4, x + w, yBase - 4, al_map_rgb(90, 70, 0), 1.5f);

    /* brilho */
    al_draw_filled_rectangle(x + w * 0.2f, yBase - h + 7,
        x + w * 0.34f, yBase - 7, al_map_rgba(255, 255, 255, 90));
}

static void folha(float x, float y)
{
    al_draw_filled_rectangle(x, y, x + 22, y + 30, al_map_rgb(255, 255, 255));
    al_draw_rectangle(x, y, x + 22, y + 30, al_map_rgb(90, 120, 170), 1.5f);

    for (int i = 0; i < 4; i++)
        al_draw_line(x + 4, y + 7 + i * 6, x + 18, y + 7 + i * 6,
            al_map_rgb(150, 170, 210), 1);
}

/* ------------------------------------------------------------------ */
/* OBJETOS DE PAPEL                                                    */
/* ------------------------------------------------------------------ */

/* Estante com livros: (x, yBase) = canto inferior esquerdo */
static void estante(float x, float yBase, float w, float h)
{
    static const int cores[4][3] = {
        { 51, 102, 204 }, { 89, 140, 230 }, { 26, 64, 153 }, { 140, 191, 242 }
    };
    ALLEGRO_COLOR madeira = al_map_rgb(45, 75, 130);
    ALLEGRO_COLOR fundo = al_map_rgb(30, 50, 100);
    ALLEGRO_COLOR borda = al_map_rgb(20, 35, 80);
    int niveis = 3;
    float nivelH = h / niveis;

    al_draw_filled_rectangle(x, yBase - h, x + w, yBase, fundo);

    for (int r = 0; r < niveis; r++) {
        float baseY = yBase - r * nivelH - 5;
        float lx = x + 8;
        int i = 0;

        while (lx + 10 < x + w - 8) {
            float bw = 7.0f + (float)((i * 3 + r) % 4);
            float bh = 24.0f + (float)((i * 5 + r * 3) % 10);
            int c = (i + r) % 4;

            al_draw_filled_rectangle(lx, baseY - bh, lx + bw, baseY,
                al_map_rgb(cores[c][0], cores[c][1], cores[c][2]));
            al_draw_rectangle(lx, baseY - bh, lx + bw, baseY, borda, 1);

            lx += bw + 1;
            i++;
        }

        /* prateleira */
        al_draw_filled_rectangle(x, baseY, x + w, baseY + 5, madeira);
    }

    /* topo e laterais */
    al_draw_filled_rectangle(x, yBase - h - 5, x + w, yBase - h, madeira);
    al_draw_filled_rectangle(x - 5, yBase - h - 5, x, yBase, madeira);
    al_draw_filled_rectangle(x + w, yBase - h - 5, x + w + 5, yBase, madeira);
}

/* Caixa de papelao fechada com fita: (x, yBase) = canto inferior esquerdo */
static void caixaPapelao(float x, float yBase, float w, float h)
{
    ALLEGRO_COLOR marrom = al_map_rgb(196, 150, 95);
    ALLEGRO_COLOR borda = al_map_rgb(120, 85, 45);

    al_draw_filled_rectangle(x, yBase - h, x + w, yBase, marrom);
    al_draw_rectangle(x, yBase - h, x + w, yBase, borda, 2);

    /* abas e fita adesiva */
    al_draw_line(x, yBase - h + 7, x + w, yBase - h + 7, borda, 1.5f);
    al_draw_filled_rectangle(x + w / 2 - 4, yBase - h, x + w / 2 + 4,
        yBase - h + 7, al_map_rgb(230, 205, 150));
}

/* Pilha de jornais amarrada com barbante */
static void pilhaJornal(float x, float yBase)
{
    ALLEGRO_COLOR barbante = al_map_rgb(140, 100, 60);

    for (int i = 0; i < 5; i++) {
        float dx = (i % 2) * 2.0f;
        float y = yBase - (i + 1) * 8;

        al_draw_filled_rectangle(x + dx, y, x + dx + 48, y + 8,
            (i % 2) ? al_map_rgb(208, 208, 198) : al_map_rgb(232, 232, 222));
        al_draw_rectangle(x + dx, y, x + dx + 48, y + 8,
            al_map_rgb(150, 150, 140), 1);
    }

    /* manchete na folha de cima */
    al_draw_filled_rectangle(x + 5, yBase - 36, x + 34, yBase - 33,
        al_map_rgb(50, 50, 50));
    al_draw_line(x + 5, yBase - 31, x + 42, yBase - 31,
        al_map_rgb(120, 120, 120), 1);

    /* barbante */
    al_draw_line(x + 14, yBase - 40, x + 14, yBase, barbante, 2);
    al_draw_line(x + 34, yBase - 40, x + 34, yBase, barbante, 2);
}

/* Sacola de papel kraft com selo de reciclagem */
static void sacolaPapel(float x, float yBase, float w, float h)
{
    ALLEGRO_COLOR kraft = al_map_rgb(205, 160, 105);
    ALLEGRO_COLOR borda = al_map_rgb(125, 90, 50);

    /* alca */
    al_draw_arc(x + w / 2, yBase - h, w * 0.28f, PI_F, PI_F, borda, 3);

    al_draw_filled_rectangle(x, yBase - h, x + w, yBase, kraft);
    al_draw_filled_rectangle(x + w * 0.72f, yBase - h, x + w, yBase,
        al_map_rgb(185, 140, 88));
    al_draw_rectangle(x, yBase - h, x + w, yBase, borda, 2);

    /* selo verde */
    al_draw_filled_circle(x + w * 0.36f, yBase - h * 0.45f, w * 0.16f,
        al_map_rgb(60, 150, 80));
    al_draw_circle(x + w * 0.36f, yBase - h * 0.45f, w * 0.16f,
        al_map_rgb(255, 255, 255), 1.5f);
}

/* Rolo de papel higienico em pe (x = centro, e = escala) */
static void roloPapel(float x, float yBase, float e)
{
    ALLEGRO_COLOR branco = al_map_rgb(250, 250, 250);
    ALLEGRO_COLOR cinza = al_map_rgb(170, 170, 175);
    float w = 13 * e;
    float h = 30 * e;

    al_draw_filled_rectangle(x - w, yBase - h, x + w, yBase, branco);
    al_draw_rectangle(x - w, yBase - h, x + w, yBase, cinza, 1.5f);

    al_draw_filled_ellipse(x, yBase - h, w, 5 * e, branco);
    al_draw_ellipse(x, yBase - h, w, 5 * e, cinza, 1.5f);
    al_draw_filled_ellipse(x, yBase - h, 5 * e, 2.5f * e,
        al_map_rgb(190, 150, 100));
}

/* Aviaozinho de papel apontando para a direita */
static void aviaoPapel(float x, float y, float e, ALLEGRO_COLOR cor)
{
    ALLEGRO_COLOR sombra = al_map_rgb(150, 180, 225);

    al_draw_filled_triangle(x, y, x + 60 * e, y + 10 * e,
        x + 10 * e, y + 14 * e, cor);
    al_draw_filled_triangle(x + 10 * e, y + 14 * e, x + 60 * e, y + 10 * e,
        x + 24 * e, y + 28 * e, sombra);
    al_draw_line(x, y, x + 60 * e, y + 10 * e, al_map_rgb(70, 100, 160), 1);
    al_draw_line(x + 10 * e, y + 14 * e, x + 60 * e, y + 10 * e,
        al_map_rgb(70, 100, 160), 1);
}

static void envelope(float x, float y)
{
    ALLEGRO_COLOR borda = al_map_rgb(90, 120, 170);

    al_draw_filled_rectangle(x, y, x + 40, y + 28, al_map_rgb(255, 255, 255));
    al_draw_rectangle(x, y, x + 40, y + 28, borda, 1.5f);
    al_draw_line(x, y, x + 20, y + 15, borda, 1.5f);
    al_draw_line(x + 40, y, x + 20, y + 15, borda, 1.5f);
    al_draw_filled_rectangle(x + 30, y + 18, x + 36, y + 24,
        al_map_rgb(220, 90, 90));
}

/* Bolinha de papel amassado */
static void bolaPapel(float x, float y, float r)
{
    ALLEGRO_COLOR cinza = al_map_rgb(150, 150, 160);

    al_draw_filled_circle(x, y, r, al_map_rgb(240, 240, 245));
    al_draw_line(x - r * 0.6f, y - r * 0.2f, x + r * 0.3f, y + r * 0.6f, cinza, 1);
    al_draw_line(x - r * 0.2f, y - r * 0.7f, x + r * 0.7f, y + r * 0.1f, cinza, 1);
    al_draw_circle(x, y, r, cinza, 1.5f);
}

/* ------------------------------------------------------------------ */
/* CENARIO AZUL - PAPEL: estante, caixas, jornais e avioes de papel    */
/* ------------------------------------------------------------------ */

static void cenarioPapel(float pisoY)
{
    ceuDegrade(pisoY, 170, 210, 250, 225, 240, 255);

    nuvem(70, 80, 1.0f, al_map_rgb(255, 255, 255));
    nuvem(540, 60, 0.9f, al_map_rgb(255, 255, 255));

    /* poucas coisas no ar: avioes de papel e uma carta */
    aviaoPapel(250, 110, 0.9f, al_map_rgb(255, 255, 255));
    aviaoPapel(630, 150, 0.8f, al_map_rgb(240, 248, 255));
    envelope(430, 80);

    /* piso de tabuas */
    pisoPapel(pisoY);

    /* sombras sob os objetos */
    sombraChao(128, pisoY + 2, 36, al_map_rgb(46, 80, 150));
    sombraChao(320, pisoY + 2, 92, al_map_rgb(46, 80, 150));
    sombraChao(464, pisoY + 2, 30, al_map_rgb(46, 80, 150));
    sombraChao(548, pisoY + 2, 24, al_map_rgb(46, 80, 150));
    sombraChao(615, pisoY + 2, 30, al_map_rgb(46, 80, 150));
    sombraChao(40, pisoY + 2, 36, al_map_rgb(46, 80, 150));
    sombraChao(760, pisoY + 2, 36, al_map_rgb(46, 80, 150));

    /* objetos de papel, bem espacados */
    caixaPapelao(100, pisoY, 56, 44);
    caixaPapelao(108, pisoY - 44, 40, 30);

    estante(240, pisoY, 160, 150);

    pilhaJornal(440, pisoY);
    sacolaPapel(530, pisoY, 36, 48);

    roloPapel(600, pisoY, 0.75f);
    roloPapel(621, pisoY, 0.75f);
    roloPapel(610.5f, pisoY - 22, 0.75f);

    /* papel amassado em primeiro plano */
    bolaPapel(150, pisoY + 28, 9);
    bolaPapel(330, pisoY + 40, 8);
    bolaPapel(560, pisoY + 32, 9);

    lixeira(15, pisoY, al_map_rgb(30, 90, 220));
    lixeira(735, pisoY, al_map_rgb(30, 90, 220));
}

/* ------------------------------------------------------------------ */
/* OBJETOS DE PLASTICO                                                 */
/* ------------------------------------------------------------------ */

/* Cadeira plastica branca vista de frente: (x, yBase) = canto inferior esquerdo */
static void cadeiraPlastica(float x, float yBase)
{
    ALLEGRO_COLOR corpo = al_map_rgb(248, 244, 240);
    ALLEGRO_COLOR borda = al_map_rgb(150, 100, 105);

    /* pernas */
    al_draw_filled_rectangle(x + 4, yBase - 36, x + 9, yBase, corpo);
    al_draw_rectangle(x + 4, yBase - 36, x + 9, yBase, borda, 1);
    al_draw_filled_rectangle(x + 37, yBase - 36, x + 42, yBase, corpo);
    al_draw_rectangle(x + 37, yBase - 36, x + 42, yBase, borda, 1);

    /* assento */
    al_draw_filled_rectangle(x, yBase - 42, x + 46, yBase - 34, corpo);
    al_draw_rectangle(x, yBase - 42, x + 46, yBase - 34, borda, 1.5f);

    /* encosto: dois montantes e tres ripas */
    al_draw_filled_rectangle(x + 2, yBase - 82, x + 8, yBase - 42, corpo);
    al_draw_rectangle(x + 2, yBase - 82, x + 8, yBase - 42, borda, 1);
    al_draw_filled_rectangle(x + 38, yBase - 82, x + 44, yBase - 42, corpo);
    al_draw_rectangle(x + 38, yBase - 82, x + 44, yBase - 42, borda, 1);

    for (int k = 0; k < 3; k++) {
        float ry = yBase - 80 + k * 13;

        al_draw_filled_rectangle(x + 8, ry, x + 38, ry + 7, corpo);
        al_draw_rectangle(x + 8, ry, x + 38, ry + 7, borda, 1);
    }
}

/* Balde de plastico: (x, yBase) = canto inferior esquerdo */
static void balde(float x, float yBase, float w, float h, ALLEGRO_COLOR cor)
{
    float topo = yBase - h;

    al_draw_arc(x + w / 2, topo, w * 0.5f, PI_F, PI_F,
        al_map_rgb(90, 90, 95), 2);

    al_draw_filled_triangle(x, topo, x + w, topo, x + w * 0.85f, yBase, cor);
    al_draw_filled_triangle(x, topo, x + w * 0.15f, yBase, x + w * 0.85f,
        yBase, cor);

    /* aro superior */
    al_draw_filled_rectangle(x - 1, topo - 3, x + w + 1, topo + 2,
        al_map_rgb(200, 110, 30));
}

/* Patinho de borracha (x = centro, e = escala) */
static void patoBorracha(float x, float yBase, float e)
{
    ALLEGRO_COLOR amarelo = al_map_rgb(255, 215, 50);
    ALLEGRO_COLOR laranja = al_map_rgb(240, 130, 30);

    al_draw_filled_triangle(x - 14 * e, yBase - 14 * e, x - 24 * e,
        yBase - 26 * e, x - 8 * e, yBase - 20 * e, amarelo);
    al_draw_filled_ellipse(x, yBase - 11 * e, 17 * e, 11 * e, amarelo);
    al_draw_filled_ellipse(x - 2 * e, yBase - 11 * e, 8 * e, 5 * e,
        al_map_rgb(235, 190, 30));

    al_draw_filled_circle(x + 10 * e, yBase - 27 * e, 9 * e, amarelo);
    al_draw_filled_triangle(x + 17 * e, yBase - 29 * e, x + 26 * e,
        yBase - 26 * e, x + 17 * e, yBase - 24 * e, laranja);
    al_draw_filled_circle(x + 12 * e, yBase - 30 * e, 1.8f * e,
        al_map_rgb(40, 30, 30));
}

/* Bola de praia inflavel (x, y = centro) */
static void bolaPraia(float x, float y, float r)
{
    for (int i = 0; i < 6; i++) {
        float a1 = i * PI_F / 3.0f;
        float a2 = (i + 1) * PI_F / 3.0f;
        ALLEGRO_COLOR c = (i % 3 == 0) ? al_map_rgb(230, 60, 60)
            : (i % 3 == 1) ? al_map_rgb(250, 250, 250)
            : al_map_rgb(60, 120, 220);

        al_draw_filled_triangle(x, y,
            x + cosf(a1) * r, y + sinf(a1) * r,
            x + cosf(a2) * r, y + sinf(a2) * r, c);
    }

    al_draw_circle(x, y, r, al_map_rgb(120, 60, 60), 1.5f);
    al_draw_filled_circle(x, y, r * 0.18f, al_map_rgb(250, 210, 70));
}

/* Copo descartavel com tampa e canudo */
static void copoCanudo(float x, float yBase, float w, float h,
    ALLEGRO_COLOR cor, ALLEGRO_COLOR tampa)
{
    float off = w * 0.15f;

    al_draw_line(x + w * 0.55f, yBase - h - 3, x + w * 0.80f, yBase - h - 14,
        al_map_rgb(250, 250, 250), 2);

    al_draw_filled_triangle(x, yBase - h, x + w, yBase - h,
        x + w - off, yBase, cor);
    al_draw_filled_triangle(x, yBase - h, x + off, yBase,
        x + w - off, yBase, cor);
    al_draw_filled_rectangle(x - 1, yBase - h - 3, x + w + 1, yBase - h, tampa);
}

/* Tampinha de garrafa vista de cima */
static void tampinha(float x, float y, ALLEGRO_COLOR cor)
{
    al_draw_filled_ellipse(x, y, 7, 3, cor);
    al_draw_ellipse(x, y, 7, 3, al_map_rgb(90, 30, 35), 1);
}

/* Garrafa PET deitada no chao: (x, y) = ponta do fundo, apontando para a direita */
static void garrafaDeitada(float x, float y, ALLEGRO_COLOR cor, ALLEGRO_COLOR tampa)
{
    /* corpo, ombro, gargalo e tampa */
    al_draw_filled_rectangle(x, y - 6, x + 24, y + 6, cor);
    al_draw_filled_triangle(x + 24, y - 6, x + 24, y + 6, x + 31, y, cor);
    al_draw_filled_rectangle(x + 30, y - 2, x + 35, y + 2, cor);
    al_draw_filled_rectangle(x + 35, y - 3, x + 39, y + 3, tampa);

    /* brilho */
    al_draw_filled_rectangle(x + 3, y - 4, x + 21, y - 2,
        al_map_rgba(255, 255, 255, 70));
}

/* ------------------------------------------------------------------ */
/* CENARIO VERMELHO - PLASTICO: por do sol com objetos de plastico     */
/* ------------------------------------------------------------------ */

static void cenarioPlastico(float pisoY)
{
    ALLEGRO_COLOR tampa = al_map_rgb(110, 25, 35);

    ceuDegrade(pisoY, 255, 190, 140, 215, 70, 75);

    /* sol baixo */
    al_draw_filled_circle(400, pisoY - 20, 90, al_map_rgb(255, 225, 160));
    al_draw_filled_circle(400, pisoY - 20, 65, al_map_rgb(255, 245, 200));

    /* nuvens do por do sol: topo claro e barriga rosada */
    nuvemSombra(60, 100, 1.0f, al_map_rgb(255, 222, 205),
        al_map_rgb(236, 150, 150));
    nuvemSombra(540, 70, 0.9f, al_map_rgb(255, 222, 205),
        al_map_rgb(236, 150, 150));
    nuvemSombra(300, 160, 0.55f, al_map_rgb(255, 215, 195),
        al_map_rgb(232, 145, 145));

    /* piso de lajotas em perspectiva */
    pisoPlastico(pisoY);

    /* sombras sob os objetos */
    sombraChao(83, pisoY + 2, 30, al_map_rgb(100, 28, 36));
    sombraChao(153, pisoY + 2, 28, al_map_rgb(100, 28, 36));
    sombraChao(203, pisoY + 2, 17, al_map_rgb(100, 28, 36));
    sombraChao(260, pisoY + 2, 36, al_map_rgb(100, 28, 36));
    sombraChao(540, pisoY + 2, 36, al_map_rgb(100, 28, 36));
    sombraChao(608, pisoY + 2, 22, al_map_rgb(100, 28, 36));
    sombraChao(662, pisoY + 2, 16, al_map_rgb(100, 28, 36));
    sombraChao(715, pisoY + 2, 17, al_map_rgb(100, 28, 36));
    sombraChao(763, pisoY + 2, 11, al_map_rgb(100, 28, 36));

    /* lado esquerdo: cadeira, garrafas PET e galao */
    cadeiraPlastica(60, pisoY);

    garrafa(130, pisoY, 13, 36, al_map_rgb(150, 200, 235), tampa);
    garrafa(147, pisoY, 13, 36, al_map_rgb(235, 110, 110), tampa);
    garrafa(164, pisoY, 13, 32, al_map_rgb(130, 200, 130), tampa);

    garrafa(190, pisoY, 26, 42, al_map_rgb(120, 170, 235), al_map_rgb(40, 80, 170));

    /* lado direito: balde, patinho, bola de praia e copo */
    balde(590, pisoY, 36, 30, al_map_rgb(245, 165, 60));
    patoBorracha(662, pisoY, 0.6f);
    bolaPraia(715, pisoY - 17, 17);
    copoCanudo(755, pisoY, 16, 24, al_map_rgb(240, 90, 90),
        al_map_rgb(255, 255, 255));

    /* detalhes no chao: duas garrafas deitadas e poucas tampinhas */
    garrafaDeitada(170, pisoY + 48, al_map_rgb(235, 110, 110), tampa);
    garrafaDeitada(610, pisoY + 56, al_map_rgb(150, 200, 235), tampa);

    tampinha(120, pisoY + 36, al_map_rgb(80, 150, 240));
    tampinha(290, pisoY + 62, al_map_rgb(250, 210, 70));
    tampinha(520, pisoY + 44, al_map_rgb(245, 245, 245));
    tampinha(740, pisoY + 50, al_map_rgb(80, 150, 240));

    lixeira(235, pisoY, al_map_rgb(220, 30, 30));
    lixeira(515, pisoY, al_map_rgb(220, 30, 30));
}

/* ------------------------------------------------------------------ */
/* OBJETOS DE VIDRO                                                    */
/* ------------------------------------------------------------------ */

static void arvore(float x, float pisoY, float e)
{
    al_draw_filled_rectangle(x - 8 * e, pisoY - 60 * e,
        x + 8 * e, pisoY, al_map_rgb(100, 70, 40));

    al_draw_filled_circle(x, pisoY - 85 * e, 40 * e, al_map_rgb(40, 140, 70));
    al_draw_filled_circle(x - 25 * e, pisoY - 60 * e, 28 * e, al_map_rgb(50, 160, 80));
    al_draw_filled_circle(x + 25 * e, pisoY - 60 * e, 28 * e, al_map_rgb(50, 160, 80));
}

/* Estufa de vidro com plantas: (x, pisoY) = canto inferior esquerdo */
static void estufa(float x, float pisoY, float w, float h)
{
    ALLEGRO_COLOR vidro = al_map_rgb(205, 238, 240);
    ALLEGRO_COLOR moldura = al_map_rgb(70, 120, 100);
    float paredeH = h * 0.62f;

    /* paineis de vidro */
    al_draw_filled_rectangle(x, pisoY - paredeH, x + w, pisoY, vidro);
    al_draw_filled_triangle(x, pisoY - paredeH, x + w, pisoY - paredeH,
        x + w / 2, pisoY - h, vidro);

    /* plantas dentro da estufa */
    for (int i = 0; i < 4; i++) {
        float px = x + w * (0.15f + 0.23f * i);

        al_draw_line(px, pisoY, px, pisoY - 14, al_map_rgb(40, 110, 50), 2);
        al_draw_filled_circle(px, pisoY - 20, 9, al_map_rgb(50, 160, 80));
        al_draw_filled_circle(px + 4, pisoY - 18, 2.5f, al_map_rgb(220, 60, 50));
    }

    /* moldura e divisorias */
    al_draw_rectangle(x, pisoY - paredeH, x + w, pisoY, moldura, 2.5f);
    al_draw_triangle(x, pisoY - paredeH, x + w, pisoY - paredeH,
        x + w / 2, pisoY - h, moldura, 2.5f);
    al_draw_line(x + w / 2, pisoY - h, x + w / 2, pisoY - paredeH, moldura, 2);

    for (int i = 1; i < 4; i++) {
        al_draw_line(x + i * w / 4, pisoY - paredeH, x + i * w / 4, pisoY,
            moldura, 2);
    }

    /* reflexos */
    al_draw_line(x + 10, pisoY - paredeH + 10, x + 24, pisoY - paredeH + 24,
        al_map_rgb(255, 255, 255), 3);
}

/* Pote de conserva com tampa de metal */
static void poteConserva(float x, float yBase, float w, float h,
    ALLEGRO_COLOR conteudo)
{
    ALLEGRO_COLOR vidro = al_map_rgb(210, 238, 234);
    ALLEGRO_COLOR borda = al_map_rgb(60, 130, 110);

    al_draw_filled_rectangle(x, yBase - h, x + w, yBase, vidro);
    al_draw_filled_rectangle(x + 2, yBase - h * 0.55f, x + w - 2, yBase - 2,
        conteudo);
    al_draw_rectangle(x, yBase - h, x + w, yBase, borda, 1.5f);

    /* tampa de rosca */
    al_draw_filled_rectangle(x - 1, yBase - h - 5, x + w + 1, yBase - h,
        al_map_rgb(190, 190, 200));
    al_draw_rectangle(x - 1, yBase - h - 5, x + w + 1, yBase - h,
        al_map_rgb(120, 120, 130), 1);
}

/* Taca de vidro com suco (x = centro) */
static void taca(float x, float yBase, float e)
{
    ALLEGRO_COLOR vidro = al_map_rgb(210, 238, 238);
    ALLEGRO_COLOR borda = al_map_rgb(70, 140, 130);

    al_draw_filled_ellipse(x, yBase - 3 * e, 13 * e, 3 * e, vidro);
    al_draw_ellipse(x, yBase - 3 * e, 13 * e, 3 * e, borda, 1.5f);
    al_draw_filled_rectangle(x - 2 * e, yBase - 30 * e, x + 2 * e,
        yBase - 3 * e, vidro);
    al_draw_rectangle(x - 2 * e, yBase - 30 * e, x + 2 * e,
        yBase - 3 * e, borda, 1);

    al_draw_filled_ellipse(x, yBase - 42 * e, 15 * e, 14 * e, vidro);
    al_draw_ellipse(x, yBase - 42 * e, 15 * e, 14 * e, borda, 1.5f);
    al_draw_filled_ellipse(x, yBase - 38 * e, 12 * e, 9 * e,
        al_map_rgb(235, 140, 50));
}

/* Jarro de vidro com flores (x = centro, e = escala) */
static void vasoFlores(float x, float yBase, float e)
{
    ALLEGRO_COLOR vidro = al_map_rgb(205, 238, 235);
    ALLEGRO_COLOR borda = al_map_rgb(60, 130, 110);
    ALLEGRO_COLOR verde = al_map_rgb(40, 120, 55);

    al_draw_filled_ellipse(x, yBase - 20 * e, 18 * e, 20 * e, vidro);
    al_draw_filled_rectangle(x - 7 * e, yBase - 56 * e, x + 7 * e,
        yBase - 36 * e, vidro);
    al_draw_filled_rectangle(x - 10 * e, yBase - 60 * e, x + 10 * e,
        yBase - 56 * e, vidro);
    al_draw_ellipse(x, yBase - 20 * e, 18 * e, 20 * e, borda, 1.5f);
    al_draw_rectangle(x - 10 * e, yBase - 60 * e, x + 10 * e,
        yBase - 56 * e, borda, 1.5f);

    /* agua */
    al_draw_filled_ellipse(x, yBase - 14 * e, 14 * e, 13 * e,
        al_map_rgb(165, 215, 240));

    /* hastes e flores */
    al_draw_line(x - 2 * e, yBase - 30 * e, x - 9 * e, yBase - 80 * e, verde, 1.5f);
    al_draw_line(x, yBase - 30 * e, x, yBase - 92 * e, verde, 1.5f);
    al_draw_line(x + 2 * e, yBase - 30 * e, x + 10 * e, yBase - 78 * e, verde, 1.5f);

    al_draw_filled_circle(x - 9 * e, yBase - 82 * e, 6 * e, al_map_rgb(235, 110, 160));
    al_draw_filled_circle(x, yBase - 94 * e, 6 * e, al_map_rgb(250, 215, 70));
    al_draw_filled_circle(x + 10 * e, yBase - 80 * e, 6 * e, al_map_rgb(170, 110, 220));
}

/* Oculos esquecidos na grama: (x, y) = ponta esquerda, e = escala */
static void oculos(float x, float y, float e)
{
    ALLEGRO_COLOR aro = al_map_rgb(110, 70, 40);

    al_draw_filled_circle(x + 10 * e, y, 10 * e, al_map_rgb(215, 240, 245));
    al_draw_filled_circle(x + 34 * e, y, 10 * e, al_map_rgb(215, 240, 245));
    al_draw_circle(x + 10 * e, y, 10 * e, aro, 2.5f);
    al_draw_circle(x + 34 * e, y, 10 * e, aro, 2.5f);
    al_draw_line(x + 20 * e, y, x + 24 * e, y, aro, 2.5f);
    al_draw_line(x, y - 2 * e, x - 9 * e, y - 7 * e, aro, 2);
    al_draw_line(x + 44 * e, y - 2 * e, x + 53 * e, y - 7 * e, aro, 2);
}

/* ------------------------------------------------------------------ */
/* CENARIO VERDE - VIDRO: arvore grande, estufa e objetos de vidro     */
/* ------------------------------------------------------------------ */

static void cenarioVidro(float pisoY)
{
    ALLEGRO_COLOR verdeVidro = al_map_rgb(30, 150, 90);
    ALLEGRO_COLOR tampa = al_map_rgb(20, 90, 55);

    ceuDegrade(pisoY, 185, 235, 225, 225, 250, 230);

    /* sol */
    al_draw_filled_circle(680, 90, 45, al_map_rgb(255, 245, 170));

    nuvem(80, 90, 1.0f, al_map_rgb(255, 255, 255));
    nuvem(360, 60, 0.8f, al_map_rgb(255, 255, 255));

    /* colinas */
    al_draw_filled_circle(160, pisoY + 120, 230, al_map_rgb(130, 200, 140));
    al_draw_filled_circle(620, pisoY + 150, 260, al_map_rgb(100, 180, 120));

    /* gramado */
    pisoVidro(pisoY);

    /* sombras sob os objetos */
    sombraChao(190, pisoY + 3, 50, al_map_rgb(38, 98, 58));
    sombraChao(272, pisoY + 3, 26, al_map_rgb(38, 98, 58));
    sombraChao(325, pisoY + 3, 28, al_map_rgb(38, 98, 58));
    sombraChao(385, pisoY + 3, 11, al_map_rgb(38, 98, 58));
    sombraChao(428, pisoY + 3, 14, al_map_rgb(38, 98, 58));
    sombraChao(615, pisoY + 3, 80, al_map_rgb(38, 98, 58));
    sombraChao(40, pisoY + 3, 36, al_map_rgb(38, 98, 58));
    sombraChao(760, pisoY + 3, 36, al_map_rgb(38, 98, 58));

    /* arvore grande: as garrafas ficam bem menores que ela */
    arvore(190, pisoY, 2.0f);

    /* garrafas de vidro verdes */
    garrafa(250, pisoY, 13, 36, verdeVidro, tampa);
    garrafa(265, pisoY, 13, 36, al_map_rgb(60, 185, 115), tampa);
    garrafa(280, pisoY, 13, 32, verdeVidro, tampa);

    /* potes, taca e jarro */
    poteConserva(310, pisoY, 20, 24, al_map_rgb(200, 90, 40));
    poteConserva(336, pisoY, 16, 18, al_map_rgb(120, 170, 60));
    taca(385, pisoY, 0.6f);
    vasoFlores(428, pisoY, 0.5f);

    /* estufa de vidro */
    estufa(540, pisoY, 150, 95);

    /* oculos esquecidos na grama */
    oculos(470, pisoY + 26, 0.6f);

    lixeira(15, pisoY, al_map_rgb(30, 160, 70));
    lixeira(735, pisoY, al_map_rgb(30, 160, 70));
}

/* ------------------------------------------------------------------ */
/* OBJETOS DE METAL                                                    */
/* ------------------------------------------------------------------ */

/* Bidao de aco: (x, yBase) = canto inferior esquerdo */
static void bidao(float x, float yBase, float w, float h, ALLEGRO_COLOR cor)
{
    ALLEGRO_COLOR escuro = al_map_rgb(45, 45, 55);

    al_draw_filled_rectangle(x, yBase - h, x + w, yBase, cor);
    al_draw_line(x, yBase - h, x, yBase, escuro, 2);
    al_draw_line(x + w, yBase - h, x + w, yBase, escuro, 2);

    /* aros de reforco */
    al_draw_line(x, yBase - h * 0.33f, x + w, yBase - h * 0.33f, escuro, 2);
    al_draw_line(x, yBase - h * 0.66f, x + w, yBase - h * 0.66f, escuro, 2);

    /* tampa */
    al_draw_filled_ellipse(x + w / 2, yBase - h, w / 2, 5,
        al_map_rgb(190, 190, 200));
    al_draw_ellipse(x + w / 2, yBase - h, w / 2, 5, escuro, 1.5f);

    /* brilho */
    al_draw_filled_rectangle(x + w * 0.15f, yBase - h + 8, x + w * 0.27f,
        yBase - 6, al_map_rgba(255, 255, 255, 70));
}

/* Caixa de ferramentas vermelha */
static void caixaFerramentas(float x, float yBase, float w, float h)
{
    ALLEGRO_COLOR escuro = al_map_rgb(60, 20, 20);

    al_draw_arc(x + w / 2, yBase - h, w * 0.28f, PI_F, PI_F, escuro, 3);

    al_draw_filled_rectangle(x, yBase - h, x + w, yBase, al_map_rgb(205, 50, 40));
    al_draw_rectangle(x, yBase - h, x + w, yBase, escuro, 2);
    al_draw_line(x, yBase - h * 0.5f, x + w, yBase - h * 0.5f, escuro, 2);

    /* travas */
    al_draw_filled_rectangle(x + 5, yBase - h * 0.58f, x + 11, yBase - h * 0.42f,
        al_map_rgb(235, 190, 40));
    al_draw_filled_rectangle(x + w - 11, yBase - h * 0.58f, x + w - 5,
        yBase - h * 0.42f, al_map_rgb(235, 190, 40));
}

/* Chave inglesa deitada: (x, y) = ponta aberta; fundo = cor do chao */
static void chaveInglesa(float x, float y, float c,
    ALLEGRO_COLOR cor, ALLEGRO_COLOR fundo)
{
    al_draw_line(x, y, x + c, y, cor, 6);

    /* cabeca aberta */
    al_draw_filled_circle(x, y, 8, cor);
    al_draw_filled_rectangle(x - 10, y - 3.5f, x - 1, y + 3.5f, fundo);

    /* cabeca fechada */
    al_draw_filled_circle(x + c, y, 7, cor);
    al_draw_filled_circle(x + c, y, 3.5f, fundo);
}

/* Porca sextavada vista de frente */
static void porca(float x, float y, float r,
    ALLEGRO_COLOR cor, ALLEGRO_COLOR fundo)
{
    for (int i = 0; i < 6; i++) {
        float a1 = i * PI_F / 3.0f;
        float a2 = (i + 1) * PI_F / 3.0f;

        al_draw_filled_triangle(x, y,
            x + cosf(a1) * r, y + sinf(a1) * r,
            x + cosf(a2) * r, y + sinf(a2) * r,
            cor);
    }

    al_draw_filled_circle(x, y, r * 0.45f, fundo);
}

/* Parafuso deitado: (x, y) = centro da cabeca */
static void parafuso(float x, float y, float c,
    ALLEGRO_COLOR cor, ALLEGRO_COLOR escuro)
{
    al_draw_filled_rectangle(x, y - 8, x + 9, y + 8, cor);
    al_draw_filled_rectangle(x + 9, y - 4.5f, x + 9 + c, y + 4.5f, cor);

    for (float t = x + 14; t < x + 9 + c; t += 5)
        al_draw_line(t, y - 4.5f, t + 3, y + 4.5f, escuro, 1.5f);
}

/* Bicicleta (sucata de metal): (x, yBase) = ponto mais baixo da roda de tras */
static void bicicleta(float x, float yBase, ALLEGRO_COLOR quadro)
{
    ALLEGRO_COLOR pneu = al_map_rgb(40, 40, 45);
    ALLEGRO_COLOR raio = al_map_rgb(120, 120, 130);
    float cy = yBase - 24;
    float rx = x + 22, fx = x + 96;             /* centros das rodas */
    float px = x + 58, py = cy + 2;             /* pedivela */
    float sx = x + 50, sy = cy - 34;            /* selim */
    float hx = x + 86, hy = cy - 36;            /* guidao */

    /* rodas */
    al_draw_circle(rx, cy, 22, pneu, 4);
    al_draw_circle(fx, cy, 22, pneu, 4);

    for (int i = 0; i < 4; i++) {
        float a = i * PI_F / 4.0f;

        al_draw_line(rx - cosf(a) * 22, cy - sinf(a) * 22,
            rx + cosf(a) * 22, cy + sinf(a) * 22, raio, 1);
        al_draw_line(fx - cosf(a) * 22, cy - sinf(a) * 22,
            fx + cosf(a) * 22, cy + sinf(a) * 22, raio, 1);
    }

    /* quadro */
    al_draw_line(rx, cy, px, py, quadro, 3);
    al_draw_line(rx, cy, sx, sy, quadro, 3);
    al_draw_line(px, py, sx, sy, quadro, 3);
    al_draw_line(sx, sy, hx, hy, quadro, 3);
    al_draw_line(px, py, hx, hy, quadro, 3);
    al_draw_line(hx, hy, fx, cy, quadro, 3);

    /* selim, guidao e pedal */
    al_draw_line(sx - 8, sy - 3, sx + 4, sy - 3, pneu, 4);
    al_draw_line(hx - 2, hy - 3, hx + 8, hy - 7, pneu, 3);
    al_draw_circle(px, py, 5, pneu, 2);
}

/* ------------------------------------------------------------------ */
/* CENARIO AMARELO - METAL: fabrica ao fundo e sucata de metal         */
/* ------------------------------------------------------------------ */

static void cenarioMetal(float pisoY)
{
    ALLEGRO_COLOR cinza = al_map_rgb(115, 115, 125);
    ALLEGRO_COLOR cinzaEscuro = al_map_rgb(80, 80, 90);
    ALLEGRO_COLOR amareloLata = al_map_rgb(240, 200, 40);
    ALLEGRO_COLOR amareloFaixa = al_map_rgb(220, 180, 30);
    ALLEGRO_COLOR corPiso = al_map_rgb(156, 128, 40);   /* cor media do chao, usada no furo das ferramentas */
    ALLEGRO_COLOR aco = al_map_rgb(170, 170, 182);
    ALLEGRO_COLOR acoEscuro = al_map_rgb(85, 85, 95);
    ALLEGRO_COLOR furo = al_map_rgb(255, 245, 200);

    ceuDegrade(pisoY, 255, 240, 170, 255, 250, 215);

    /* sol */
    al_draw_filled_circle(120, 100, 50, al_map_rgb(255, 215, 60));

    /* fabrica ao fundo (menor, pois esta distante) */
    al_draw_filled_rectangle(625, pisoY - 185, 650, pisoY - 90, cinzaEscuro);
    al_draw_filled_rectangle(625, pisoY - 185, 650, pisoY - 176, amareloFaixa);
    al_draw_filled_rectangle(735, pisoY - 150, 760, pisoY - 90, cinzaEscuro);
    al_draw_filled_rectangle(735, pisoY - 150, 760, pisoY - 141, amareloFaixa);

    al_draw_filled_circle(640, pisoY - 198, 11, al_map_rgba(160, 160, 160, 200));
    al_draw_filled_circle(652, pisoY - 218, 15, al_map_rgba(175, 175, 175, 190));
    al_draw_filled_circle(748, pisoY - 163, 10, al_map_rgba(160, 160, 160, 200));
    al_draw_filled_circle(758, pisoY - 182, 13, al_map_rgba(175, 175, 175, 190));

    al_draw_filled_rectangle(610, pisoY - 90, 780, pisoY, cinza);
    al_draw_filled_triangle(610, pisoY - 90, 695, pisoY - 120, 695, pisoY - 90, cinza);
    al_draw_filled_triangle(695, pisoY - 90, 780, pisoY - 120, 780, pisoY - 90, cinza);

    for (int i = 0; i < 3; i++) {
        al_draw_filled_rectangle(625 + i * 40, pisoY - 72, 651 + i * 40,
            pisoY - 48, al_map_rgb(255, 225, 90));
        al_draw_rectangle(625 + i * 40, pisoY - 72, 651 + i * 40,
            pisoY - 48, cinzaEscuro, 2);
    }

    al_draw_filled_rectangle(745, pisoY - 42, 772, pisoY, cinzaEscuro);

    /* piso de chapas de aco com faixa de seguranca */
    pisoMetal(pisoY);

    /* sombras sob os objetos */
    sombraChao(115, pisoY + 2, 34, al_map_rgb(112, 90, 24));
    sombraChao(184, pisoY + 2, 24, al_map_rgb(112, 90, 24));
    sombraChao(270, pisoY + 2, 38, al_map_rgb(112, 90, 24));
    sombraChao(427, pisoY + 2, 26, al_map_rgb(112, 90, 24));
    sombraChao(502, pisoY + 2, 22, al_map_rgb(112, 90, 24));
    sombraChao(576, pisoY + 2, 22, al_map_rgb(112, 90, 24));
    sombraChao(40, pisoY + 2, 36, al_map_rgb(112, 90, 24));
    sombraChao(370, pisoY + 2, 36, al_map_rgb(112, 90, 24));

    /* pilha de latas (3 + 2 + 1) */
    lata(90, pisoY, 16, 24, amareloLata);
    lata(107, pisoY, 16, 24, amareloLata);
    lata(124, pisoY, 16, 24, amareloLata);
    lata(98.5f, pisoY - 24, 16, 24, amareloLata);
    lata(115.5f, pisoY - 24, 16, 24, amareloLata);
    lata(107, pisoY - 48, 16, 24, amareloLata);

    /* bidao de aco */
    bidao(165, pisoY, 38, 54, al_map_rgb(70, 110, 160));

    /* par de engrenagens encaixadas */
    engrenagem(250, pisoY - 30, 20, al_map_rgb(200, 165, 30), furo);
    engrenagem(292, pisoY - 19, 12, al_map_rgb(225, 185, 40), furo);

    /* caixa de ferramentas e bicicleta velha */
    caixaFerramentas(405, pisoY, 44, 28);
    bicicleta(480, pisoY, al_map_rgb(200, 60, 40));

    /* ferramentas e ferragens em primeiro plano */
    chaveInglesa(130, pisoY + 30, 48, aco, corPiso);
    porca(250, pisoY + 36, 9, aco, corPiso);
    parafuso(380, pisoY + 26, 30, aco, acoEscuro);

    lixeira(15, pisoY, al_map_rgb(235, 190, 20));
    lixeira(345, pisoY, al_map_rgb(235, 190, 20));
}

/* ------------------------------------------------------------------ */
/* FUNCAO PRINCIPAL                                                    */
/* ------------------------------------------------------------------ */

void cenarios(int tipo, float pisoY)
{
    switch (tipo) {

    case CENARIO_PAPEL:
        cenarioPapel(pisoY);
        break;

    case CENARIO_PLASTICO:
        cenarioPlastico(pisoY);
        break;

    case CENARIO_VIDRO:
        cenarioVidro(pisoY);
        break;

    case CENARIO_METAL:
        cenarioMetal(pisoY);
        break;

    default:
        cenarioPapel(pisoY);
        break;
    }
}