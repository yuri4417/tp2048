#ifndef __UTILS_H__
#define __UTILS_H__

#include "structs.h"


// ===========================================================================================================================================================
// UTILITÁRIOS GERAIS
// ===========================================================================================================================================================
void limpar_buffer();
void delay_ms(int ms);
void clear();
void zeraGuloso(Tabuleiro* jogo);
void abortar(Tabuleiro* jogo);
int mudanca(Tabuleiro* jogo);
int verificaVitória(Tabuleiro* jogo);
int verificaDerrota(Tabuleiro* jogo);
int trataVitória(Tabuleiro* jogo, Tops* ranking);
int trataDerrota(Tabuleiro* jogo, Tops* ranking);

// ===========================================================================================================================================================
// MANIPULAÇÃO DE STRINGS
// ===========================================================================================================================================================
int lerEntrada(char* str, int tam);
char converteMinuscula(char letra);
int perguntaSN(char* str);
void catTxt(char *str, int tam);
int arrumaString(char *str);
void apagaEspaço(char *str);
int stringReal(char* str);

// ===========================================================================================================================================================
// IMPRESSÃO EM TELA
// ===========================================================================================================================================================
void imprimeTabela(Tabuleiro* jogo);
void imprimeRanking(Tops* ranking);
void mostraAjuda(Tabuleiro* jogo);

// ===========================================================================================================================================================
// FUNÇÕES DE RANKING
// ===========================================================================================================================================================
int carregaRanking(Tops* ranking);
int salvaRanking(Tops* ranking);
void ordenaRanking(Ranking* rank, int n);
int atualizaRanking(Tops* ranking, Tabuleiro* jogo);
#endif