#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#ifdef _WIN32
	#include <windows.h>
#endif
#define ANSI_RESET            "\x1b[0m"  // desativa os efeitos anteriores
#define ANSI_BOLD             "\x1b[1m"  // coloca o texto em negrito
#define ANSI_COLOR_BLACK      "\x1b[30m"
#define ANSI_COLOR_RED        "\x1b[31m"
#define ANSI_COLOR_GREEN      "\x1b[32m"
#define ANSI_COLOR_YELLOW     "\x1b[33m"
#define ANSI_COLOR_BLUE       "\x1b[34m"
#define ANSI_COLOR_MAGENTA    "\x1b[35m"
#define ANSI_COLOR_CYAN       "\x1b[36m"
#define ANSI_COLOR_WHITE      "\x1b[37m"
#define ANSI_BG_COLOR_BLACK   "\x1b[40m"
#define ANSI_BG_COLOR_RED     "\x1b[41m"
#define ANSI_BG_COLOR_GREEN   "\x1b[42m"
#define ANSI_BG_COLOR_YELLOW  "\x1b[43m"
#define ANSI_BG_COLOR_BLUE    "\x1b[44m"
#define ANSI_BG_COLOR_MAGENTA "\x1b[45m"
#define ANSI_BG_COLOR_CYAN    "\x1b[46m"
#define ANSI_BG_COLOR_WHITE   "\x1b[47m"

// macros para facilitar o uso
#define BOLD(string)       ANSI_BOLD             string ANSI_RESET
#define BLACK(string)      ANSI_COLOR_BLACK      string ANSI_RESET
#define BLUE(string)       ANSI_COLOR_BLUE       string ANSI_RESET
#define RED(string)        ANSI_COLOR_RED        string ANSI_RESET
#define GREEN(string)      ANSI_COLOR_GREEN      string ANSI_RESET
#define YELLOW(string)     ANSI_COLOR_YELLOW     string ANSI_RESET
#define BLUE(string)       ANSI_COLOR_BLUE       string ANSI_RESET
#define MAGENTA(string)    ANSI_COLOR_MAGENTA    string ANSI_RESET
#define CYAN(string)       ANSI_COLOR_CYAN       string ANSI_RESET
#define WHITE(string)      ANSI_COLOR_WHITE      string ANSI_RESET
#define BG_BLACK(string)   ANSI_BG_COLOR_BLACK   string ANSI_RESET
#define BG_BLUE(string)    ANSI_BG_COLOR_BLUE    string ANSI_RESET
#define BG_RED(string)     ANSI_BG_COLOR_RED     string ANSI_RESET
#define BG_GREEN(string)   ANSI_BG_COLOR_GREEN   string ANSI_RESET
#define BG_YELLOW(string)  ANSI_BG_COLOR_YELLOW  string ANSI_RESET
#define BG_BLUE(string)    ANSI_BG_COLOR_BLUE    string ANSI_RESET
#define BG_MAGENTA(string) ANSI_BG_COLOR_MAGENTA string ANSI_RESET
#define BG_CYAN(string)    ANSI_BG_COLOR_CYAN    string ANSI_RESET
#define BG_WHITE(string)   ANSI_BG_COLOR_WHITE   string ANSI_RESET

// caracteres uteis para tabelas
#define TAB_HOR "\u2501" // ━ (horizontal)
#define TAB_VER "\u2503" // ┃ (vertical)
#define TAB_TL  "\u250F" // ┏ (top-left)
#define TAB_ML  "\u2523" // ┣ (middle-left)
#define TAB_BL  "\u2517" // ┗ (bottom-left)
#define TAB_TJ  "\u2533" // ┳ (top-join)
#define TAB_MJ  "\u254B" // ╋ (middle-join)
#define TAB_BJ  "\u253B" // ┻ (bottom-join)
#define TAB_TR  "\u2513" // ┓ (top-right)
#define TAB_MR  "\u252B" // ┫ (middle-right)
#define TAB_BR  "\u251B" // ┛ (bottom-right)
// ============================
// 	STRUCTS
// ============================
typedef struct {
	int valor;
	int nao_guloso; //peça recém gerada não pode gerar outra na mesma rodada q foi criada
} Tablepos;
typedef struct {
	Tablepos **table;
	int tam;
	int pts;
	char nick[50];
	int numUndo; //+1 a cada peça de 256 gerada
	int numTroca; //+1 a cada peça de 512 gerada
	Tablepos **table_bkp;
	int pts_bkp;
} Tabuleiro;

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

// Matrizes
Tablepos **criaMatriz(int n); 
void liberaMatriz(Tablepos **matriz, int n); 


// ============================
//	MAIN
// ============================
int main () {
	Tabuleiro jogo;
	jogo.table = NULL;
	jogo.pts = 0;
	jogo.numUndo = 0;
	jogo.numTroca = 0;
	srand(time(NULL));
	mostraMenu(&jogo);
	return 0;
}



//	Jogo
void mostraMenu(Tabuleiro* jogo) {
	char opt, frase[30];
	do {
		printf("\t---MENU - 2048 ---\n");
		printf("Selecione uma opção\n");
		printf("N) Iniciar novo jogo\n");
		printf("J) Continuar um jogo já existente\n");
		printf("\nC) Carregar um jogo salvo\n");
		printf("S) Salvar o jogo atual\n");
		printf("\nM) Mostrar TOP 10 pontuações\n");
		printf("A) Instruções do jogo\n");
		printf("\nR) Sair\n");
		printf("\nO que deseja fazer? ");
		fgets(frase, 30, stdin);
		if (strlen(frase) == 2 && frase[1] == '\n')
			opt = toupper(frase[0]);
		else
			opt = '\0';
		switch (opt) {
			case 'N':
					novoJogo(jogo);
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
					if (strcmp(temp, "sim") == 0)
						exit(EXIT_SUCCESS);
					else if (strcmp(temp, "não") == 0 || strcmp(temp, "nao") == 0)  { //tratar remoção de acentos
						printf("Retornando ao Menu...\n");
						opt = '\0';
						delay_ms(200);
					}
					else {
					 	printf(BOLD(RED("Opção Inválida. Tente novamente")) "\n");
					 	opt = '\0';
					}
					break;
			default: 
					printf(BOLD(RED("Opção inválida! Escolha novamente.")) "\n");
					delay_ms(450);
					
		} 
		} while (opt != 'R' && opt != 'r');
}
void novoJogo(Tabuleiro* jogo) {
	printf("Iniciando novo jogo\nEscolha a dificuldade do jogo:\n");
	printf("\t(4) Normal  (4x4) - 1 peça gerada por rodada   - 90%% de chance de ser 2 e 10%% de ser 4\n");
	printf("\t(5) Difícil (5x5) - 1 peça gerada por rodada   - 85%% de chance de ser 2 e 15%% de ser 4\n");
	printf("\t(6) Expert  (6x6) - 2 peças geradas por rodada - 80%% de chance de serem 2's e 20%% de serem 4's\n");
	printf("Escolha o tamanho da tabela: ");
	scanf("%d", &jogo->tam);
	while (jogo->tam < 4 || jogo->tam > 6) { 
		printf("Valor inválido! Escolha um valor entre 4 e 6\n-> ");
		scanf("%d", &jogo->tam);
		limpar_buffer();
	}
	jogo->table = criaMatriz(jogo->tam);
	jogo->table_bkp = criaMatriz(jogo->tam);
	if (jogo->table == NULL || jogo->table_bkp == NULL) {
		printf(BOLD(RED("ERRO: Falha ao iniciar o jogo. Tente novamente.\n")));
		exit(EXIT_FAILURE);
	}

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

	int espacoLimpo[jogo->tam * jogo->tam][2]; // matriz p salvar [i][j] zerado
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
}

//	Matrizes
Tablepos **criaMatriz(int n) {
	Tablepos **matriz;
	matriz = malloc(n*sizeof(Tablepos*));
	if (matriz == NULL) return NULL;
	for (int i = 0; i < n; i++) {
		matriz[i] = malloc(n*sizeof(Tablepos));
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
void liberaMatriz(Tablepos **matriz, int n) {
	for (int i = 0; i < n; i++)
		free(matriz[i]);
	free(matriz);
}











