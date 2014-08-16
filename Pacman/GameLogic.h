#pragma once
#include <cstdint>

enum LOGIC_INPUT_MASK {
	INPUT_LEFT = 1,
	INPUT_RIGHT = 2,
	INPUT_UP = 4,
	INPUT_DOWN = 8,
	INPUT_FIRE = 16,
	INPUT_ESC = 32
};
struct SDL_Renderer;
class GameLogic {
public:
	virtual void Update(float dt, uint32_t input) = 0;
	virtual void Render(float dt, SDL_Renderer* renderer) = 0;
};
