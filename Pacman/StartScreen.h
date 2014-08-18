#pragma once


#include "GameLogic.h"
#include "Graphics.h"
#include <SDL_mixer.h>

#define MENU_ENTRY_COUNT 3
enum MENU_STATUS {
	MS_START,
	MS_SCORE,
	MS_HELP
};
class StartScreen : public GameLogic {
	Texture background;
	Mix_Music* background_music;
	Texture menu_entries[MENU_ENTRY_COUNT];
	int selected;
	MENU_STATUS status;
	FontManager fonts;

	GraphText help_text;
	GraphText score_text;
	GraphText game_result_text;
public:
	void Init(ResourceManager& res_manager);
	void Stats(bool victory, int score);

	void Update(float dt, uint32_t input_state, uint32_t input_events, PacmanApp* app);
	void Render(float dt, SDL_Renderer* renderer);
};