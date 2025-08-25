#include "cores.h"
#include "structs.h"
#include "game.h"
#include "utils.h"


#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main () {
	Tabuleiro jogo;
	Tops rankings;
	jogo.table = NULL;
	jogo.table_bkp = NULL;
	jogo.unsaved = 0;

	srand(time(NULL));

	clear();
	mostraMenu(&jogo, &rankings);

	if(jogo.table != NULL) {
		liberaMatriz(jogo.table, jogo.tam);
		liberaMatriz(jogo.table_bkp, jogo.tam);
	}
	return 0;
}