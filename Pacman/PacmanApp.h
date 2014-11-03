#pragma once

#include <SDL.h>
#include "Graphics.h"
#undef main
#include "PacmanGame.h"
#include "GameLogic.h"
#include "StartScreen.h"


#define WINDOW_SIZE_X 640
#define WINDOW_SIZE_Y 480

class PacmanApp {
	enum GAME_STATUS {
		START_SCREEN,
		IN_GAME,
		GAME_EXITING
	};

	GAME_STATUS status;
	SDL_Renderer* renderer;
	SDL_Window* window;
	ResourceManager res_manager;

	GameLogic* logic;

	StartScreen start_screen;
	PacmanGame pacman;

public:
	PacmanApp();

	void LoadGame(const char* filename);
	void LoadStartScreen();
	void LoadScore(bool victory, int score);

	void ExitGame();

	bool Init();
	void Run();
	void Done();
};