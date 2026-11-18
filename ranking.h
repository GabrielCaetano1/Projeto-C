#ifndef RANKING_H
#define RANKING_H

#define MAX_NOME 50
#define MAX_RANKING 10
#define ARQUIVO_RANKING "ranking.txt"

typedef struct {
    char nome[MAX_NOME];
    int tentativas;
} Jogador;

void carregar_ranking(Jogador *ranking, int *total, const char *caminho);

void salvar_ranking(const Jogador *ranking, int total, const char *caminho);

int registrar_no_ranking(Jogador *ranking, int *total, const char *nome, int tentativas);

void exibir_ranking(const Jogador *ranking, int total);

#endif
