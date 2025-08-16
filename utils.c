#include "utils.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
    #include <windows.h>
#else 
    #include <sys/ioctl.h>
    #include <unistd.h>
#endif
// Utilitários
int lerEntrada(char* str, int tam) {
    if (fgets(str, tam, stdin) == NULL) // Ctrl D
        return 0;
    
    if (strchr(str, '\n') == NULL) // Limpa se digitar mais que a string
        limpar_buffer();
    
    str[strcspn(str, "\n")] = '\0'; // Remove \n
    return 1;
}
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
void clear() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
}
void cprintf( char* string) {
    int largura_terminal = 80;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
        largura_terminal = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0) {
        largura_terminal = w.ws_col;
    }
#endif

    int tamanho_texto = strlen(string);
    int espacamento = (largura_terminal - tamanho_texto) / 2;

    if (espacamento < 0) {
        espacamento = 0;
    }

    for (int i = 0; i < espacamento; i++) {
        printf(" ");
    }

    printf("%s\n", string);
}
void zeraGuloso(Tabuleiro* jogo) {
	for (int i = 0; i < jogo->tam; i++) {
		for (int j = 0; j < jogo->tam; j++) {
			jogo->table[i][j].nao_guloso = 0;
		}
	}
}
char converteMinuscula(char letra) {
	if (letra >= 65 && letra <= 90)
		letra += 32; 	
	return letra;
}