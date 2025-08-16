#ifndef __UTILS_H__
#define __UTILS_H__

#include "structs.h"

// Utilitários
int lerEntrada(char* str, int tam);
void limpar_buffer();
void delay_ms(int ms);
void clear();
void cprintf(char* string); //Imprime no centro de uma linha
void zeraGuloso(Tabuleiro* jogo);
char converteMinuscula(char letra);
int stringReal(char* str);
int perguntaSN(char* str);


#endif