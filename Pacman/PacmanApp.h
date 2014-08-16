#pragma once

#include <SDL.h>
#include "Graphics.h"
#undef main
#include "PacmanGame.h"
#include "GameLogic.h"
#include "StartScreen.h"




class PacmanApp {
	enum GAME_STATUS {
		START_SCREEN,
		IN_GAME,
		SCORE,
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
	void ExitGame();

	bool Init();
	void Run();
	void Done();
};