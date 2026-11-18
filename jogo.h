#ifndef JOGO_H
#define JOGO_H

#include "ranking.h"

void exibir_menu(void);

int ler_inteiro_intervalo(const char *prompt, int min, int max);

void jogar_modo_humano(Jogador *ranking, int *total_ranking);

void jogar_modo_ia(void);

#endif
