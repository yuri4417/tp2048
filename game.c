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
		cprintf(BOLD(CYAN("Bem-vindo!\n\n")));
		for (int i = 0; i < 3; i++)
			printf("\n");
		cprintf(BOLD(GREEN("(N) "))BOLD(WHITE("Iniciar Novo Jogo\n")));
		cprintf(BOLD(GREEN("(J) "))BOLD(WHITE("Continuar um jogo atual\n\n")));
		cprintf(BOLD(YELLOW("(C) "))BOLD(WHITE("Carregar um Jogo Salvo\n")));
		cprintf(BOLD(YELLOW("(S) "))BOLD(WHITE("Salvar o Jogo Atual\n\n")));
		cprintf(BOLD(BLUE("(M) "))BOLD(WHITE("Mostrar TOP 10 pontuações\n")));
		cprintf(BOLD(BLUE("(A) "))BOLD(WHITE("Como jogar? \n\n")));
		cprintf(BOLD(RED("(R) "))BOLD(WHITE("Sair \n\n")));
		cprintf("O que deseja fazer?\n");
		cprintf("-> "); 
		if (!lerEntrada(frase, 10)) {
			opt = 'r';
			continue;
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

					break;
			case 'c':
		
					break;
			case 's':

					break;
			case 'm':

					break;
			case 'a':

					break;
			case 'r': {
					if (perguntaSN("Deseja realmente sair? (S/N)\n-> ")) {
						printf(BOLD(RED("Saindo...\n")));
						delay_ms(650);
						opt = 'r';
					}
					else {
						printf(BOLD(GREEN("Retornando ao menu\n")));
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
	cprintf("Dificuldades disponíveis:\n");
	cprintf("(4) Normal  (4x4) \n");
	cprintf("(5) Difícil (5x5) \n");
	cprintf("(6) Expert  (6x6) \n");
	cprintf("Escolha o tamanho do jogo: ");
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
	lerEntrada(jogo->nick, 50);

	//Criação e verificação das matrizes
	jogo->table = criaMatriz(jogo->tam);
	jogo->table_bkp = criaMatriz(jogo->tam);
	if (jogo->table == NULL || jogo->table_bkp == NULL) {
		printf(BOLD(RED("ERRO: Falha ao iniciar o jogo. Tente novamente.\n")));
		delay_ms(2000);
		return;
	}
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



	do {
		clear();
		imprimeTabela(jogo);
		printf("\nComando: Movimento (W, A, S ou D), Desfazer Movimento (U), Trocar peças (T) ou retornar ao menu (voltar):\n-> ");
		if (!lerEntrada(resp, 50)) 
			break;
		for (int i = 0; resp[i] != '\0'; i++)
			resp[i] = converteMinuscula(resp[i]);

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

	switch (cmd) {
		case 'a':
		case 'd':
		case 's':
		case 'w':
			if (strlen(opt) == 1) {
				fazBackup(jogo);
				if (cmd == 'a') moveEsquerda(jogo);
				if (cmd == 'd') moveDireita(jogo);
				if (cmd == 's') moveBaixo(jogo);
				if (cmd == 'w') moveCima(jogo);
				
				if (mudanca(jogo)) {
					tabuleiro_mudou = 1;
				} else {
					printf("Movimento inválido! O Jogo continua igual\n");
					delay_ms(800);
				}
			} else {
				printf(BOLD(RED("Comando inválido! Tente novamente\n")));
				delay_ms(500);
			}
			break;

		case 'u':
			if (strlen(opt) == 1) {
				if (jogo->numUndo > 0) {
					desfazMovimento(jogo);
					jogo->numUndo--;
					printf(BOLD(GREEN("Jogada desfeita!\n")));
					delay_ms(800);
				} else {
					printf("Você não possui mais chances de voltar! Obtenha formando peças de 256\n");
					delay_ms(800);
				}
			} else {
				printf(BOLD(RED("Comando inválido! Tente novamente\n")));
				delay_ms(500);
			}
			break;

		case 't': {
			
			break;
		}	
		default:
			printf("Opção inválida! Digite novamente\n");
			delay_ms(850);
	}


	if (tabuleiro_mudou) {
		geradorRand(jogo);
		jogo->unsaved = 1;
	}
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
		
		// COPIAR NÚMEROS PRA MÁXIMA ESQUERDA ==========================================================================================
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
				jogo->pts += jogo->table[i][f].valor;

				jogo->table[i][f+1].valor = 0;
				jogo->table[i][f].nao_guloso = 1;
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
				jogo->pts += jogo->table[i][f].valor;

				jogo->table[i][f].nao_guloso = 1;
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

	
	for (int i = 0; i < jogo->tam; i++) { // i = colunas
		
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

				jogo->table[f][i].valor *= 2;
				jogo->pts += jogo->table[f][i].valor;

				jogo->table[f][i].nao_guloso = 1;
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
				jogo->pts += jogo->table[f][i].valor;

				jogo->table[f][i].nao_guloso = 1;
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
}
void desfazMovimento(Tabuleiro* jogo) {
	Célula **temp = NULL;
	temp = criaMatriz(jogo->tam);
	long int temp_pts = jogo->pts;
	if (temp == NULL) {
		printf(BOLD(RED("ERRO DE FUNCIONAMENTO DO JOGO. TENTE NOVAMENTE")) "\n");
		return;
	}
	for (int i = 0; i < jogo->tam; i++) // Matriz temporária
		for (int j = 0; j < jogo->tam; j++)
			temp[i][j] = jogo->table[i][j];

	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++)
			jogo->table[i][j] = jogo->table_bkp[i][j];

	for (int i = 0; i < jogo->tam; i++)
		for (int j = 0; j < jogo->tam; j++)
			jogo->table_bkp[i][j] = temp[i][j];

	jogo->pts = jogo->pts_bkp;
	jogo->pts_bkp = temp_pts;
	liberaMatriz(temp, jogo->tam);	
}
void imprimeTabela(Tabuleiro* jogo) {
	for (int i = 0; i < jogo->tam; i++) {
		for (int j = 0; j < jogo->tam; j++)
			printf("%6d ", jogo->table[i][j].valor);
	printf("\n");
	}
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

    printf(BOLD(GREEN("Parabéns! Você Venceu!\n")));
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