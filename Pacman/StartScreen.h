#pragma once


#include "GameLogic.h"
#include "Graphics.h"

class StartScreen : public GameLogic {
	Texture background;
	Texture play_text;
	Texture exit_text;
public:
	void Init(ResourceManager& res_manager);

	void Update(float dt, uint32_t input);
	void Render(float dt, SDL_Renderer* renderer);
};