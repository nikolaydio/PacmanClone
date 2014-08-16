#include "PacmanApp.h"

#include <SDL.h>
#undef main
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <cassert>

#define MSEC_PER_FRAME 16

PacmanApp::PacmanApp() {
}
bool PacmanApp::Init() {
	status = START_SCREEN;
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	TTF_Init();

	window = SDL_CreateWindow("Pacman Game", 
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 768, 480, 0);
	if(!window) {
		return false;
	}
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!renderer) {
		SDL_DestroyWindow(window);
		return false;
	}
	res_manager.Init(renderer, "data/");
	
	start_screen.Init(res_manager);
	logic = &start_screen;

	return true;
}
void PacmanApp::Run() {
	int last_frame = SDL_GetTicks();
	
	while(status != GAME_EXITING) {
		int now = SDL_GetTicks();
		int delta_ms = now - last_frame;
		last_frame = now;

		if(delta_ms < MSEC_PER_FRAME) {
			SDL_Delay(MSEC_PER_FRAME - delta_ms);
		}

		SDL_Event e;
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) {
				status = GAME_EXITING;
			}
		}

		SDL_RenderClear(renderer);
		logic->Update(delta_ms / 1000.0f, 0);
		logic->Render(delta_ms / 1000.0f, renderer);
		SDL_RenderPresent(renderer);
	}
}
void PacmanApp::Done() {
	res_manager.Cleanup();
	SDL_Quit();
}