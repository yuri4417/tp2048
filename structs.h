#ifndef __STRUCTS_H__
#define __STRUCTS_H__

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
	int pts;
	char nick[50];
	int verificado; // Se for igual a 1 -> jogo já ganho


	int unsaved; // Jogo iniciado, mas não salvo ainda
	int numUndo; //+1 a cada peça de 256 gerada
	int numTroca; //+1 a cada peça de 512 gerada

	//Backups
	Célula **table_bkp;
	int pts_bkp; 
	int numUndo_bkp; 
	int numTroca_bkp;
} Tabuleiro;

typedef struct {
	char nick[50];
	int pts;
} Ranking;
typedef	struct {
	Ranking top10_4[10];
	Ranking top10_5[10];
	Ranking top10_6[10];

	int tam4;
	int tam5;
	int tam6;
} Tops;
#endif
