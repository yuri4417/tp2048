#ifndef __GAME_H__
#define __GAME_H__

#include "structs.h"


void mostraMenu(Tabuleiro* jogo, Tops* ranking); 
void novoJogo(Tabuleiro* jogo, Tops* ranking); 
void geradorRand(Tabuleiro* jogo);
void processaComando(Tabuleiro* jogo, char* opt);
void gameLoop(Tabuleiro* jogo, Tops* ranking);
void mostraAjuda(Tabuleiro* jogo);

// Matrizes
Célula **criaMatriz(int n); 
void liberaMatriz(Célula** matriz, int n); 
void moveEsquerda(Tabuleiro* jogo);
void moveDireita(Tabuleiro* jogo);
void moveCima(Tabuleiro* jogo);
void moveBaixo(Tabuleiro* jogo);
void fazBackup(Tabuleiro* jogo);
void desfazMovimento(Tabuleiro* jogo);
void imprimeTabela(Tabuleiro* jogo);
int mudanca(Tabuleiro* jogo);
int verificaVitória(Tabuleiro* jogo);
int verificaDerrota(Tabuleiro* jogo);
int trataVitória(Tabuleiro* jogo, Tops* ranking);
int trataDerrota(Tabuleiro* jogo, Tops* ranking);
void scoreRodada(Tabuleiro* jogo);
int carregaJogo(Tabuleiro* jogo);
int salvaJogo(Tabuleiro* jogo);
int trocaPos(Tabuleiro* jogo, char* str);
void ordenaRanking(Ranking* rank, int n);
int atualizaRanking(Tops* ranking, Tabuleiro* jogo);
void imprimeRanking(Tops* ranking);
#endif