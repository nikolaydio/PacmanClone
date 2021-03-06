#pragma once
#include <cstdint>
#include <cassert>
#include "GameLogic.h"
#include "Graphics.h"
#include <cmath>


enum TILE_TYPE {
	TT_BLOCK,
	TT_EMPTY,
	TT_FOOD,
	TT_PACMAN,
	TT_GHOST,
	TT_TILE_COUNT
};
enum GHOST_TYPE {
	GT_RED,
	GT_PINK,
	GT_BLUE,
	GT_ORANGE,
	GT_COUNT
};
enum GHOST_MODE {
	SCATTER,
	CHASE
};
#define GHOST_SPEED 2.7f
#define PACMAN_SPEED 3.0f
struct Vector2df {
	Vector2df() {}
	Vector2df(float x_, float y_) : x(x_), y(y_) {}
	Vector2df operator+(const Vector2df& sec) const {
		return Vector2df(x + sec.x, y + sec.y);
	}
	Vector2df operator-(const Vector2df& sec) const {
		return Vector2df(x - sec.x, y - sec.y);
	}
	Vector2df operator*(float factor) const {
		return Vector2df(x * factor, y * factor);
	}
	float len() const {
		return sqrt(x*x + y*y);
	}
	Vector2df normalized() const {
		float l = len();
		if(l == 0)
			l = 1;
		return Vector2df(x / l, y / l);
	}
	bool operator!=(const Vector2df& vec) const {
		if(x != vec.x)
			return true;
		if(y != vec.y)
			return true;
		return false;
	}
	bool operator==(const Vector2df& vec) const {
		return !((*this)!=vec);
	}
	Vector2df operator-() const {
		return Vector2df(-x, -y);
	}

	float x, y;
};
struct Vector2d {
	Vector2d() {}
	Vector2d(const Vector2df& v) : x(v.x), y(v.y) {}
	Vector2d(int x_, int y_) : x(x_), y(y_) {}
	Vector2d operator+(const Vector2d& sec) const {
		return Vector2d(x + sec.x, y + sec.y);
	}
	Vector2d operator*(int factor) const {
		return Vector2d(x * factor, y * factor);
	}
	Vector2df operator*(float factor) const {
		return Vector2df(x * factor, y * factor);
	}
	bool operator!=(const Vector2d& vec) const {
		if(x != vec.x)
			return true;
		if(y != vec.y)
			return true;
		return false;
	}
	int x, y;
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
	Vector2d Search(uint8_t tile) {
		Vector2d pos;
		for(pos.x = 0; pos.x < width_; ++pos.x) {
			for(pos.y = 0; pos.y < height_; ++pos.y) {
				if(GetTile(pos.x, pos.y) == tile) 
					return pos;
			}
		}
		return pos;
	}
	int Count(uint8_t tile) {
		int count = 0;
		for(int i = 0; i < width_; ++i) {
			for(int u = 0 ; u < height_; ++u) {
				if(tile == GetTile(i, u))
					++count;
			}
		}
		return count;
	}
	int GetWidth() { return width_; }
	int GetHeight() { return height_;}
};

TileMap* LoadTileMap(const char* filename);

enum ORIENTATION {
	UP, DOWN, LEFT, RIGHT
};
class PacmanGame : public GameLogic {
	TileMap* tile_map;
	Texture sprite_sheet;
	struct TileData {
		Texture tex;
	};
	struct Pacman {
		Vector2d sheet_offset;
		Vector2d sheet_stride;
		Vector2d sprite_size;
		float current_state;

		Vector2df position;
		Vector2df target_pos;
		//the current way pacaman is going
		Vector2df ori;
		//next one
		Vector2df next_ori;
	};
	struct Ghost {
		Vector2df target_tile;
		Vector2df position;
		Vector2df short_target;
		Vector2d ori;

		Vector2df sheet_offset;
		Vector2df sheet_stride;
		Vector2df sprite_size;

		Vector2d scatter_tile;
	};
	TileData tiles[TT_TILE_COUNT];

	int tile_size;

	Pacman pacman;
	Ghost ghost[GT_COUNT];

	int food_left;
	int score;
	GraphText score_text;
	FontManager font_man;
	GHOST_MODE mode;
	float time_in_mode;
	

	void UpdatePacman(Vector2df pacman_tile, float dt);
	void UpdateGhost(Ghost& gh, float dt);
public:

	PacmanGame();
	void Init(ResourceManager& res_manager);
	void SetTilemap(TileMap* map);

	void Update(float dt, uint32_t input_state, uint32_t input_events, PacmanApp* app);
	void Render(float dt, SDL_Renderer* renderer);
};