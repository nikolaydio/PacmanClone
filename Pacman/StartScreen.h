#pragma once


#include "GameLogic.h"
#include "Graphics.h"

#define MENU_ENTRY_COUNT 3
class StartScreen : public GameLogic {
	Texture background;

	Texture menu_entries[MENU_ENTRY_COUNT];
	int selected;
public:
	void Init(ResourceManager& res_manager);

	void Update(float dt, uint32_t input_state, uint32_t input_events, PacmanApp* app);
	void Render(float dt, SDL_Renderer* renderer);
};