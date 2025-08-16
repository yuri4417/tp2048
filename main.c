#include "cores.h"
#include "structs.h"
#include "game.h"
#include "utils.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#ifdef _WIN32
	#include <windows.h>
#else 
	#include <sys/ioctl.h>
	#include <unistd.h>
#endif

// ============================
//	MAIN
// ============================
int main () {
	Tabuleiro jogo;
	jogo.table = NULL;
	jogo.table_bkp = NULL;
	jogo.unsaved = 1;

	srand(time(NULL));

	clear();
	mostraMenu(&jogo);

	if(jogo.table != NULL) {
		liberaMatriz(jogo.table, jogo.tam);
		liberaMatriz(jogo.table_bkp, jogo.tam);
	}
	return 0;
}
















