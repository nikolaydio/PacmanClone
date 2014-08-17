#include "StartScreen.h"
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <cassert>
#include "PacmanApp.h"

void StartScreen::Init(ResourceManager& res_manager) {
	//setup start screen;
	background = res_manager.GetTexture("wallpaper.png");
	selected = 0;
	TTF_Font* font = TTF_OpenFont("ROCKSTEADY.ttf", 36);
	assert(font);
	SDL_Color color;
	color.a = 255;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	SDL_Surface* surf = TTF_RenderText_Blended(font, "Play", color);
	menu_entries[0] = res_manager.AddSurface("Play", surf);

	surf = TTF_RenderText_Blended(font, "Help", color);
	menu_entries[1] = res_manager.AddSurface("Help", surf);
	SDL_SetTextureAlphaMod(menu_entries[1].ptr, 100);

	surf = TTF_RenderText_Blended(font, "Exit", color);
	menu_entries[2] = res_manager.AddSurface("Exit", surf);
	SDL_SetTextureAlphaMod(menu_entries[2].ptr, 100);

	TTF_CloseFont(font);

	background_music = Mix_LoadMUS("music/pacman-start.ogg");
	if(background_music == 0) {
		printf("Failed to load some sound files.\n");
	}
	Mix_PlayMusic(background_music, -1);
}

void StartScreen::Update(float dt, uint32_t input_state, uint32_t input_events, PacmanApp* app) {
	if((input_events & INPUT_UP) && selected > 0) {
		SDL_SetTextureAlphaMod(menu_entries[selected].ptr, 100);
		selected--;
		SDL_SetTextureAlphaMod(menu_entries[selected].ptr, 255);
	}
	if((input_events & INPUT_DOWN) && selected < MENU_ENTRY_COUNT - 1) {
		SDL_SetTextureAlphaMod(menu_entries[selected].ptr, 100);
		selected++;
		SDL_SetTextureAlphaMod(menu_entries[selected].ptr, 255);
	}
	if(input_events & INPUT_FIRE) {
		if(selected == 0) {
			app->LoadGame("stages/level1.txt");
			Mix_PauseMusic();
			return;
		}
		if(selected == 2) {
			app->ExitGame();
			return;
		}
	}
	if(Mix_PausedMusic())
		Mix_ResumeMusic();
}
void StartScreen::Render(float dt, SDL_Renderer* renderer) {
	int width, height;
	SDL_GetRendererOutputSize(renderer, &width, &height);

	SDL_RenderCopy(renderer, background.ptr, 0, 0);
	SDL_Rect rect;
	rect.y = 300;
	for(int i = 0; i < MENU_ENTRY_COUNT; ++i ) {
		rect.w = menu_entries[i].width;
		rect.h = menu_entries[i].height;
		rect.x = width / 2 - rect.w / 2;
		SDL_RenderCopy(renderer, menu_entries[i].ptr, 0, &rect);
		rect.y += rect.h;
	}
}