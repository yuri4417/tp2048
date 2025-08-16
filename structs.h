#ifndef __ESTRUTURAS_H__
#define __ESTRUTURAS_H__

// ============================
// 	STRUCTS
// ============================
typedef struct {
	int valor;
	int nao_guloso; //peça recém gerada não pode gerar outra na mesma rodada q foi criada
} Célula;
typedef struct {
	Célula **table;
	int tam;
	long int pts;
	char nick[50];
	int unsaved; // Jogo iniciado, mas não salvo ainda
	int numUndo; //+1 a cada peça de 256 gerada
	int numTroca; //+1 a cada peça de 512 gerada
	Célula **table_bkp;
	long int pts_bkp;
} Tabuleiro;
#endif