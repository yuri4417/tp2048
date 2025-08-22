#include "utils.h"
#include "cores.h"
#include "game.h"
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
    for (int i = 0; str[i] != '\0'; i++)
            str[i] = converteMinuscula(str[i]);
    return 1;
}

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
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
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
int perguntaSN(char* str) {
    char resp[16];
    while (1) {
        printf("%s", str);
        if (!lerEntrada(resp, 16)) return 0; // EOF == 'n'

        if (strlen(resp) == 1) {
            char c = converteMinuscula(resp[0]);
            if (c == 's') return 1;
            if (c == 'n') return 0;
        }

        printf(BOLD(RED("Opção Inválida! Tente novamente (S/N)\n")));
        delay_ms(500);
    }
}
void abortar(Tabuleiro* jogo) {
    if (jogo->table != NULL) {
        liberaMatriz(jogo->table, jogo->tam);
        liberaMatriz(jogo->table_bkp, jogo->tam);
    }
    printf(BOLD(RED("\nERRO DE ENTRADA. O Jogo será finalizado\n")));
    delay_ms(1000);
    exit(-1);
}