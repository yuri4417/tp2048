#ifndef __GAME_H__
#define __GAME_H__

#include "structs.h"


void mostraMenu(Tabuleiro* jogo); 
void novoJogo(Tabuleiro* jogo); 
void geradorRand(Tabuleiro* jogo);
void processaComando(Tabuleiro* jogo, char* opt);
void gameLoop(Tabuleiro* jogo);
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
int trataVitória(Tabuleiro* jogo);
int trataDerrota(Tabuleiro* jogo);
void scoreRodada(Tabuleiro* jogo);
int carregaJogo(Tabuleiro* jogo);
int salvaJogo(Tabuleiro* jogo);
#endif