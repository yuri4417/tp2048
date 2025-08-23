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
    printf(BOLD(RED("\nERRO DE ENTRADA. O Jogo será finalizado. \n")));
    delay_ms(1000);
    exit(-1);
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