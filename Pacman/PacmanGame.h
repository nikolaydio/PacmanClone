#pragma once
#include <cstdint>
#include <cassert>
#include "GameLogic.h"
#include "Graphics.h"


enum TILE_TYPE {
	TT_BLOCK,
	TT_EMPTY,
	TT_FOOD,
	TT_PACMAN,
	TT_GHOST,
	TT_TILE_COUNT
};
class TileMap {
	uint8_t* tiles_;
	int width_;
	int height_;

	//forbid generating the following funcs
	TileMap& operator=(const TileMap& map) {}
public:
	TileMap(int width, int height) {
		width_ = width;
		height_ = height;
		tiles_ = new uint8_t[width_ * height_];
	}
	~TileMap() {
		delete [] tiles_;
	}
	uint8_t GetTile(int x, int y) {
		assert(x >= 0 && x < width_);
		assert(y >= 0 && y < height_);
		return tiles_[x + y * width_];
	}
	void SetTile(int x, int y, uint8_t tile) {
		assert(x >= 0 && x < width_);
		assert(y >= 0 && y < height_);
		tiles_[x + y * width_] = tile;
	}
	int GetWidth() { return width_; }
	int GetHeight() { return height_;}
};

TileMap* LoadTileMap(const char* filename);

class PacmanGame : public GameLogic {
	TileMap* tile_map;
	struct TileData {
		Texture tex;
	};
	TileData tiles[TT_TILE_COUNT];
public:

	PacmanGame();
	void Init(ResourceManager& res_manager);
	void SetTilemap(TileMap* map);

	void Update(float dt, uint32_t input_state, uint32_t input_events, PacmanApp* app);
	void Render(float dt, SDL_Renderer* renderer);
};