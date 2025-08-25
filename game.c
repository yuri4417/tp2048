#include "game.h"

#include "utils.h"    
#include "cores.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



// ===========================================================================================================================================================
// NÚCLEO DE JOGO
// ===========================================================================================================================================================
void mostraMenu(Tabuleiro* jogo, Tops* ranking) {
	char opt = '\0';
	char frase[20];
	do {
		clear();
		printf(BOLD(CYAN("\t\t\tBem-vindo!\n\n")));
		for (int i = 0; i < 3; i++)
			printf("\n");
		printf(BOLD(GREEN("(N) "))BOLD(WHITE("Iniciar Novo Jogo\n")));
		printf(BOLD(GREEN("(J) "))BOLD(WHITE("Continuar um jogo atual\n\n")));
		printf(BOLD(YELLOW("(C) "))BOLD(WHITE("Carregar um Jogo Salvo\n")));
		printf(BOLD(YELLOW("(S) "))BOLD(WHITE("Salvar o Jogo Atual\n\n")));
		printf(BOLD(BLUE("(M) "))BOLD(WHITE("Mostrar TOP 10 pontuações\n")));
		printf(BOLD(BLUE("(A) "))BOLD(WHITE("Como jogar? \n\n")));
		printf(BOLD(RED("(R) "))BOLD(WHITE("Sair \n\n")));
		printf("O que deseja fazer?\n");
		printf("-> "); 
		if (!lerEntrada(frase, 20))
			abortar(jogo);
		apagaEspaço(frase);

		if (strlen(frase) == 1)
			opt = converteMinuscula(frase[0]);
		else
			opt = '\0';

		switch (opt) {
			case 'n': {
				int start = 0;
				if (!jogo->unsaved) 
					start = 1;
				else {
					if (perguntaSN(BOLD(YELLOW("\nAVISO: Jogo não salvo detectado. Deseja continuar e descartá-lo? (S/N)\n-> "))))
						start = 1;
					else {
						printf(BOLD(YELLOW("Retornando ao menu...\n")));
						delay_ms(600);
					}	
				}
				if (start) {
					printf(BOLD(GREEN("Iniciando novo jogo... Divirta-se!")) "\n");
					delay_ms(850);
					novoJogo(jogo, ranking);
				}	
			}
					break;
			case 'j':
					if (jogo->table != NULL) {
						gameLoop(jogo, ranking);
					}
					else {
						printf(BOLD(RED("ERRO: Nenhum jogo em andamento.")" Carregue um arquivo de jogo ou inicie uma nova partida\n"));
						delay_ms(850);
					}
					break;
			case 'c':
					if (carregaJogo(jogo)) {
						printf(BOLD("Jogo carregado!" BOLD(GREEN(" Carregando tabela...\n"))));
						delay_ms(500);
						gameLoop(jogo, ranking);
					}
					else {
						printf(BOLD(RED("Erro ao carregar jogo.")) BOLD("Tente novamente ou inicie uma nova partida\n"));
						delay_ms(850);
					}
					break;
			case 's':
					if(salvaJogo(jogo))
						printf(BOLD(GREEN("Jogo salvo com sucesso!\n")));
					else 
						printf(BOLD(RED("Erro: Não há nada para ser salvo/Falha em salvar o jogo. Tente novamente\n")));
					delay_ms(850);
					break; 
			case 'm':
					imprimeRanking(ranking, jogo);
					break;
			case 'a': 
					mostraAjuda(jogo);
					break;
			case 'r': {
					if (perguntaSN(BOLD(RED("Deseja realmente sair? ")"(" BOLD(RED("S")) "/" BOLD(GREEN("N")))")\n->" )) {
						printf(BOLD(RED("Saindo...\n")));
						delay_ms(550);
					}
					else {
						printf(BOLD(GREEN("Retornando ao menu...\n")));
						opt = '\0';
						delay_ms(650);
					}
			}		
					break;
			default: 
					printf(BOLD(RED("Opção inválida! Escolha novamente.")) "\n");
					delay_ms(850);
		} 
	} while (opt != 'r');
}
void novoJogo(Tabuleiro* jogo, Tops* ranking) {
	char resp[30];
	if (jogo->table != NULL) {
		liberaMatriz(jogo->table, jogo->tam);
		liberaMatriz(jogo->table_bkp, jogo->tam);
	}
	clear();
	printf("===== Tamanhos disponíveis =====\n");
	printf(CYAN("\t(4) ")BOLD(GREEN("Pequeno"))"  (4x4) \n");
	printf(CYAN("\t(5) ")BOLD(YELLOW("Normal"))"  (5x5) \n");
	printf(CYAN("\t(6) ")BOLD(RED("Grande"))"  (6x6) \n");
	printf("Escolha o tamanho do jogo: ");
	jogo->tam = 0;
	while (jogo->tam == 0) {
    	if (lerEntrada(resp, 30)) {
    		apagaEspaço(resp);
    	    if (strlen(resp) == 1 && resp[0] >= '4' && resp[0] <= '6')
    	        jogo->tam = resp[0] - '0';
    	    else
    	        printf(BOLD(RED("Tamanho inválido! Escolha entre 4, 5, ou 6\n")) "->");
    	}
    	else
    	    abortar(jogo);
	}
	printf("Digite seu nickname: ");
	fgets(jogo->nick, 27, stdin);
	if (strchr(jogo->nick, '\n') == NULL) // Limpa se digitar mais que a string
        limpar_buffer();
    int len = strcspn(jogo->nick, "\n");
    if (len < 26) {
    	jogo->nick[len] = '\0';
    }

	//Criação e verificação das matrizes
	jogo->table = criaMatriz(jogo->tam);
	jogo->table_bkp = criaMatriz(jogo->tam);
	if (jogo->table == NULL || jogo->table_bkp == NULL) {
		printf(BOLD(RED("ERRO: Falha ao iniciar o jogo. Tente novamente.\n")));
		delay_ms(2000);
		return;
	}
	//Flags e fichas
	jogo->verificado = 0;
	jogo->unsaved = 0;
	jogo->pts = 0;
	jogo->numUndo = 0;
	jogo->numTroca = 0;

	// Peças iniciais
	if (jogo->tam == 6)
		geradorRand(jogo);
	else {
		geradorRand(jogo);
		geradorRand(jogo);
	}
	gameLoop(jogo, ranking);
}
void gameLoop(Tabuleiro* jogo, Tops* ranking) {
	char resp[55];
	do {
		clear();
		imprimeTabela(jogo);
		scoreRodada(jogo);
		printf(BOLD("\nComando: Movimento ") BOLD(GREEN("(W, A, S ou D)"))"," BOLD(" Desfazer Movimento ") BOLD(MAGENTA("(U)"))", " BOLD("Trocar peças") BOLD(CYAN(" (T)")) BOLD(" ou retornar ao menu ") BOLD(RED("(voltar)"))BOLD(":\n-> "));
		if (!lerEntrada(resp, 55)) 
			abortar(jogo);
		if (resp[0] != 't') 
			apagaEspaço(resp);


		int i = 0;
		while (resp[i] != '\0') {
			resp[i] = converteMinuscula(resp[i]);
			i++;
		}
		if (strcmp(resp, "voltar") == 0) 
			break;

		processaComando(jogo, resp);

		if (!jogo->verificado && verificaVitória(jogo)) {
			if (!trataVitória(jogo, ranking))
				break;
		}
		else if (verificaDerrota(jogo)) {
			if (!trataDerrota(jogo, ranking))
				break;
		}
	} while (1);
}
void processaComando(Tabuleiro* jogo, char* opt) {
	int len = strlen(opt);
	int tabuleiro_mudou = 0;
	if (len == 0)
		return;
	
	if (converteMinuscula(opt[0]) == 't') {
		if(arrumaString(opt)) {
			fazBackup(jogo);
			trocaPos(jogo, opt);	
		}
		else {
			printf(BOLD(RED("Comando inválido! Tente novamente.\n")));
			delay_ms(750);
		}
	}
	else if (strlen(opt) == 1) {
	char cmd = converteMinuscula(opt[0]);
		switch (cmd) {
			case 'a':
			case 'd':
			case 's':
			case 'w':
					fazBackup(jogo);
					if (cmd == 'a') moveEsquerda(jogo);
					else if (cmd == 'd') moveDireita(jogo);
					else if (cmd == 's') moveBaixo(jogo);
					else  moveCima(jogo); 
					
					if (mudanca(jogo)) {
						tabuleiro_mudou = 1;
					}
					else {
						printf(BOLD(YELLOW("Movimento inválido! O jogo continua igual\n")));
						delay_ms(800);
					}
				break;
			case 'u':
					if (jogo->numUndo > 0) {
						desfazMovimento(jogo);
						jogo->numUndo--;
					}
					else {
						printf(BOLD(YELLOW("SEM FICHAS DE DESFAZER JOGADA DISPONÍVEIS. Obtenha mais formando peças de 256\n")));
						delay_ms(800);
					}
				break;
			default:
				printf(BOLD(RED("Comando inválido! Tente novamente.\n")));
				delay_ms(750);
		}
	}
	else {
		printf(BOLD(RED("Comando inválido! Tente novamente.\n")));
		delay_ms(750);
	}

	if (tabuleiro_mudou) {
		geradorRand(jogo);
		jogo->unsaved = 1;
	}
}
void geradorRand(Tabuleiro* jogo) {
    int toAdd = 1; // qtd de peças para serem geradas
    int prob4 = 10; // chance do número gerado ser 4
    if (jogo->tam == 5)
        prob4 = 15;
    else if (jogo->tam == 6) { 
        toAdd = 2;
        prob4 = 20;
    }

    int tam2 = jogo->tam*jogo->tam;

    int **espacoLimpo = malloc(tam2*sizeof(int*));
    if (espacoLimpo == NULL) {
        printf(BOLD(RED("ERRO NA VERIFICAÇÃO DE SEGURANÇA. Tente novamente")) "\n");
        return; 
    }
    for (int i = 0; i < tam2; i++) {
        espacoLimpo[i] = malloc(2*sizeof(int));
        if (espacoLimpo[i] == NULL) {
            printf(BOLD(RED("ERRO NA VERIFICAÇÃO DE SEGURANÇA. Tente novamente")) "\n");
            for (int z = 0; z < i; z++)
                free(espacoLimpo[z]);
            free(espacoLimpo);
            return;
        }
    }

    int qtdVazias = 0;
    for (int i = 0; i < jogo->tam; i++) // Mapeamento de quais posições da tabela estão vazias
        for (int j = 0; j < jogo->tam; j++) 
            if (jogo->table[i][j].valor == 0) {
                espacoLimpo[qtdVazias][0] = i; 
                espacoLimpo[qtdVazias][1] = j; 
                qtdVazias++;
            }
    if (qtdVazias == 0) { // Não há o que gerar, encerra a função
        for (int i = 0; i < tam2; i++)
            free(espacoLimpo[i]);
        free(espacoLimpo);
        return;
    }
    if (toAdd > qtdVazias) 
        toAdd = qtdVazias; // 1 espaço livre, gera apenas uma peça

    for (int i = 0; i < toAdd; i++) { // sorteio da posição p preencher
        int temp = rand() % qtdVazias; // 0 - qtdvazias - 1
        int j = espacoLimpo[temp][0];
        int k = espacoLimpo[temp][1];

        if ((rand() % 100) < prob4) // Sorteio para ver se o número gerado é 2 ou 4
            jogo->table[j][k].valor = 4;
        else
            jogo->table[j][k].valor = 2;

        //6x6 - remove o primeiro espaço preenchido e troca pelo último
        espacoLimpo[temp][0] = espacoLimpo[qtdVazias - 1][0];
        espacoLimpo[temp][1] = espacoLimpo[qtdVazias - 1][1];
        qtdVazias--;
    }
    for (int i = 0; i < tam2; i++)
        free(espacoLimpo[i]);
    free(espacoLimpo);
}
// ===========================================================================================================================================================
// MATRIZES
// ===========================================================================================================================================================

Célula **criaMatriz(int n) {
	Célula **matriz;
	matriz = malloc(n*sizeof(Célula*));
	if (matriz == NULL) return NULL;
	for (int i = 0; i < n; i++) {
		matriz[i] = malloc(n*sizeof(Célula));
		if (matriz[i] == NULL) {
			for (int z = 0; z < i; z++) {
				free(matriz[z]);
			}
			free(matriz);
			return NULL;
		}
		for (int j = 0; j < n; j++) {
			matriz[i][j].valor = 0;
			matriz[i][j].nao_guloso = 0;
		}
	}
	return matriz;
}
void liberaMatriz(Célula** matriz, int n) {
	for (int i = 0; i < n; i++)
		free(matriz[i]);
	free(matriz);
}
void moveEsquerda(Tabuleiro* jogo) {
	zeraGuloso(jogo);
	int espacovazio;

	for (int i = 0; i < jogo->tam; i++) {
		
		// COPIA NÚMEROS PRA MÁXIMA ESQUERDA ==========================================================================================
		espacovazio = 0;
		for (int j = 0; j < jogo->tam; j++) {
			if (jogo->table[i][j].valor != 0) {
				jogo->table[i][espacovazio] = jogo->table[i][j];
				espacovazio++;
			}
		}
		// LIMPAR NÚMEROS JÁ COPIADOS
		for (int l = espacovazio; l < jogo->tam; l++) {
			jogo->table[i][l].valor = 0;
			jogo->table[i][l].nao_guloso = 0;
		}

		// FUNDIR NÚMEROS ============================================================================================================== 
		for (int f = 0; f < jogo->tam - 1; f++) {
			if (jogo->table[i][f].valor != 0 && jogo->table[i][f].valor == jogo->table[i][f+1].valor &&
				jogo->table[i][f].nao_guloso == 0 && jogo->table[i][f+1].nao_guloso == 0) {

				jogo->table[i][f].valor *= 2;
				jogo->table[i][f].nao_guloso = 1;
				if (jogo->table[i][f].valor == 256) {
					jogo->numUndo++;
					printf(BOLD(GREEN("Ficha de Desfazer Jogada obtida!\n")));
					delay_ms(600);
				}
				if (jogo->table[i][f].valor == 512) {
					jogo->numTroca++;
					printf(BOLD(GREEN("Ficha de Trocar peças obtida!\n")));
					delay_ms(600);
				}
				jogo->pts += jogo->table[i][f].valor;

				jogo->table[i][f+1].valor = 0;
		
			}
		}

		// COPIA E LIMPA NOVAMENTE ======================================================================================================
		espacovazio = 0;
		for (int j = 0; j < jogo->tam; j++) {
			if (jogo->table[i][j].valor != 0) {
				jogo->table[i][espacovazio] = jogo->table[i][j];
				espacovazio++;
			}
		}
		for (int l = espacovazio; l < jogo->tam; l++) {
			jogo->table[i][l].valor = 0;
			jogo->table[i][l].nao_guloso = 0;
		}
	}
}
void moveDireita(Tabuleiro* jogo) {
	zeraGuloso(jogo);
	int espacovazio;

	for (int i = 0; i < jogo->tam; i++) {

		// COPIAR NÚMEROS PRA MÁXIMA DIREITA ==========================================================================================
		espacovazio = jogo->tam - 1;
		for (int j = jogo->tam - 1; j >= 0; j--) {
			if (jogo->table[i][j].valor != 0) {
				jogo->table[i][espacovazio] = jogo->table[i][j];
				espacovazio--;
			}
		}
		// LIMPA NÚMEROS COPIADOS ====================================================================================================
		for (int l = espacovazio; l >= 0; l--) {
			jogo->table[i][l].valor = 0;
			jogo->table[i][l].nao_guloso = 0;
		}

		// FUNDIR NÚMEROS ============================================================================================================
		for (int f = jogo->tam - 1; f >= 1; f--) {
			if (jogo->table[i][f].valor != 0 && jogo->table[i][f].valor == jogo->table[i][f-1].valor && 
				jogo->table[i][f].nao_guloso == 0 && jogo->table[i][f-1].nao_guloso == 0) {

				jogo->table[i][f].valor *= 2;
				jogo->table[i][f].nao_guloso = 1;
				jogo->pts += jogo->table[i][f].valor;
				if (jogo->table[i][f].valor == 256) {
					jogo->numUndo++;
					printf(BOLD(GREEN("Ficha de Desfazer Jogada obtida!\n")));
					delay_ms(600);
				}
				if (jogo->table[i][f].valor == 512) {
					jogo->numTroca++;
					printf(BOLD(GREEN("Ficha de Trocar Peças obtida!\n")));
					delay_ms(600);
				}
				
				jogo->table[i][f-1].valor = 0;
			}
		}

		// DESLIZA E LIMPA NOVAMENTE ==================================================================================================
		espacovazio = jogo->tam - 1;
		for (int j = jogo->tam - 1; j >= 0; j--) {
			if (jogo->table[i][j].valor != 0) {
				jogo->table[i][espacovazio] = jogo->table[i][j];
				espacovazio--;
			}
		}
		for (int l = espacovazio; l >= 0; l--) {
			jogo->table[i][l].valor = 0;
			jogo->table[i][l].nao_guloso = 0;
		}
	}
}
void moveCima(Tabuleiro* jogo) {
	zeraGuloso(jogo);
	int espacovazio;

	
	for (int i = 0; i < jogo->tam; i++) { //colunas
		
		// COPIA E PUXA PEÇAS PRA CIMA ================================================================================================
		espacovazio = 0; 
		for (int j = 0; j < jogo->tam; j++) {
			if (jogo->table[j][i].valor != 0) {
				jogo->table[espacovazio][i] = jogo->table[j][i];
				espacovazio++;
			}
		}
		// LIMPA PEÇAS DO FUNDO QUE FORAM COPIADAS ====================================================================================
		for (int l = espacovazio; l < jogo->tam; l++) {
			jogo->table[l][i].valor = 0;
			jogo->table[l][i].nao_guloso = 0;
		}

		// FUSÃO DOS NÚMEROS ==========================================================================================================
		for (int f = 0; f < jogo->tam - 1; f++) {
			if (jogo->table[f][i].valor != 0 && jogo->table[f][i].valor == jogo->table[f+1][i].valor &&
				jogo->table[f][i].nao_guloso == 0 && jogo->table[f+1][i].nao_guloso == 0) {

				jogo->table[f][i].nao_guloso = 1;
				jogo->table[f][i].valor *= 2;
				jogo->pts += jogo->table[f][i].valor;

				if (jogo->table[f][i].valor == 256) {
					printf(BOLD(GREEN("Ficha de Desfazer Jogada obtida!\n")));
					jogo->numUndo++;
					delay_ms(600);
				}
				if (jogo->table[f][i].valor == 512) {
					printf(BOLD(GREEN("Ficha de Trocar peças obtida!\n")));
					jogo->numTroca++;
					delay_ms(600);
				}


				jogo->table[f+1][i].valor = 0;
			}
		}

		// DESLIZA E LIMPA ============================================================================================================
		espacovazio = 0;
		for (int j = 0; j < jogo->tam; j++) {
			if (jogo->table[j][i].valor != 0) {
				jogo->table[espacovazio][i] = jogo->table[j][i];
				espacovazio++;
			}
		}
		for (int l = espacovazio; l < jogo->tam; l++) {
			jogo->table[l][i].valor = 0;
			jogo->table[l][i].nao_guloso = 0;
		}
	}
}
void moveBaixo(Tabuleiro* jogo) {
	zeraGuloso(jogo);
	int espacovazio;

	for (int i = 0; i < jogo->tam; i++) { // colunas
	
		espacovazio = jogo->tam - 1; 
		for (int j = jogo->tam - 1; j >= 0; j--) {

		// COPIA VALORES PRO FUNDO ==================================================================================================
			if (jogo->table[j][i].valor != 0) {
				jogo->table[espacovazio][i] = jogo->table[j][i];
				espacovazio--;
			}
		}

		// LIMPA CASAS COPIADAS =====================================================================================================
		for (int l = espacovazio; l >= 0; l--) {
			jogo->table[l][i].valor = 0;
			jogo->table[l][i].nao_guloso = 0;
		}

		// FUSÃO ====================================================================================================================
		for (int f = jogo->tam - 1; f > 0; f--) {
			if (jogo->table[f][i].valor != 0 && jogo->table[f][i].valor == jogo->table[f-1][i].valor &&
				jogo->table[f][i].nao_guloso == 0 && jogo->table[f-1][i].nao_guloso == 0) {


				jogo->table[f][i].valor *= 2;
				jogo->table[f][i].nao_guloso = 1;
				jogo->pts += jogo->table[f][i].valor;


				if (jogo->table[f][i].valor == 256) {
					printf(BOLD(GREEN("Ficha de Desfazer Jogada obtida!\n")));
					jogo->numUndo++;
					delay_ms(600);
				}
				if (jogo->table[f][i].valor == 512) {
					printf(BOLD(GREEN("Ficha de Trocar Peças obtida!\n")));
					jogo->numTroca++;
					delay_ms(600);
				}



				
				jogo->table[f-1][i].valor = 0;
			}
		}

		// DESLIZA E LIMPA ==========================================================================================================
		espacovazio = jogo->tam - 1; 
		for (int j = jogo->tam - 1; j >= 0; j--) {

			if (jogo->table[j][i].valor != 0) {
				jogo->table[espacovazio][i] = jogo->table[j][i];
				espacovazio--;
			}
		}
		for (int l = espacovazio; l >= 0; l--) {
			jogo->table[l][i].valor = 0;
			jogo->table[l][i].nao_guloso = 0;
		}
	}
}
void fazBackup(Tabuleiro* jogo) {
	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++)
			jogo->table_bkp[i][j] = jogo->table[i][j];
	jogo->pts_bkp = jogo->pts;
	jogo->numUndo_bkp = jogo->numUndo;
	jogo->numTroca_bkp = jogo->numTroca;
}
void desfazMovimento(Tabuleiro* jogo) {
	Célula **temp = NULL;
	temp = criaMatriz(jogo->tam);
	if (temp == NULL) {
		printf(BOLD(RED("ERRO DE FUNCIONAMENTO DO JOGO. TENTE NOVAMENTE")) "\n");
		return;
	}
	for (int i = 0; i < jogo->tam; i++) // Matriz temporária recebe a tabela atual
		for (int j = 0; j < jogo->tam; j++)
			temp[i][j] = jogo->table[i][j];
	int temp_pts = jogo->pts;
	int undotemp = jogo->numUndo;
	int trocatemp = jogo->numTroca;



	//Matriz backup substitui a matriz atual
	jogo->pts = jogo->pts_bkp;
	jogo->numUndo = jogo->numUndo_bkp;
	jogo->numTroca = jogo->numTroca_bkp; 
	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++)
			jogo->table[i][j] = jogo->table_bkp[i][j];


	//Backup recebe a matriz temp (que era a atual)
	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++)
			jogo->table_bkp[i][j] = temp[i][j];
	jogo->pts_bkp = temp_pts;
	jogo->numTroca_bkp = trocatemp;
	jogo->numUndo_bkp = undotemp;


	liberaMatriz(temp, jogo->tam);	
	printf(BOLD(GREEN("Jogada desfeita!\n")));
	delay_ms(800);
}
void trocaPos(Tabuleiro* jogo, char* str) {
	// str: tc1x1c2x2\0

	int lin1 = str[1] - 'a';
	int col1 = str[2] - '0' - 1;
	int lin2 = str[3] - 'a';
	int col2 = str[4] - '0' - 1;

	if (lin1 > jogo->tam-1 || lin2 > jogo->tam-1 || col1 > jogo->tam-1 || col2 > jogo->tam-1) {
		printf(BOLD(RED("Tamanho de tabuleiro inválido! Tente novamente\n")));
		delay_ms(750);
		return;
	}
	if (jogo->table[lin1][col1].valor == 0 || jogo->table[lin2][col2].valor == 0) {
		printf(BOLD(YELLOW("Células zeradas NÃO podem ser trocadas!\n")));
		delay_ms(750);
		return;
	}

	if (jogo->numTroca > 0) {
		jogo->numTroca--;
		int temp = jogo->table[lin1][col1].valor;
		jogo->table[lin1][col1].valor = jogo->table[lin2][col2].valor;
		jogo->table[lin2][col2].valor = temp;
		jogo->unsaved = 1;
		return;
	}
	else {
		printf(BOLD(YELLOW("SEM FICHAS DE TROCA DISPONÍVEIS. Obtenha mais formando peças de 512\n")));
		delay_ms(750);
		return;
	}
}