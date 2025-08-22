#include "game.h"

#include "utils.h"    
#include "cores.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


void mostraMenu(Tabuleiro* jogo) {
	char opt = '\0';
	char frase[10];
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
		if (!lerEntrada(frase, 10)) {
			abortar(jogo);
			break;
		}
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
					novoJogo(jogo);
				}	
			}
					break;
			case 'j':
					if (jogo->table != NULL)
						gameLoop(jogo);
					else {
						printf(BOLD(RED("ERRO: Nenhum jogo em andamento.")" Carregue um novo jogo ou inicie uma nova partida\n"));
						delay_ms(850);
					}
					break;
			case 'c':
					if (!carregaJogo(jogo)) {
						printf("Erro ao carregar jogo, tente novamente ou inicie uma nova partida\n");
						delay_ms(2500);
					}
					else {
						printf("Jogo carregado! Selecione a opção \"J\" no menu para continuar com o jogo\n");
						opt = 'j';
						delay_ms(2500);
					}
					break;
			case 's':
					salvaJogo(jogo);	
					break; 
			case 'm':
					imprimeTabela(jogo);
					delay_ms(2000);
					break;
			case 'a':

					break;
			case 'r': {
					if (perguntaSN(BOLD(RED("Deseja realmente sair? ")"(" BOLD(RED("S")) "/" BOLD(GREEN("N")))")\n->" )) {
						printf(BOLD(RED("Saindo...\n")));
						delay_ms(650);
						opt = 'r';
					}
					else {
						printf(BOLD(GREEN("Retornando ao menu...\n")));
						opt = '\0';
						delay_ms(600);
					}
			}		
					break;
			default: 
					printf(BOLD(RED("Opção inválida! Escolha novamente.")) "\n");
					delay_ms(850);
		} 
	} while (opt != 'r');
}
void novoJogo(Tabuleiro* jogo) {
	char resp[50];
	if (jogo->table != NULL) {
		liberaMatriz(jogo->table, jogo->tam);
		liberaMatriz(jogo->table_bkp, jogo->tam);
	}
	clear();
	printf("===== Dificuldades disponíveis =====\n");
	printf(CYAN("\t(4) ")BOLD(GREEN("Normal"))"  (4x4) \n");
	printf(CYAN("\t(5) ")BOLD(YELLOW("Difícil"))"  (5x5) \n");
	printf(CYAN("\t(6) ")BOLD(RED("Expert"))"  (6x6) \n");
	printf("Escolha o tamanho do jogo: ");
	int tam = 0;
	while (tam == 0) {
		if (lerEntrada(resp, 50) && strlen(resp) == 1)
			if (resp[0] >= '4' && resp[0] <= '6')
				tam = resp[0] - '0';

		if (tam == 0)
			printf(RED("Tamanho inválido! Escolha entre 4, 5, ou 6\n-> "));
	}
	jogo->tam = tam;
	printf("Digite seu nickname: ");
	fgets(jogo->nick, 50, stdin);
	if (strchr(jogo->nick, '\n') == NULL) // Limpa se digitar mais que a string
        limpar_buffer();
    jogo->nick[strcspn(jogo->nick, "\n")] = '\0';

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
	gameLoop(jogo);
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
	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++) 
			if (jogo->table[i][j].valor == 0) {
				espacoLimpo[qtdVazias][0] = i; 
				espacoLimpo[qtdVazias][1] = j; 
				qtdVazias++;
			}
	if (qtdVazias == 0) {
		for (int i = 0; i < tam2; i++)
			free(espacoLimpo[i]);
		free(espacoLimpo);
		return;
	}
	if (toAdd > qtdVazias) 
		toAdd = qtdVazias;

	for (int i = 0; i < toAdd; i++) { // sorteio da posição p preencher
		int temp = rand() % qtdVazias; // 0 - qtdvazias - 1
		int j = espacoLimpo[temp][0];
		int k = espacoLimpo[temp][1];

		if ((rand() % 100) < prob4)
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
void processaComando(Tabuleiro* jogo, char* opt) {
	int tabuleiro_mudou = 0;
	if (strlen(opt) == 0) {
		return;
	}

	char cmd = converteMinuscula(opt[0]);

	int len = strlen(opt);
	switch (cmd) {
		case 'a':
		case 'd':
		case 's':
		case 'w':
			if (len == 1) {
				fazBackup(jogo);
				if (cmd == 'a') moveEsquerda(jogo);
				if (cmd == 'd') moveDireita(jogo);
				if (cmd == 's') moveBaixo(jogo);
				if (cmd == 'w') moveCima(jogo);
				
				if (mudanca(jogo)) {
					tabuleiro_mudou = 1;
				}
				else {
					printf("Movimento inválido! O Jogo continua igual\n");
					delay_ms(800);
				}
			} 
			else {
				printf(BOLD(RED("Comando inválido! Tente novamente\n")));
				delay_ms(500);
			}
			break;

		case 'u':
			if (len == 1) {
				if (jogo->numUndo > 0) {
					desfazMovimento(jogo);
					jogo->numUndo--;
					printf(BOLD(GREEN("Jogada desfeita!\n")));
					delay_ms(800);
				}
				else {
					printf("Você não possui mais chances de voltar! Obtenha formando peças de 256\n");
					delay_ms(800);
				}
			} 
			else {
				printf(BOLD(RED("Comando inválido! Tente novamente\n")));
				delay_ms(500);
			}
			break;

		case 't': {
			
		}	
			break;
		default:
			printf("Opção inválida! Digite novamente\n");
			delay_ms(850);
	}


	if (tabuleiro_mudou) {
		geradorRand(jogo);
		jogo->unsaved = 1;
	}
}
void gameLoop(Tabuleiro* jogo) {
	char resp[10];
	do {
		clear();
		imprimeTabela(jogo);
		scoreRodada(jogo);
		printf("\nComando: Movimento (W, A, S ou D), Desfazer Movimento (U), Trocar peças (T) ou retornar ao menu (voltar):\n-> ");
		if (!lerEntrada(resp, 50)) 
			break;

		if (strcmp(resp, "voltar") == 0) 
			break;

		processaComando(jogo, resp);

		if (!jogo->verificado && verificaVitória(jogo)) {
			if (!trataVitória(jogo))
				break;
		}
		else if (verificaDerrota(jogo)) {
			if (!trataDerrota(jogo))
				break;
		}
	} while (1);
}


//	Matrizes
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
				}
				if (jogo->table[i][f].valor == 512) {
					jogo->numTroca++;
					printf(BOLD(GREEN("Ficha de Trocar peças obtida!\n")));
					delay_ms(800);
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
				}
				if (jogo->table[i][f].valor == 512) {
					jogo->numTroca++;
					printf(BOLD(GREEN("Ficha de Trocar Peças obtida!\n")));
					delay_ms(800);
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
				}
				if (jogo->table[f][i].valor == 512) {
					printf(BOLD(GREEN("Ficha de Trocar peças obtida!\n")));
					jogo->numTroca++;
					delay_ms(800);
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

	for (int i = 0; i < jogo->tam; i++) {
	
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
				}
				if (jogo->table[f][i].valor == 512) {
					printf(BOLD(GREEN("Ficha de Trocar Peças obtida!\n")));
					jogo->numTroca++;
					delay_ms(800);
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
	int temp_pts = jogo->pts;
	int undotemp = jogo->numUndo;
	int trocatemp = jogo->numTroca_bkp;
	if (temp == NULL) {
		printf(BOLD(RED("ERRO DE FUNCIONAMENTO DO JOGO. TENTE NOVAMENTE")) "\n");
		return;
	}
	for (int i = 0; i < jogo->tam; i++) // Matriz temporária
		for (int j = 0; j < jogo->tam; j++)
			temp[i][j] = jogo->table[i][j];

	jogo->pts = jogo->pts_bkp;
	jogo->numUndo = jogo->numUndo_bkp;
	jogo->numTroca = jogo->numTroca_bkp; 
	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++)
			jogo->table[i][j] = jogo->table_bkp[i][j];

	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++)
			jogo->table_bkp[i][j] = temp[i][j];

	
	jogo->pts_bkp = temp_pts;
	jogo->numTroca_bkp = trocatemp;
	jogo->numUndo_bkp = undotemp;
	liberaMatriz(temp, jogo->tam);	
}
void imprimeTabela(Tabuleiro* jogo) {
    const int largura = 6;

    // borda superior
    printf(TAB_TL);
    for (int j = 0; j < jogo->tam; j++) {
        for (int k = 0; k < largura; k++) 
        	printf(TAB_HOR); 
        if (j < jogo->tam - 1) 
        	 printf(TAB_TJ); 
    }
    printf(TAB_TR);
    printf("\n");

    // nros
    for (int i = 0; i < jogo->tam; i++) {
        printf(TAB_VER);
        for (int j = 0; j < jogo->tam; j++) {
            if (jogo->table[i][j].valor == 0) 
                 printf("%*s", largura, " ");

            else 
                 printf("%*d", largura, jogo->table[i][j].valor);
            printf(TAB_VER);
        }
        printf("\n");

        // horizontal
        if (i < jogo->tam - 1) {
            printf(TAB_ML);
            for (int j = 0; j < jogo->tam; j++) {
                for (int k = 0; k < largura; k++) { printf(TAB_HOR); }
                if (j < jogo->tam - 1) { printf(TAB_MJ); }
            }
            printf(TAB_MR);
            printf("\n");
        }
    }

    // fundo
    printf(TAB_BL);
    for (int j = 0; j < jogo->tam; j++) {
        for (int k = 0; k < largura; k++) { printf(TAB_HOR); }
        if (j < jogo->tam - 1) { printf(TAB_BJ); }
    }
    printf(TAB_BR);
    printf("\n");
}
int mudanca(Tabuleiro* jogo) {
	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++)
			if (jogo->table[i][j].valor != jogo->table_bkp[i][j].valor)
				return 1; //Houve alteração de uma rodada para outra
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
int trataVitória(Tabuleiro* jogo) {
    jogo->verificado = 1;

    printf(BOLD(GREEN("\t\t\t\t\tParabéns! Você Venceu!\n")));
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
int trataDerrota(Tabuleiro* jogo) {
    printf(BOLD(RED("Você PERDEU!\n" ANSI_RESET)));
    if (jogo->numUndo > 0) {
            clear();
            imprimeTabela(jogo);
            if (perguntaSN("Você pode desfazer seu último movimento. Gostaria de voltar?\n-> ")) {
                desfazMovimento(jogo);
                jogo->numUndo--;
                return 1;
            } 
            else {
                printf(BOLD(YELLOW("Voltando ao menu...")));
                delay_ms(650);
                return 0;
            }
    } 
    else {
        char opt[10];
        printf(BOLD(RED("Sem movimentos de desfazer disponíveis.\n")));
        printf("Aperte qualquer tecla para retornar ao menu...\n->");
        lerEntrada(opt, 10);
        return 0;
    }
}
void scoreRodada(Tabuleiro* jogo) {
	for (int i = 0; i < 30; i++)
		printf("=");
	printf("\n");
	printf("Jogador: %s\n\n", jogo->nick);
	printf("Pontuação: %d\n", jogo->pts);
	printf("Desfazer: %d\n", jogo->numUndo);
	printf("Trocas: %d\n", jogo->numTroca);
	for (int i = 0; i < 30; i++)
		printf("=");
	printf("\n");
}
int carregaJogo(Tabuleiro* jogo) {
	char filename[50];
	clear();
	printf("Digite o nome do arquivo de save a ser carregado: ");
	fgets(filename, 50, stdin);
	filename[strcspn(filename, "\n")] = '\0';
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
	jogo->table_bkp = criaMatriz(jogo->tam); //colocar verificacao

	if (jogo->table == NULL || jogo->table_bkp == NULL) {
		printf(BOLD(RED("ERRO: FALHA AO CARREGAR O JOGO. Tente novamente\n")));
		fclose(save);
		return 0;
	}

	fscanf(save, "%d", &jogo->numUndo);
	fscanf(save, "%d", &jogo->numTroca);

	fscanf(save, "%d", &jogo->pts);
	fgetc(save);
	fgets(jogo->nick, 50, save);
	jogo->nick[strcspn(jogo->nick, "\n")] = '\0';
	
	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++) 
			fscanf(save, "%d", &jogo->table[i][j].valor);

	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++) 
			fscanf(save, "%d", &jogo->table_bkp[i][j].valor);

	printf(BOLD(GREEN("Arquivo \"%s\" carregado com sucesso!\n")), filename);
	fclose(save);
	return 1;
}
int salvaJogo(Tabuleiro* jogo) {
	char filename[50];
	FILE *toSave = NULL;

	printf("Digite o nome do arquivo para salvamento: ");
	fgets(filename, 50, stdin);

	filename[strcspn(filename, "\n")] = '\0';
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
			if (j < jogo->tam-1)
				fprintf(toSave, "%d ", jogo->table[i][j].valor);
			else
				fprintf(toSave, "%d", jogo->table[i][j].valor);
		fprintf(toSave, "\n");
	}


	for (int i = 0; i < jogo->tam; i++) {
		for (int j = 0; j < jogo->tam; j++)
			if (j < jogo->tam)
				fprintf(toSave, "%d ", jogo->table_bkp[i][j].valor);
			else
				fprintf(toSave, "%d", jogo->table_bkp[i][j].valor);
		fprintf(toSave, "\n");
	}

	jogo->unsaved = 0;
	printf("Jogo salvo com sucesso no arquivo \"%s\"!\n", filename);
	fclose(toSave);
	return 1;
}