#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "jogo.h"

#define ANSI_RESET   "\033[0m"
#define ANSI_VERM    "\033[1;31m"
#define ANSI_AMAR    "\033[1;33m"
#define ANSI_MAG     "\033[1;35m"
#define ANSI_CIANO   "\033[1;36m"
#define ANSI_AZUL    "\033[1;34m"
#define ANSI_VERDE   "\033[1;32m"

static void limpar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void exibir_menu(void) {
    printf("\n==================================================\n");
    printf("   TERMINAL DO VILAO CIPHER - PROTOCOLO DE FUGA   \n");
    printf("==================================================\n");
    printf("  [1] Jogar (Humano adivinha o codigo)\n");
    printf("  [2] Modo IA (Computador adivinha seu numero)\n");
    printf("  [3] Ver ranking (Top 10)\n");
    printf("  [0] Sair\n");
    printf("--------------------------------------------------\n");
}

int ler_inteiro_intervalo(const char *prompt, int min, int max) {
    int valor;
    int lido;
    int prox;
    int lixo_na_linha;

    while (1) {
        printf("%s", prompt);
        lido = scanf("%d", &valor);

        if (lido == EOF) {
            return min;
        }

        lixo_na_linha = 0;
        if (lido == 1) {
            while ((prox = getchar()) != '\n' && prox != EOF) {
                if (!isspace((unsigned char)prox)) {
                    lixo_na_linha = 1;
                }
            }
        } else {
            limpar_buffer();
        }

        if (lido != 1 || lixo_na_linha) {
            printf(ANSI_VERM "[Erro] Entrada invalida! Digite apenas um numero inteiro.\n" ANSI_RESET);
            continue;
        }

        if (valor < min || valor > max) {
            printf(ANSI_AMAR "[Aviso] Numero fora do intervalo permitido [%d, %d].\n" ANSI_RESET,
                   min, max);
            continue;
        }

        return valor;
    }
}

static char ler_resposta_ia(void) {
    char c;
    int prox;
    int valido;

    while (1) {
        printf("Sua resposta ('>' maior, '<' menor, '=' acertou): ");
        if (scanf(" %c", &c) == EOF) {
            return '=';
        }

        valido = 1;
        while ((prox = getchar()) != '\n' && prox != EOF) {
            if (!isspace((unsigned char)prox)) {
                valido = 0;
            }
        }

        if (valido && (c == '>' || c == '<' || c == '=')) {
            return c;
        }

        printf(ANSI_VERM "[Erro] Digite apenas '>', '<' ou '='.\n" ANSI_RESET);
    }
}

static void escolher_dificuldade(int *limite_max, int *max_tentativas) {
    int nivel;

    printf("\n--- Nivel de Seguranca do Terminal ---\n");
    printf("  [1] Facil   (1 a 100)\n");
    printf("  [2] Medio   (1 a 1000)\n");
    printf("  [3] Dificil (1 a 10000)\n");
    nivel = ler_inteiro_intervalo("Escolha a dificuldade [1-3]: ", 1, 3);

    switch (nivel) {
        case 1:
            *limite_max = 100;
            break;
        case 3:
            *limite_max = 10000;
            break;
        case 2:
        default:
            *limite_max = 1000;
            break;
    }

    *max_tentativas = (int)ceil(log2((double)(*limite_max)));
}

static const char *obter_cor_temperatura(int palpite, int secreto, int limite_max, const char **rotulo) {
    int distancia = abs(palpite - secreto);
    double razao = (double)distancia / (double)limite_max;

    if (distancia == 0) {
        *rotulo = "EXATO";
        return ANSI_VERDE;
    }
    if (razao <= 0.02 || distancia <= 3) {
        *rotulo = "FERVENDO";
        return ANSI_VERM;
    }
    if (razao <= 0.10) {
        *rotulo = "QUENTE";
        return ANSI_AMAR;
    }
    if (razao <= 0.25) {
        *rotulo = "MORNO";
        return ANSI_MAG;
    }
    if (razao <= 0.50) {
        *rotulo = "FRIO";
        return ANSI_CIANO;
    }
    *rotulo = "CONGELANDO";
    return ANSI_AZUL;
}

static void exibir_historico(const int *historico, int qtd, int secreto, int limite_max) {
    int i;
    const char *rotulo = "";
    const char *cor;

    if (qtd == 0) {
        return;
    }

    printf("Historico de palpites: ");
    for (i = 0; i < qtd; i++) {
        cor = obter_cor_temperatura(historico[i], secreto, limite_max, &rotulo);
        printf("%s%d (%s)%s", cor, historico[i], rotulo, ANSI_RESET);
        if (i < qtd - 1) {
            printf(" | ");
        }
    }
    printf("\n");
}

static void ler_nome_jogador(char *destino, int tamanho) {
    size_t len;

    printf("Digite seu codinome para o ranking: ");
    if (fgets(destino, tamanho, stdin) == NULL) {
        strcpy(destino, "Agente Anonimo");
        return;
    }

    len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') {
        destino[len - 1] = '\0';
    } else {
        limpar_buffer();
    }

    if (destino[0] == '\0') {
        strcpy(destino, "Agente Anonimo");
    }
}

void jogar_modo_humano(Jogador *ranking, int *total_ranking) {
    int limite_max = 1000;
    int max_tentativas = 10;
    int secreto;
    int palpite;
    int tentativas = 0;
    int acertou = 0;
    int *historico = NULL;
    const char *rotulo = "";
    const char *cor;

    escolher_dificuldade(&limite_max, &max_tentativas);

    historico = (int *)malloc((size_t)max_tentativas * sizeof(int));
    if (historico == NULL) {
        printf(ANSI_VERM "[Erro] Falha ao alocar memoria para o historico.\n" ANSI_RESET);
        return;
    }

    secreto = (rand() % limite_max) + 1;

    printf("\n[CIPHER] Terminal bloqueado! Codigo entre 1 e %d.\n", limite_max);
    printf("Voce tem %d tentativas antes do alarme disparar.\n\n", max_tentativas);

    do {
        exibir_historico(historico, tentativas, secreto, limite_max);
        printf("Tentativa %d de %d\n", tentativas + 1, max_tentativas);
        palpite = ler_inteiro_intervalo("Seu palpite: ", 1, limite_max);

        historico[tentativas] = palpite;
        tentativas++;

        if (palpite == secreto) {
            acertou = 1;
        } else {
            cor = obter_cor_temperatura(palpite, secreto, limite_max, &rotulo);
            if (palpite < secreto) {
                printf("Dica: O codigo secreto eh MAIOR | Termometro: %s%s%s\n\n",
                       cor, rotulo, ANSI_RESET);
            } else {
                printf("Dica: O codigo secreto eh MENOR | Termometro: %s%s%s\n\n",
                       cor, rotulo, ANSI_RESET);
            }
        }
    } while (!acertou && tentativas < max_tentativas);

    if (acertou) {
        char nome[MAX_NOME];
        printf(ANSI_VERDE "\n[ACESSO LIBERADO] Voce hackeou o terminal em %d tentativa(s)!\n" ANSI_RESET,
               tentativas);
        ler_nome_jogador(nome, MAX_NOME);

        if (registrar_no_ranking(ranking, total_ranking, nome, tentativas)) {
            salvar_ranking(ranking, *total_ranking, ARQUIVO_RANKING);
            printf(ANSI_VERDE "Parabens, %s! Registro salvo no Top 10 (%s).\n" ANSI_RESET,
                   nome, ARQUIVO_RANKING);
        } else {
            printf("Voce venceu, mas nao superou o 10o lugar do ranking atual.\n");
        }
    } else {
        printf(ANSI_VERM "\n[ALARME DISPARADO] Tentativas esgotadas! O codigo secreto era %d.\n" ANSI_RESET,
               secreto);
    }

    free(historico);
    historico = NULL;
}

void jogar_modo_ia(void) {
    int limite_max = 1000;
    int max_tentativas = 10;
    int inicio;
    int fim;
    int meio;
    int chutes = 0;
    int acertou = 0;
    char resp;

    escolher_dificuldade(&limite_max, &max_tentativas);
    inicio = 1;
    fim = limite_max;

    printf("\n[MODO IA - BUSCA BINARIA]\n");
    printf("Pense em um numero secreto entre 1 e %d.\n", limite_max);
    printf("Vou descobri-lo em no maximo %d tentativas!\n", max_tentativas);
    printf("Responda: '>' se o seu numero for MAIOR, '<' se for MENOR, '=' se eu acertei.\n\n");

    while (inicio <= fim && chutes < max_tentativas && !acertou) {
        meio = inicio + (fim - inicio) / 2;
        chutes++;

        printf("Chute %d/%d -> O numero que voce pensou eh %d? ",
               chutes, max_tentativas, meio);
        resp = ler_resposta_ia();

        if (resp == '=') {
            acertou = 1;
        } else if (resp == '>') {
            inicio = meio + 1;
        } else {
            fim = meio - 1;
        }
    }

    if (acertou) {
        printf(ANSI_VERDE "\n[IA] Descobri o numero %d em %d tentativa(s)!\n" ANSI_RESET,
               meio, chutes);
        if (chutes > max_tentativas) {
            printf(ANSI_VERM "[Alerta] Algo esta errado: excedeu o limite teorico de %d chutes.\n" ANSI_RESET,
                   max_tentativas);
        }
    } else {
        printf(ANSI_VERM "\n[Erro na Busca Binaria] Algo esta errado! "
               "Suas respostas foram inconsistentes ou excederam %d tentativas.\n" ANSI_RESET,
               max_tentativas);
    }
}
