#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ranking.h"
#include "jogo.h"

int main(void) {
    Jogador ranking[MAX_RANKING];
    int total_ranking = 0;
    int opcao;

    srand((unsigned int)time(NULL));
    carregar_ranking(ranking, &total_ranking, ARQUIVO_RANKING);

    do {
        exibir_menu();
        opcao = ler_inteiro_intervalo("Escolha uma opcao [0-3]: ", 0, 3);

        switch (opcao) {
            case 1:
                jogar_modo_humano(ranking, &total_ranking);
                break;
            case 2:
                jogar_modo_ia();
                break;
            case 3:
                exibir_ranking(ranking, total_ranking);
                break;
            case 0:
                salvar_ranking(ranking, total_ranking, ARQUIVO_RANKING);
                printf("\nDesconectando do terminal do Cipher. Ate a proxima!\n");
                break;
            default:
                break;
        }
    } while (opcao != 0);

    return 0;
}
