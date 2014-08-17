#include "PacmanGame.h"
#include <fstream>
#include <string>
#include <vector>
#include "PacmanApp.h"

bool is_new_line(char ch) {
	return ch == '\n' || ch == '\r';
}
uint8_t char_to_tile(char ch) {
	if(ch == 'B') {
		return TT_BLOCK;
	}else if(ch == ' ') {
		return TT_EMPTY;
	}else if(ch == 'P') {
		return TT_PACMAN;
	}else if(ch == 'G') {
		return TT_GHOST;
	}else if(ch == 'F') {
		return TT_FOOD;
	}
	return 255;
}

TileMap* LoadTileMap(const char* filename) {
	std::ifstream file(filename);
	if(!file.is_open()) {
		return 0;
	}
	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<unsigned char> buffer(size);
	if (file.read((char*)buffer.data(), size))
	{
		return 0;
	}

	int width = 0, height = 0;
	//extract width
	for(int i = 0; i < buffer.size(); ++i) {
		if(is_new_line(buffer[i])) {
			break;
		}
		width++;
	}
	//extract height
	for(int i = 0; i < buffer.size(); ++i) {
		if(is_new_line(buffer[i])) {
			height++;
		}
	}

	if(width <= 0 || height <= 0) {
		return 0;
	}

	TileMap* map = new TileMap(width, height);
	int x = 0, y = 0;
	for(int i = 0; i < buffer.size(); ++i) {
		if(is_new_line(buffer[i])) {
			y++;
			if(y >= height) break;
			x = 0;
		}else{
			uint8_t t = char_to_tile(buffer[i]);
			map->SetTile(x, y, t);
			x++;
		}
	}
	return map;
}

PacmanGame::PacmanGame() {
	tile_map = 0;
}
void PacmanGame::Init(ResourceManager& res_manager) {
	tiles[TT_BLOCK].tex = res_manager.GetTexture("block-temp.png");
	tiles[TT_EMPTY].tex = res_manager.GetTexture("empty-tile1.png");
	tiles[TT_FOOD].tex = res_manager.GetTexture("empty-tile1.png");
	tiles[TT_PACMAN].tex = res_manager.GetTexture("empty-tile1.png");
	tiles[TT_GHOST].tex = res_manager.GetTexture("empty-tile1.png");
}
void PacmanGame::SetTilemap(TileMap* map) {
	delete tile_map;
	tile_map = map;
}

void PacmanGame::Update(float dt, uint32_t input_state, uint32_t input_events, PacmanApp* app) {
	if(input_events & INPUT_ESC) {
		app->LoadStartScreen();
	}
	
}
void PacmanGame::Render(float dt, SDL_Renderer* renderer) {
	if(!tile_map) {
		return;
	}
	SDL_Rect rect;
	for(int x = 0; x < tile_map->GetWidth(); ++x) {
		for(int y = 0; y < tile_map->GetHeight(); ++y) {
			rect.x = x * 20;
			rect.y = y * 20;
			rect.w = 20;
			rect.h = 20;
			SDL_RenderCopy(renderer, tiles[tile_map->GetTile(x, y)].tex.ptr, 0, &rect);
		}
	}
}