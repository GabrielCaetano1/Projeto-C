#include <stdio.h>
#include <string.h>
#include "ranking.h"

static void ordenar_ranking(Jogador *ranking, int total) {
    int i, j;
    for (i = 1; i < total; i++) {
        Jogador chave = ranking[i];
        j = i - 1;
        while (j >= 0 && ranking[j].tentativas > chave.tentativas) {
            ranking[j + 1] = ranking[j];
            j--;
        }
        ranking[j + 1] = chave;
    }
}

void carregar_ranking(Jogador *ranking, int *total, const char *caminho) {
    FILE *fp = fopen(caminho, "r");
    Jogador temp;

    *total = 0;
    if (fp == NULL) {
        return;
    }

    while (*total < MAX_RANKING &&
           fscanf(fp, "%d;%49[^\r\n]%*[\r\n]", &temp.tentativas, temp.nome) == 2) {
        if (temp.tentativas > 0) {
            ranking[*total] = temp;
            (*total)++;
        }
    }

    fclose(fp);
    ordenar_ranking(ranking, *total);
}

void salvar_ranking(const Jogador *ranking, int total, const char *caminho) {
    FILE *fp = fopen(caminho, "w");
    int i;

    if (fp == NULL) {
        printf("\n[Aviso] Nao foi possivel salvar o ranking em '%s'.\n", caminho);
        return;
    }

    for (i = 0; i < total; i++) {
        fprintf(fp, "%d;%s\n", ranking[i].tentativas, ranking[i].nome);
    }

    fclose(fp);
}

int registrar_no_ranking(Jogador *ranking, int *total, const char *nome, int tentativas) {
    Jogador novo;
    size_t i;

    strncpy(novo.nome, nome, MAX_NOME - 1);
    novo.nome[MAX_NOME - 1] = '\0';

    for (i = 0; novo.nome[i] != '\0'; i++) {
        if (novo.nome[i] == ';') {
            novo.nome[i] = ' ';
        }
    }
    if (novo.nome[0] == '\0') {
        strcpy(novo.nome, "Agente Anonimo");
    }
    novo.tentativas = tentativas;

    if (*total < MAX_RANKING) {
        ranking[*total] = novo;
        (*total)++;
        ordenar_ranking(ranking, *total);
        return 1;
    }

    if (tentativas < ranking[MAX_RANKING - 1].tentativas) {
        ranking[MAX_RANKING - 1] = novo;
        ordenar_ranking(ranking, *total);
        return 1;
    }

    return 0;
}

void exibir_ranking(const Jogador *ranking, int total) {
    int i;

    printf("\n==================================================\n");
    printf("          RANKING - MELHORES HACKERS (TOP 10)     \n");
    printf("==================================================\n");

    if (total == 0) {
        printf("  Nenhum registro ainda. Seja o primeiro a escapar!\n");
        printf("==================================================\n");
        return;
    }

    printf(" Pos. | %-30s | Tentativas\n", "Codinome");
    printf("------+--------------------------------+-----------\n");
    for (i = 0; i < total; i++) {
        printf(" %2d.  | %-30s | %d\n",
               i + 1, ranking[i].nome, ranking[i].tentativas);
    }
    printf("==================================================\n");
}
