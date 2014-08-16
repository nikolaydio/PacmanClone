#include "PacmanApp.h"

int main(int argc, char* argv[]) {
	PacmanApp game;
	if(game.Init())
		game.Run();
	game.Done();
	return 0;
}