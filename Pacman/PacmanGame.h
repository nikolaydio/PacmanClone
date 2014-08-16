#pragma once
#include <cstdint>
#include <cassert>



enum TILE_TYPE {
	BLOCK,
	EMPTY,
	FOOD,
	SPAWN
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
};


class PacmanGame {
	TileMap* tile_map;
public:
	PacmanGame() {
		tile_map = 0;
	}
	void SetTilemap(TileMap* map) {
		delete tile_map;
		tile_map = map;
	}
	void Init() {

	}
	void Update(float dt) {

	}
	void Done() {

	}
};