#include "cores.c"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#ifdef _WIN32
	#include <windows.h>
#else 
	#include <sys/ioctl.h>
	#include <unistd.h>
#endif


// ============================
// 	STRUCTS
// ============================
typedef struct {
	int valor;
	int nao_guloso; //peça recém gerada não pode gerar outra na mesma rodada q foi criada
} Célula;
typedef struct {
	Célula **table;
	int tam;
	long int pts;
	char nick[50];
	int unsaved; // Jogo iniciado, mas não salvo ainda
	int numUndo; //+1 a cada peça de 256 gerada
	int numTroca; //+1 a cada peça de 512 gerada
	Célula **table_bkp;
	long int pts_bkp;
} Tabuleiro;
typedef struct {
	int largura;
	int altura;
} Infos;



// ============================
// 	Funções
// ============================

// Jogo
void mostraMenu(Tabuleiro* jogo); 
void novoJogo(Tabuleiro* jogo); 

// Utilitários
void limpar_buffer();
void delay_ms(int ms);
void geradorRand(Tabuleiro* jogo);
void clear();
void terminalInfos(Infos* dim);
void cprintf(char* string); //Imprime no centro de uma linha
void mprintf(char* string); //Imprime no centro do terminal;
void zeraGuloso(Tabuleiro* jogo);





// Matrizes
Célula **criaMatriz(int n); 
void liberaMatriz(Célula** matriz, int n); 
void moveCursor(int linha, int coluna);
void moveEsquerda(Tabuleiro* jogo);
void moveDireita(Tabuleiro* jogo);
void moveCima(Tabuleiro* jogo);
void moveBaixo(Tabuleiro* jogo);

// ============================
//	MAIN
// ============================
int main () {
	Tabuleiro jogo;
	jogo.table = NULL;
	jogo.unsaved = 1;
	jogo.pts = 0;
	jogo.numUndo = 0;
	jogo.numTroca = 0;
	srand(time(NULL));
	clear();
	mostraMenu(&jogo);
	return 0;
}






//	Jogo
void mostraMenu(Tabuleiro* jogo) {
	char opt, frase[100];
	do {	
		cprintf("Bem-vindo!");
		printf("N) Iniciar novo jogo\n");
		printf("J) Continuar um jogo já existente\n");
		printf("\nC) Carregar um jogo salvo\n");
		printf("S) Salvar o jogo atual\n");
		printf("\nM) Mostrar TOP 10 pontuações\n");
		printf("A) Instruções do jogo\n");
		printf("\nR) Sair\n");
		printf("\nO que deseja fazer? ");
		fgets(frase, 100, stdin);
		if (strlen(frase) == 2)
			opt = toupper(frase[0]);
		else
			opt = '\0';
		switch (opt) {
			case 'N': {
					int start = 0;
					if (!jogo->unsaved) 
						start = 1;
					else {
						char temp[10];
        				printf(BOLD(BG_YELLOW("AVISO: JOGO EM ANDAMENTO NÃO SALVO")) ". Deseja continuar mesmo assim?(S/N)\n->");
       					fgets(temp, 10, stdin);
        				if (tolower(temp[0]) == 's')
           					 start = 1;
   					}
					if (start) {
						printf(BOLD(GREEN("Iniciando novo jogo... Divirta-se!")) "\n");
						delay_ms(850);
						clear();
						novoJogo(jogo);
					}
		}
					break;
			case 'J':
		
					break;
			case 'C':
		
					break;
			case 'S':

					break;
			case 'M':

					break;
			case 'A':

					break;
			case 'R':
					char temp[30];
					printf("Deseja realmente encerrar o jogo? (Sim/Não)\n->");
					fgets(temp, 30, stdin);
					int len = strlen(temp);
					if (temp[len-1] == '\n') 
						temp[len-1] = '\0';
					for (int i = 0; temp[i] != '\0'; i++)
						temp[i] = tolower(temp[i]);
					if (strcmp(temp, "sim") == 0) {
						if (!jogo->unsaved)
							exit(EXIT_SUCCESS);
						else {
							printf("Jogo não salvo. Deseja sair mesmo assim?");
						}
					}

					else if (strcmp(temp, "não") == 0 || strcmp(temp, "nao") == 0|| strcmp(temp, "nÃo") == 0)  { //tratar remoção de acentos
						printf(BOLD(GREEN("Retornando ao Menu...")) "\n");
						opt = '\0';
						delay_ms(250);
						clear();
					}
					else {
					 	printf(BOLD(RED("Opção Inválida. Tente novamente")) "\n");
					 	opt = '\0';
					}
					break;
			default: 
					printf(BOLD(RED("Opção inválida! Escolha novamente.")) "\n");
					delay_ms(850);
					clear();
		} 
	} while (opt != 'R' && opt != 'r');
}
void novoJogo(Tabuleiro* jogo) {
	char resp[50];
	printf("\t(4) Normal  (4x4) - 1 peça gerada por rodada   - 90%% de chance de ser 2 e 10%% de ser 4\n");
	printf("\t(5) Difícil (5x5) - 1 peça gerada por rodada   - 85%% de chance de ser 2 e 15%% de ser 4\n");
	printf("\t(6) Expert  (6x6) - 2 peças geradas por rodada - 80%% de chance de serem 2's e 20%% de serem 4's\n");
	printf("Escolha o tamanho da tabela: ");
	scanf("%d", &jogo->tam);
	while (jogo->tam < 4 || jogo->tam > 6) {
		printf("Valor inválido! Escolha um valor entre 4 e 6\n-> ");
		scanf("%d", &jogo->tam);
	}
	limpar_buffer();
	//Criação e verificação das matrizes
	jogo->table = criaMatriz(jogo->tam);
	jogo->table_bkp = criaMatriz(jogo->tam);
	if (jogo->table == NULL || jogo->table_bkp == NULL) {
		printf(BOLD(RED("ERRO: Falha ao iniciar o jogo. Tente novamente.\n")));
		delay_ms(2000);
		return;
	}


	do {


		fgets(resp, 50, stdin);
		int len = strlen(resp);
		if (resp[len-1] == '\n')
			resp[len-1] = '\0';

	} while (strcmp(resp, "voltar") != 0);	

}






// Utilitários
void limpar_buffer() {
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF);
}
void delay_ms(int ms) {
#ifdef _WIN32
	Sleep(ms);
#else 
	struct timespec ts;
	ts.tv_sec = ms / 1000;                 
    ts.tv_nsec = (ms % 1000) * 1000000L;    
    nanosleep(&ts, NULL);
#endif
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

	int **espacoLimpo = malloc(jogo->tam*jogo->tam*sizeof(int));
	if (espacoLimpo == NULL) {
		printf(BOLD(RED("ERRO NA VERIFICAÇÃO DE SEGURANÇA. Tente novamente")) "\n");
		return;	
	}
	for (int i = 0; i < (jogo->tam*jogo->tam); i++) {
		espacoLimpo[i] = malloc(2*sizeof(int));
		if (espacoLimpo[i] == NULL) {
			printf(BOLD(RED("ERRO NA VERIFICAÇÃO DE SEGURANÇA. Tente novamente")) "\n");
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
	if (qtdVazias == 0)
		return;
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
	for (int i = 0; i < jogo->tam*jogo->tam; i++)
		free(espacoLimpo[i]);
	free(espacoLimpo);
}
void clear() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}
void cprintf(char *string) {
	struct winsize w;
	int colunas;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
    	colunas = w.ws_col;
    }
	int print = strlen(string);
	int center = (colunas - print) / 2;
	for (int i = 0; i < center; i++)
		printf(" ");
	printf("%s", string);
	printf("\n");
}
void terminalInfos(Infos *dim) {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        dim->largura = w.ws_col;
        dim->altura = w.ws_row;
    }
}
void moveCursor(int linha, int coluna) {
    printf("\x1b[%d;%dH", linha, coluna);
}
void mprintf(char* texto) {
    Infos dim;
    terminalInfos(&dim);

    int tamanho_texto = strlen(texto);

    int linha_inicio = dim.altura / 2;
    int coluna_inicio = (dim.largura - tamanho_texto) / 2;

    if (coluna_inicio < 1) coluna_inicio = 1;
    if (linha_inicio < 1) linha_inicio = 1;

    moveCursor(linha_inicio, coluna_inicio);

    printf("%s\n", texto);
}
void zeraGuloso(Tabuleiro* jogo) {
	for (int i = 0; i < jogo->tam; i++) {
		for (int j = 0; j < jogo->tam; j++) {
			jogo->table[i][j].nao_guloso = 0;
		}
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
void copiaMatriz(Célula** table, Célula table_bkp) {
	
}