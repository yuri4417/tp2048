#include "utils.h"
#include "cores.h"
#include "game.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
    #include <windows.h>
#else 
    #include <unistd.h>
#endif




// ===========================================================================================================================================================
// UTILITÁRIOS GERAIS
// ===========================================================================================================================================================
void limpar_buffer() {
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF);
}
void delay_ms(int ms) {
    clock_t start_time = clock();
    clock_t end_time = start_time + (ms * CLOCKS_PER_SEC / 1000);
    while (clock() < end_time);
}
void clear() {
    printf("\033[2J\033[H");
}
void zeraGuloso(Tabuleiro* jogo) {
	for (int i = 0; i < jogo->tam; i++) {
		for (int j = 0; j < jogo->tam; j++) {
			jogo->table[i][j].nao_guloso = 0;
		}
	}
}
void abortar(Tabuleiro* jogo) {
    if (jogo->table != NULL) {
        liberaMatriz(jogo->table, jogo->tam);
        liberaMatriz(jogo->table_bkp, jogo->tam);
    }
    printf(BOLD(RED("\nERRO DE ENTRADA. O Jogo será finalizado. \n")));
    delay_ms(1000);
    exit(-1);
}
int mudanca(Tabuleiro* jogo) {
    for (int i = 0; i < jogo->tam; i++)
        for (int j = 0; j < jogo->tam; j++)
            if (jogo->table[i][j].valor != jogo->table_bkp[i][j].valor)
                return 1; 
    return 0;
}
int verificaVitória(Tabuleiro* jogo) {
    for (int i = 0; i < jogo->tam; i++)
        for(int j = 0; j < jogo->tam; j++)
            if (jogo->table[i][j].valor == 2048)
                return 1;
    return 0;
}
int verificaDerrota(Tabuleiro* jogo) {
    for (int i = 0; i < jogo->tam; i++)
        for (int j = 0; j < jogo->tam; j++)
            if (jogo->table[i][j].valor == 0)
                return 0; //Célula com pelo menos 1 valor 0
    for (int i = 0; i < jogo->tam; i++) {
        for (int j = 0; j < jogo->tam; j++) {
            if (i < jogo->tam - 1)
                if (jogo->table[i][j].valor == jogo->table[i+1][j].valor)
                    return 0; //Célula abaixo igual a atual, movimento possível
            if (j < jogo->tam - 1)
                if (jogo->table[i][j].valor == jogo->table[i][j+1].valor)
                    return 0; //Célula a direita igual

        }
    }
    return 1;
}
int trataVitória(Tabuleiro* jogo, Tops* ranking) {
    jogo->verificado = 1;
    clear();
    imprimeTabela(jogo);
    scoreRodada(jogo);

    printf(BOLD(GREEN("\t\t\t\t\tParabéns! Você Venceu!\n")));
    if (perguntaSN(BOLD(GREEN("Deseja salvar sua pontuação no ranking?")))) {
        atualizaRanking(ranking, jogo);
        printf(BOLD(GREEN("Ranking Atualizado!\n")));
        delay_ms(750);
    }

    if (perguntaSN("Deseja continuar jogando?\n-> ")) {
        printf(BOLD(GREEN("Continuando o jogo...\n")));
        return 1;
    } 
    else {
        printf(BOLD(YELLOW("Voltando ao menu...\n")));
        delay_ms(500);
        return 0;
    }
}
int trataDerrota(Tabuleiro* jogo, Tops* ranking) {
    if (jogo->numUndo > 0) {
            clear();
            imprimeTabela(jogo);
            scoreRodada(jogo);
            printf(BOLD(RED("Você PERDEU!\n")));
            if (perguntaSN("Você pode desfazer seu último movimento. Gostaria de tentar mais uma vez?\n-> ")) {
                jogo->numUndo--;
                desfazMovimento(jogo);
                return 1;
            } 
            else {
                if (perguntaSN(BOLD(GREEN("Deseja salvar sua pontuação no ranking?")))) {
                    atualizaRanking(ranking, jogo);
                    printf(BOLD(GREEN("Ranking Atualizado!\n")));
                    delay_ms(750);
                }
                printf(BOLD(YELLOW("Voltando ao menu...\n")));
                delay_ms(650);
                return 0;
            }
    } 
    else {
        char opt[10];
        printf(BOLD(RED("Você PERDEU!\n")));
        printf(BOLD(RED("Sem movimentos de desfazer disponíveis.\n")));
        if (perguntaSN(BOLD(GREEN("Deseja salvar sua pontuação no ranking?")))) {
            atualizaRanking(ranking, jogo);
            printf(BOLD(GREEN("Ranking Atualizado!\n")));
            delay_ms(750);
        }
        printf("Aperte qualquer tecla para retornar ao menu...\n->");
        lerEntrada(opt, 10);
        return 0;
    }
}


// ===========================================================================================================================================================
// MANIPULAÇÃO DE STRINGS
// ===========================================================================================================================================================
int lerEntrada(char* str, int tam) {
    if (fgets(str, tam, stdin) == NULL) 
        return 0;
    
    int len = strcspn(str, "\n");
    if (len < tam-1)
        str[len] = '\0';
    else {
        str[tam-1] = '\0';
        limpar_buffer(); // Limpa se digitar mais que a string
    }
    
    for (int i = 0; str[i] < tam; i++)
            str[i] = converteMinuscula(str[i]);
    return 1;
}
char converteMinuscula(char letra) {
    if (letra >= 65 && letra <= 90)
        letra += 32;    
    return letra;
}
int perguntaSN(char* str) {
    char resp[16];
    while (1) {
        printf("%s", str);
        if (!lerEntrada(resp, 16)) return 0; // EOF == 'n'

        apagaEspaço(resp);

        if (strlen(resp) == 1) {
            char c = converteMinuscula(resp[0]);
            if (c == 's') return 1;
            if (c == 'n') return 0;
        }

        printf(BOLD(RED("Opção Inválida! Tente novamente (S/N)\n")));
        delay_ms(500);
    }
}
void catTxt(char *str, int tam) {
    char *ultimoPonto = strrchr(str, '.');
    if (ultimoPonto == NULL || strcmp(ultimoPonto+1, "txt") != 0) {
        if (strlen(str) + 4 < tam) {
            if (ultimoPonto == NULL) 
                strcat(str, ".txt");
            else 
                strcpy(ultimoPonto, ".txt");
        }
    }
}
int arrumaString(char *str) {
    int i = 1; // ignora o 't'

    if (str[1] != 32) //espaço logo depois do t
        return 0;

    while (str[i] != '\0') {
        if (str[i] != 32) {  
            if (isalpha(str[i])) { // Se for uma letra....
                if (str[i+1] == '\0' || !isdigit(str[i+1])) // e o proximo  for o fim da string ou não for um digito, é invalido
                    return 0;
            } else if (isdigit(str[i])) { // Se for um número...
                if (str[i+1] != '\0' && str[i+1] != ' ') // E não for o fim da string OU se não estiver espaçado, é invalido 
                    return 0;
            } else {
                return 0; // caractere qualquer
            }
        }
        i++;
    }

    apagaEspaço(str);
    i = 0;
    while (str[i] != '\0') {
        str[i] = converteMinuscula(str[i]);
        i++;
    }

    if (strlen(str) != 5) // formato tCXCX\0
        return 0;

    return 1; // string está no formato tCXCX\0 (C=char, x = nro), validação feita na função de troca
}
void apagaEspaço(char *str) {
    int i = 0;
    int j = 0;
    while (str[i] != '\0') {
        if (str[i] != 32) {
            str[j] = str[i];
            j++;
        }
        i++;
    }
    str[j] = '\0';
}
int stringReal(char* str) {
    int len = 0;
    int i = 0;
    while (str[i] != '\0') {
        if (str[i] == '\x1b') {
            while (str[i] != '\0' && str[i] != 'm') {
                i++;
            }
            if (str[i] != '\0') {
                i++;
            }
        } else {
            len++;
            i++;
        }
    }
    return len;
}



// ===========================================================================================================================================================
// IMPRESSÃO EM TELA
// ===========================================================================================================================================================
void imprimeTabela(Tabuleiro* jogo) {
    const int digitosMAX = 8;


    // borda superior
    for (int i = 0; i< jogo->tam; i++)
        printf("%*d", digitosMAX+1, 1+i);
    printf("\n");
    printf("  "TAB_TL);
    for (int j = 0; j < jogo->tam; j++) {
        for (int k = 0; k < digitosMAX; k++)  
            printf(TAB_HOR); 
        if (j < jogo->tam - 1) 
             printf(TAB_TJ); 
    }
    printf(TAB_TR);
    printf("\n");

    // nros
    for (int i = 0; i < jogo->tam; i++) {
        printf("%c", 'A'+i);
        printf(" "TAB_VER);
        for (int j = 0; j < jogo->tam; j++) {

            switch (jogo->table[i][j].valor) {
                case 0:    printf("%*s", digitosMAX, " "); break;
                case 2:    printf(BG_WHITE(BLACK("%*d")), digitosMAX, jogo->table[i][j].valor); break;
                case 4:    printf(BG_GREEN(BLACK("%*d")), digitosMAX, jogo->table[i][j].valor); break; 
                case 8:    printf(BG_CYAN(BLACK("%*d")), digitosMAX, jogo->table[i][j].valor); break; 
                case 16:   printf(BG_BLUE(WHITE("%*d")), digitosMAX, jogo->table[i][j].valor); break; 
                case 32:   printf(BG_YELLOW(BLACK("%*d")), digitosMAX, jogo->table[i][j].valor); break; 
                case 64:   printf(BG_MAGENTA(WHITE("%*d")), digitosMAX, jogo->table[i][j].valor); break;

                case 128:  printf(BG_WHITE(BOLD(BLUE("%*d"))), digitosMAX, jogo->table[i][j].valor); break;
                case 256:  printf(BG_GREEN(BOLD(WHITE("%*d"))), digitosMAX, jogo->table[i][j].valor); break;
                case 512:  printf(BG_YELLOW(BOLD(RED("%*d"))), digitosMAX, jogo->table[i][j].valor); break; 
                case 1024: printf(BG_BLUE(BOLD(YELLOW("%*d"))), digitosMAX, jogo->table[i][j].valor); break;
                case 2048: printf(BG_RED(BOLD(YELLOW("%*d"))), digitosMAX, jogo->table[i][j].valor); break;
                default:   printf(BG_MAGENTA(BOLD(BLACK("%*d"))), digitosMAX, jogo->table[i][j].valor); break;
            }

            printf(TAB_VER);
        }
        printf("\n");

        // horizontal
        printf("  ");
        if (i < jogo->tam - 1) {
            printf(TAB_ML);
            for (int j = 0; j < jogo->tam; j++) {
                for (int k = 0; k < digitosMAX; k++)
                    printf(TAB_HOR); 
                if (j < jogo->tam - 1) 
                    printf(TAB_MJ); 
            }
            printf(TAB_MR);
            printf("\n");
        }
    }

    // fundo
    printf(TAB_BL);
    for (int j = 0; j < jogo->tam; j++) {
        for (int k = 0; k < digitosMAX; k++) { printf(TAB_HOR); }
        if (j < jogo->tam - 1) { printf(TAB_BJ); }
    }
    printf(TAB_BR);
    printf("\n");
}
void imprimeRanking(Tops* ranking) {
    for (int i = 0; i < ranking->tam4; i++) {
        printf("%s    -   %d\n", ranking->top10_4[i].nick, ranking->top10_4[i].pts);
    }
    for (int i = 0; i < ranking->tam5; i++) {
        printf("%s    -   %d\n", ranking->top10_5[i].nick, ranking->top10_5[i].pts);
    }
    for (int i = 0; i < ranking->tam6; i++) {
        printf("%s    -   %d\n", ranking->top10_6[i].nick, ranking->top10_6[i].pts);
    }
}
void mostraAjuda(Tabuleiro* jogo) {
    clear();
    char wait[50];
    printf(BOLD("Bem-vindo ao jogo!\n"));
    delay_ms(450);
    printf(BOLD("Para vencer o jogo, deve-se juntar peças com valores iguais até alcançar a peça com valor de " BOLD(YELLOW("2048\n"))));
    delay_ms(450);
    printf(BOLD("Para isso, as peças no tabuleiro podem ser deslocadas por 4 movimentos principais:\n"));
    printf(BOLD(GREEN("(W)")) " Mover para Cima\n");
    printf(BOLD(GREEN("(A)")) " Mover para Esquerda\n");
    printf(BOLD(GREEN("(S)")) " Mover para Baixo\n");
    printf(BOLD(GREEN("(D)")) " Mover para Direita\n");
    printf(BOLD("Aperte qualquer tecla para continuar..."));
    if (!lerEntrada(wait, 50))
        abortar(jogo);
    printf(BOLD("Peças geradas em uma rodada não podem ser utilizadas novamente para se juntarem a outras, e o valor de uma peça gerada é adicionada a pontuação geral\n"));
    delay_ms(450);
    printf(BOLD("* Início do Jogo: ") "Todo novo jogo começa com " BOLD(GREEN("duas")) " peças posicionadas aleatoriamente.\n");
    delay_ms(450);
    printf(BOLD("* Novas Peças (a cada jogada válida):\n"));
    printf(WHITE("  - Tabuleiros 4x4 e 5x5: Adicionam " BOLD(GREEN("uma")) " nova peça (2 ou 4).\n"));
    printf(WHITE("  - Tabuleiro 6x6: Adicionam " BOLD(YELLOW("duas")) " novas peças (2 ou 4).\n"));
    printf(BOLD("Aperte qualquer tecla para continuar..."));
    if (!lerEntrada(wait, 50))
        abortar(jogo);
    printf(BOLD("Existem também algumas") MAGENTA("Fichas Especiais") "que podem auxiliar no caminho para a vitória. São elas:\n");
    delay_ms(450);
    printf(BOLD(RED("\t------- Ficha de Desfazer Jogada (U) -------\n")));
}
// ===========================================================================================================================================================
// FUNÇÕES DE RANKING
// ===========================================================================================================================================================
int carregaRanking(Tops* ranking) {
    FILE *file = NULL;
    file = fopen("ranking.dat", "rb");
    

    if (file == NULL) {
        ranking->tam4 = 0;
        ranking->tam5 = 0;
        ranking->tam6 = 0;
        return 1;
    }
    
    fread(&ranking->tam4, sizeof(int), 1, file);
    fread(&ranking->tam5, sizeof(int), 1, file);
    fread(&ranking->tam6, sizeof(int), 1, file);
    
    if (fread(ranking->top10_4, sizeof(Ranking), ranking->tam4, file) != ranking->tam4) {
        fclose(file);
        return 0;
    }
    if (fread(ranking->top10_5, sizeof(Ranking), ranking->tam5, file) != ranking->tam5) {
        fclose(file);
        return 0;
    }
    if (fread(ranking->top10_6, sizeof(Ranking), ranking->tam6, file)) {
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1;
}
int salvaRanking(Tops* ranking) {
    FILE *file = NULL;
    file = fopen("ranking.dat", "wb");
    if (file == NULL)
        return 0;
    fwrite(&ranking->tam4, sizeof(int), 1, file);
    fwrite(&ranking->tam5, sizeof(int), 1, file);
    fwrite(&ranking->tam6, sizeof(int), 1, file);

    fwrite(ranking->top10_4, sizeof(Ranking), ranking->tam4, file);
    fwrite(ranking->top10_5, sizeof(Ranking), ranking->tam5, file);
    fwrite(ranking->top10_6, sizeof(Ranking), ranking->tam6, file);

    fclose(file);
    return 1;
}
int atualizaRanking(Tops* ranking, Tabuleiro* jogo) {
    if (!carregaRanking(ranking))
        return 0;

    if (jogo->tam == 4) {
        if (ranking->tam4 < 10) {
            strcpy(ranking->top10_4[ranking->tam4].nick, jogo->nick);
            ranking->top10_4[ranking->tam4].pts = jogo->pts;
            ranking->tam4++;
        }
        else {
            if (jogo->pts > ranking->top10_4[9].pts) {
            strcpy(ranking->top10_4[9].nick, jogo->nick);
            ranking->top10_4[9].pts = jogo->pts;
            ordenaRanking(ranking->top10_4, ranking->tam4);
        }
        }
    }
    else if (jogo->tam == 5) {
        if (ranking->tam5 < 10) {
            strcpy(ranking->top10_5[ranking->tam5].nick, jogo->nick);
            ranking->top10_5[ranking->tam5].pts = jogo->pts;
            ranking->tam5++;
        }
        else {
            if (jogo->pts > ranking->top10_5[9].pts) {
            strcpy(ranking->top10_5[9].nick, jogo->nick);
            ranking->top10_5[9].pts = jogo->pts;
            ordenaRanking(ranking->top10_5, ranking->tam5);
        }
        }
    }
    else {
        if (ranking->tam6 < 10) {
            strcpy(ranking->top10_6[ranking->tam6].nick, jogo->nick);
            ranking->top10_6[ranking->tam6].pts = jogo->pts;
            ranking->tam6++;
        }
        else {
            if (jogo->pts > ranking->top10_6[9].pts) {
            strcpy(ranking->top10_6[9].nick, jogo->nick);
            ranking->top10_6[9].pts = jogo->pts;
            ordenaRanking(ranking->top10_6, ranking->tam6);
        }
        }
    }

    if (!salvaRanking(ranking))
        return 0;
    return 1;
}
void ordenaRanking(Ranking* rank, int n) {
    for (int i = 0; i < n-1; i++) {
        for (int k = 0; k < n-1; k++) {
            if (rank[k].pts < rank[k+1].pts) {
                Ranking temp = rank[k];
                rank[k] = rank[k+1];
                rank[k+1] = temp;
            }
        }
    }
}