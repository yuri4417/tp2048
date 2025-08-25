#ifndef __GAME_H__
#define __GAME_H__

#include "structs.h"

// ===========================================================================================================================================================
// NÚCLEO DE JOGO
// ===========================================================================================================================================================
void mostraMenu(Tabuleiro* jogo, Tops* ranking); // Loop principal
void novoJogo(Tabuleiro* jogo, Tops* ranking); // Inicialização de jogo
void gameLoop(Tabuleiro* jogo, Tops* ranking); // Loop do jogo. Enquanto ganhar e quiser continuar ou perder e ter trocas
void processaComando(Tabuleiro* jogo, char* opt); 
void geradorRand(Tabuleiro* jogo); //Gerador dos números 2 ou 4 na tabela

// ===========================================================================================================================================================
// MATRIZES
// ===========================================================================================================================================================
Célula **criaMatriz(int n); 
void liberaMatriz(Célula** matriz, int n); 
void moveEsquerda(Tabuleiro* jogo);
void moveDireita(Tabuleiro* jogo);
void moveCima(Tabuleiro* jogo);
void moveBaixo(Tabuleiro* jogo);
void fazBackup(Tabuleiro* jogo);
void desfazMovimento(Tabuleiro* jogo);
void trocaPos(Tabuleiro* jogo, char* str); // troca entre duas posições da tabela


#endif