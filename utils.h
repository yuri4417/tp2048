#ifndef __UTILS_H__
#define __UTILS_H__

#include "structs.h"


// ===========================================================================================================================================================
// UTILITÁRIOS GERAIS
// ===========================================================================================================================================================
void limpar_buffer();
void delay_ms(int ms); //atraso estético
void clear(); // "Apagar" a tela
void zeraGuloso(Tabuleiro* jogo);
void abortar(Tabuleiro* jogo); // Evita a repetição infinita por EOF
int mudanca(Tabuleiro* jogo); // Verifica se entre o tabuleiro atual e o backup (antes do movimento) houve alguma alteração
int verificaVitória(Tabuleiro* jogo);
int verificaDerrota(Tabuleiro* jogo);
int trataVitória(Tabuleiro* jogo, Tops* ranking);
int trataDerrota(Tabuleiro* jogo, Tops* ranking);
int carregaJogo(Tabuleiro* jogo);
int salvaJogo(Tabuleiro* jogo);

// ===========================================================================================================================================================
// MANIPULAÇÃO DE STRINGS
// ===========================================================================================================================================================
int lerEntrada(char* str, int tam); // fgets + tratamento de EOF + remoção de \n + Limpar buffer + conversão para minuscula;
char converteMinuscula(char letra);
int perguntaSN(char* str); // pergunta de Sim ou Não
void catTxt(char *str, int tam); //Acrescenta .txt ao arquivo, se necessário
int arrumaString(char *str); //Tratamento da string para o comando de trocar posições 
void apagaEspaço(char *str); // Auxiliar para a função acima / Tratamento de entradas com espaços como "  s  "

// ===========================================================================================================================================================
// IMPRESSÃO EM TELA
// ===========================================================================================================================================================
void imprimeTabela(Tabuleiro* jogo);
void imprimeRanking(Tops* ranking, Tabuleiro* jogo);
void mostraAjuda(Tabuleiro* jogo);
void scoreRodada(Tabuleiro* jogo);

// ===========================================================================================================================================================
// FUNÇÕES DE RANKING
// ===========================================================================================================================================================
int carregaRanking(Tops* ranking);
int salvaRanking(Tops* ranking);
void ordenaRanking(Ranking* rank, int n);
void atualizaRanking(Tops* ranking, Tabuleiro* jogo);
#endif