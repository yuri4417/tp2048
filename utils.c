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
    #ifdef _WIN32
        Sleep(ms);
    #else
        usleep(ms * 1000);
    #endif
}
void clear() {
    printf("\033[2J\033[H");
}
void zeraGuloso(Tabuleiro* jogo) {
	for (int i = 0; i < jogo->tam; i++) 
		for (int j = 0; j < jogo->tam; j++) 
			jogo->table[i][j].nao_guloso = 0;
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
        for (int j = 0; j < jogo->tam; j++) {
            if (jogo->table[i][j].valor == 0)
                return 0; //Célula com pelo menos 1 valor 0
            if (i < jogo->tam - 1 && jogo->table[i][j].valor == jogo->table[i+1][j].valor)
                    return 0; //Célula abaixo igual a atual, movimento possível
            if (j < jogo->tam - 1 && jogo->table[i][j].valor == jogo->table[i][j+1].valor)
                    return 0; //Célula a direita igual     
        }
    return 1;
}
int trataVitória(Tabuleiro* jogo, Tops* ranking) {
    jogo->verificado = 1;
    clear();
    imprimeTabela(jogo);
    scoreRodada(jogo);

    printf(BOLD(GREEN("\t\t\t\t\tParabéns! Você VENCEU!\n")));
    if (perguntaSN("Deseja continuar jogando?\n-> ")) {
        printf(BOLD(GREEN("Continuando o jogo...\n")));
        return 1;
    } 
    else {
        if (perguntaSN(BOLD(GREEN("Deseja salvar sua pontuação no ranking?")))) 
            atualizaRanking(ranking, jogo);     
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
                desfazMovimento(jogo);
                jogo->numUndo--;
                return 1;
            } 
            else {
                if (perguntaSN(BOLD(GREEN("Deseja salvar sua pontuação no ranking?")))) 
                    atualizaRanking(ranking, jogo);
                printf(BOLD(YELLOW("Voltando ao menu...\n")));
                delay_ms(450);
                return 0;
                }
    } 
    else {
        char opt[10];
        printf(BOLD(RED("Você PERDEU!\n")));
        printf(BOLD(RED("Sem movimentos de desfazer disponíveis.\n")));
        if (perguntaSN(BOLD(GREEN("Deseja salvar sua pontuação no ranking?")))) 
            atualizaRanking(ranking, jogo);
        printf("Aperte qualquer tecla para retornar ao menu...\n->");
        lerEntrada(opt, 10);
        return 0;
    }
}
int carregaJogo(Tabuleiro* jogo) {
    char filename[80];
    clear();
    printf("Digite o nome do arquivo de save a ser carregado: ");
    fgets(filename, 80, stdin);

    int len = strcspn(filename, "\n"); 
    filename[len] = '\0';
    if (len == strlen(filename)-1)
        limpar_buffer(); // Limpa se digitar mais que a string
    catTxt(filename, 80);

    FILE *save = NULL;
    save = fopen(filename, "r");
    if (save == NULL) 
        return 0;

    if (jogo->table != NULL) {
        liberaMatriz(jogo->table, jogo->tam);
        liberaMatriz(jogo->table_bkp, jogo->tam);
    }

    fscanf(save, "%d", &jogo->tam);
    jogo->table = criaMatriz(jogo->tam);
    jogo->table_bkp = criaMatriz(jogo->tam); 

    if (jogo->table == NULL || jogo->table_bkp == NULL) {
        fclose(save);
        return 0;
    }

    fscanf(save, "%d", &jogo->numUndo);
    fscanf(save, "%d", &jogo->numTroca);

    fscanf(save, "%d", &jogo->pts);
    fgetc(save); //consome espaço entre pts e nome de jogador
    fgets(jogo->nick, 27, save);
    jogo->nick[strcspn(jogo->nick, "\n")] = '\0';
    
    for (int i = 0; i < jogo->tam; i++)
        for (int j = 0; j < jogo->tam; j++) {
            fscanf(save, "%d", &jogo->table[i][j].valor);
            if (jogo->table[i][j].valor >= 2048)
                jogo->verificado = 1; //Se já ganhou, não precisa da mensagem de vitória novamente
        }


    for (int i = 0; i < jogo->tam; i++)
        for (int j = 0; j < jogo->tam; j++) {
            fscanf(save, "%d", &jogo->table_bkp[i][j].valor);
            if (jogo->table_bkp[i][j].valor >= 2048)
                jogo->verificado = 1; //Se já ganhou, não precisa da mensagem de vitória novamente
        }
            

    //Duplica pontos e fichas para caso carregar e desfazer logo em seguida, evitando que lixo seja utilizado
    jogo->pts_bkp = jogo->pts; 
    jogo->numUndo_bkp = jogo->numUndo; 
    jogo->numTroca_bkp = jogo->numTroca;
    fclose(save);
    return 1;
}
int salvaJogo(Tabuleiro* jogo) {
    char filename[80];
    FILE *toSave = NULL;

    if (jogo->table == NULL || jogo->unsaved == 0)
        return 0;
    printf("Digite o nome do arquivo para salvamento: ");
    fgets(filename, 80, stdin);


    int len = strcspn(filename, "\n"); 
    filename[len] = '\0';
    if (len == strlen(filename)-1)
        limpar_buffer(); // Limpa se digitar mais que a string

    catTxt(filename, 80);


    toSave = fopen(filename, "w");
    if (toSave == NULL)
        return 0;

    
    fprintf(toSave, "%d ", jogo->tam);
    fprintf(toSave, "%d ", jogo->numUndo);
    fprintf(toSave, "%d\n", jogo->numTroca);

    fprintf(toSave, "%d ", jogo->pts);
    fprintf(toSave, "%s\n", jogo->nick);

    for (int i = 0; i < jogo->tam; i++) {
        for (int j = 0; j < jogo->tam; j++) 
                fprintf(toSave, "%d ", jogo->table[i][j].valor);
        fprintf(toSave, "\n");
    }


    for (int i = 0; i < jogo->tam; i++) {
        for (int j = 0; j < jogo->tam; j++)
                fprintf(toSave, "%d ", jogo->table_bkp[i][j].valor);
        fprintf(toSave, "\n");
    }

    jogo->unsaved = 0;
    fclose(toSave);
    return 1;
}

// ===========================================================================================================================================================
// MANIPULAÇÃO DE STRINGS
// ===========================================================================================================================================================
int lerEntrada(char* str, int tam) {
    if (fgets(str, tam, stdin) == NULL) // EOF
        return 0;
    
    int len = strcspn(str, "\n");
    if (len < tam-1)
        str[len] = '\0';
    else {
        str[tam-1] = '\0';
        limpar_buffer(); // Limpa se digitar mais que a string
    }

    int i = 0;
    while (str[i] != '\0') {
        str[i] = converteMinuscula(str[i]);
        i++;
    }
    return 1;
}
char converteMinuscula(char letra) {
    if (letra >= 65 && letra <= 90)
        letra += 32;    
    return letra;
}
int perguntaSN(char* str) {
    char resp[16];
    printf("%s", str);
    while (1) {
        if (!lerEntrada(resp, 16)) return 0; // EOF == 'n'

        apagaEspaço(resp);

        if (strlen(resp) == 1) {
            char opt = converteMinuscula(resp[0]);
            if (opt == 's') return 1;
            if (opt == 'n') return 0;
        }

        printf(BOLD(RED("Opção Inválida! Tente novamente (Responda com S/N)\n")));
        delay_ms(500);
    }
}
void catTxt(char *str, int tam) {
    char *ultimoPonto = strrchr(str, '.'); // Encontra o último ponto. NULL = Não há ponto
    if (ultimoPonto == NULL || strcmp(ultimoPonto+1, "txt") != 0) { // Se não há ponto OU o que estiver depois dele for diferente de txt...
        if (strlen(str) + 4 < tam) { // Se cabe o txt na string
            if (ultimoPonto == NULL) // Se não há o ponto, então junte str a ".txt"
                strcat(str, ".txt");
            else // Se tem ponto, então o que vem depois não é txt. Sobrescreva txt a partir do último ponto para ".txt"
                strcpy(ultimoPonto, ".txt");
        }
    }
}
int arrumaString(char *str) {
    int i = 1; // ignora o 't'

    if (str[1] != 32) { // E deve ser seguido por um espaço
        return 0;
    }

    while (str[i] != '\0') {
        if (str[i] != 32) {  
            if (isalpha(str[i])) { // Se for uma letra....
                if (str[i+1] == '\0' || !isdigit(str[i+1])) // e o proximo  for o fim da string ou não for um digito, é invalido
                    return 0;
            } else if (isdigit(str[i])) { // Se for um número...
                if (str[i+1] != '\0' && str[i+1] != 32) // E o proximo não é nem o fim da string nem um espaço, é invalido 
                    return 0;
            } else {
                return 0; // caractere qualquer
            }
        }
        i++;
    }

    apagaEspaço(str);
    i = 0;

    if (strlen(str) != 5) // idealmente, a string deveria estar no formato tCXCX\0
        return 0;

    return 1; // string está no formato tCXCX\0 (C=char, X = nro), validação feita na função de troca
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

// ===========================================================================================================================================================
// IMPRESSÃO EM TELA
// ===========================================================================================================================================================
void imprimeTabela(Tabuleiro* jogo) {
    int digitosMAX = 8;


    // borda superior
    for (int i = 0; i < jogo->tam; i++)
        printf(BOLD("%*d"), digitosMAX+1, 1+i);
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
        printf(BOLD("%c"), 'A'+i);
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
        for (int k = 0; k < digitosMAX; k++) 
            printf(TAB_HOR);
        if (j < jogo->tam - 1) 
            printf(TAB_BJ);
    }
    printf(TAB_BR);
    printf("\n");
}
void imprimeRanking(Tops* ranking, Tabuleiro* jogo) {
    int tam;
    char resp[50];
    while (1) {
        clear();
        tam = 0;
        printf(CYAN("\t(4) ")BOLD(GREEN("Ranking jogos 4x4"))"\n");
        printf(CYAN("\t(5) ")BOLD(YELLOW("Ranking jogos 5x5"))"\n");
        printf(CYAN("\t(6) ")BOLD(RED("Ranking jogos 6x6"))"\n");
        printf(BOLD("Deseja acessar qual ranking?\n->"));

        while (tam == 0) {
            if (lerEntrada(resp, 50)) {
                apagaEspaço(resp);
                if (strlen(resp) == 1 && resp[0] >= '4' && resp[0] <= '6')
                    tam = resp[0] - '0';
                else
                    printf(BOLD(RED("Tamanho inválido! Escolha entre 4, 5, ou 6\n")) "->");
            }
            else
                abortar(jogo);
        }
        if(!carregaRanking(ranking)) {
            printf(BOLD(RED("ERRO AO CARREGAR O RANKING. Tente novamente\n")));
            delay_ms(850);
            return;
        }

        switch (tam) {
            case 4:
                printf(BOLD(CYAN("Nome  ")BOLD("----------------------------    ") BOLD(GREEN("PTS\n"))));
                for (int i = 0; i < ranking->tam4; i++) 
                    printf(BOLD(CYAN("%-27s")"   -    "BOLD(GREEN("%09d\n"))), ranking->top10_4[i].nick, ranking->top10_4[i].pts);
                break;
            case 5:
                printf(BOLD(CYAN("Nome  ")BOLD("----------------------------    ") BOLD(GREEN("PTS\n"))));
                for (int i = 0; i < ranking->tam5; i++) 
                    printf(BOLD(CYAN("%-27s")"   -    "BOLD(GREEN("%09d\n"))), ranking->top10_5[i].nick, ranking->top10_5[i].pts);
                break;
            case 6:
                printf(BOLD(CYAN("Nome  ")BOLD("----------------------------    ") BOLD(GREEN("PTS\n"))));
                for (int i = 0; i < ranking->tam6; i++) 
                    printf(BOLD(CYAN("%-27s")"   -    "BOLD(GREEN("%09d\n"))), ranking->top10_6[i].nick, ranking->top10_6[i].pts);
                break;
        }
        if (!perguntaSN(BOLD("Deseja visualizar outro ranking?\n->")))
            break;   
    }
}
void mostraAjuda(Tabuleiro* jogo) {
    clear();
    char wait[5];
    printf(BOLD("Bem-vindo ao jogo!\n"));
    delay_ms(450);
    printf(BOLD("Para vencer, você deve juntar peças com valores iguais até alcançar a peça com valor de " BOLD(YELLOW("2048.\n"))));
    delay_ms(450);
    printf(BOLD("Para isso, as peças no tabuleiro podem ser deslocadas por 4 movimentos principais:\n"));
    printf(BOLD(GREEN("(W)")) BOLD(" Mover para Cima\n"));
    printf(BOLD(GREEN("(A)")) BOLD(" Mover para Esquerda\n"));
    printf(BOLD(GREEN("(S)")) BOLD(" Mover para Baixo\n"));
    printf(BOLD(GREEN("(D)")) BOLD(" Mover para Direita\n"));
    printf(BOLD("Aperte qualquer tecla para continuar..."));
    if (!lerEntrada(wait, 5))
        abortar(jogo);
    printf(BOLD("Aqui, existe a regra de ") BOLD(GREEN("não guloso:\n")));
    printf(BOLD("\t* Uma peça gerada em uma rodada ") BOLD(RED("não pode ser utilizada para se fundir com outra peça nessa mesma rodada.\n\n")));
    delay_ms(450);
    printf(BOLD("* Início do Jogo: ") "Todo novo jogo começa com " BOLD(GREEN("duas")) " peças posicionadas aleatoriamente.\n");
    delay_ms(450);
    printf(BOLD("* Novas Peças " BOLD(BLUE("(a cada jogada válida):\n"))));
    printf(BOLD("  - Tabuleiros 4x4 e 5x5: Adicionam " BOLD(GREEN("uma")) BOLD(" nova peça (2 ou 4).\n")));
    printf(BOLD("  - Tabuleiro 6x6: Adicionam " BOLD(YELLOW("duas")) BOLD   (" novas peças (2 ou 4).\n")));
    printf(BOLD("Aperte qualquer tecla para continuar..."));
    if (!lerEntrada(wait, 5))
        abortar(jogo);
    printf(BOLD("Existem também algumas") BOLD(MAGENTA(" Fichas Especiais ")) "que podem auxiliar no caminho para a vitória. São elas:\n\n");
    delay_ms(450);
    printf(BOLD(MAGENTA("\t------- Ficha de Desfazer Jogada (U) -------\n")));
    delay_ms(400);
    printf(BOLD("Toda vez que você criar uma nova peça de 256, você ganha uma ficha que permite" BOLD(MAGENTA(" retornar o estado do jogo para uma rodada atrás.\n"))));
    delay_ms(400);
    printf(BOLD("Com isso, você pode refazer jogadas que acabaram se provando ruins.\n"));
    delay_ms(400);
    printf(BOLD("Para desfazer sua jogada, basta apertar" BOLD(GREEN(" (U) ")) BOLD("dentro do jogo e, se você tiver alguma ficha, o jogo será restaurado!\n")));
    delay_ms(400);
    printf(BOLD(CYAN("\t------- Ficha de Trocar Peças (T) -------\n")));
    delay_ms(400);
    printf(BOLD("Toda vez que uma peça de valor 512 é gerada, você ganha uma ficha para ") BOLD(CYAN("trocar duas peças entre si!\n")));
    printf(BOLD(YELLOW("MAS ATENÇÃO: VOCÊ SÓ PODE USAR ESSE COMANDO SE:\n")));
    delay_ms(200);
    printf(BOLD(YELLOW("\t - Tiver uma ficha de troca disponível;\n")));
    delay_ms(200);
    printf(BOLD(YELLOW("\t - As duas posições possuírem alguma peça;\n")));
    delay_ms(200);
    printf(BOLD(YELLOW("\t - O comando estar formatado do seguinte jeito: t pos1 pos2.\n")));
    printf(BOLD("Aperte qualquer tecla para continuar..."));
    if (!lerEntrada(wait, 5))
        abortar(jogo); 
    printf(BOLD("Você também pode retornar ao menu utilizando o comando") BOLD(RED(" (voltar)")) BOLD(". Mas não se esqueça de salvar o jogo depois!\n\n"));
    delay_ms(400);
    printf(BOLD("\t ---------- Alguns comentários adicionais -------------------\n"));
    printf(BOLD("PONTUAÇÃO: O valor de uma peça gerada é somada a pontuação.\n"));
    printf(BOLD("\t* Peças novas só são geradas após um ") BOLD(BLUE("movimento válido (que altera o tabuleiro), "))BOLD("e se houver um espaço livre disponível.\n"));
    delay_ms(400);
    printf(BOLD(GREEN("* Regra de prioridade de movimento:\n")));
    delay_ms(200);
    printf(BOLD("Se determinada linha for semelhante a ") BOLD(GREEN("[ ][2][2][2] ")) BOLD("e você decidir fazer um movimento para a ") BOLD(GREEN("direita...\n")));
    delay_ms(200);
    printf(BOLD("O resultado final seria") BOLD(GREEN(" [ ][ ][2][4]")) BOLD(", pois as junções mais a ") BOLD(GREEN("direita")) BOLD(" tem prioridade, neste caso.\n"));
    delay_ms(1000);
    printf(BOLD("Esse é o fim do tutorial! Se sente pronto para jogar? Você pode voltar aqui sempre que quiser relembrar alguma regra/comando.") BOLD(GREEN(" Boa sorte!\n"))); 
    delay_ms(600);
    printf(BOLD("Aperte qualquer tecla para voltar ao menu..."));
    if (!lerEntrada(wait, 5))
        abortar(jogo);
    printf(BOLD(GREEN("Retornando ao menu....\n")));
    delay_ms(650);  
}
void scoreRodada(Tabuleiro* jogo) {
    printf(BOLD(BLUE("=====================================")) "\n");
    printf(BOLD(YELLOW("- Jogador:")) BOLD(" \n\t%s\n"), jogo->nick);
    printf(BOLD(GREEN("- Pontuação:")) BOLD(GREEN(" \n\t%d\n\n")), jogo->pts);
    printf(BOLD(MAGENTA("* Fichas de Desfazer Jogada:") " %d\n"), jogo->numUndo);
    printf(BOLD(CYAN("* Fichas de troca de peças:") " %d\n"), jogo->numTroca);
    printf(BOLD(BLUE("=====================================")) "\n");
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
    
    if (fread(&ranking->top10_4, sizeof(Ranking), ranking->tam4, file) != ranking->tam4) {
        fclose(file);
        return 0;
    }
    if (fread(&ranking->top10_5, sizeof(Ranking), ranking->tam5, file) != ranking->tam5) {
        fclose(file);
        return 0;
    }
    if (fread(&ranking->top10_6, sizeof(Ranking), ranking->tam6, file) != ranking->tam6) {
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
void atualizaRanking(Tops* ranking, Tabuleiro* jogo) {
    if (!carregaRanking(ranking)) {
        printf(BOLD("ERRO: Falha ao interagir com o arquivo de ranking\n"));
        delay_ms(750);
        return;
    }

    if (jogo->tam == 4) {
        if (ranking->tam4 < 10) {
            strcpy(ranking->top10_4[ranking->tam4].nick, jogo->nick);
            ranking->top10_4[ranking->tam4].pts = jogo->pts;
            ranking->tam4++;
            ordenaRanking(ranking->top10_4, ranking->tam4);
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
            ordenaRanking(ranking->top10_5, ranking->tam5);
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
            ordenaRanking(ranking->top10_6, ranking->tam6);;
        }
        else {
            if (jogo->pts > ranking->top10_6[9].pts) {
                strcpy(ranking->top10_6[9].nick, jogo->nick);
                ranking->top10_6[9].pts = jogo->pts;
                ordenaRanking(ranking->top10_6, ranking->tam6);
            }
        }
    }

    if (!salvaRanking(ranking)) {
        printf(BOLD("ERRO: Falha ao interagir com o arquivo de ranking\n"));
        delay_ms(750);
        return;
    }
    printf(BOLD(GREEN("Ranking Atualizado!\n")));
    delay_ms(750);
}