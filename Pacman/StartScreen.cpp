#include "StartScreen.h"
#include <SDL_ttf.h>
#include <cassert>

void StartScreen::Init(ResourceManager& res_manager) {
	//setup start screen;
	background = res_manager.GetTexture("wallpaper.png");
	TTF_Font* font = TTF_OpenFont("ROCKSTEADY.ttf", 36);
	assert(font);
	SDL_Color color;
	color.a = 255;
	color.r = 255;
	color.g = 255;
	color.b = 255;
	SDL_Surface* surf = TTF_RenderText_Blended(font, "Play", color);
	play_text = res_manager.AddSurface("Play", surf);
	surf = TTF_RenderText_Blended(font, "Exit", color);
	exit_text = res_manager.AddSurface("Exit", surf);
	TTF_CloseFont(font);
}

void StartScreen::Update(float dt, uint32_t input) {

}
void StartScreen::Render(float dt, SDL_Renderer* renderer) {
	int width, height;
	SDL_GetRendererOutputSize(renderer, &width, &height);

	SDL_Rect rect;
	SDL_RenderCopy(renderer, background.ptr, 0, 0);
	rect.w = play_text.width;
	rect.h = play_text.height;
	rect.x = width / 2 - rect.w / 2;
	rect.y = 300;
	SDL_RenderCopy(renderer, play_text.ptr, 0, &rect);
	rect.y += rect.h;
	SDL_RenderCopy(renderer, exit_text.ptr, 0, &rect);
}