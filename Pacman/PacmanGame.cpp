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
bool is_walkable(uint8_t ch) {
	if(ch == TT_BLOCK)
		return false;
	return true;
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
	tile_size = 20;
}
void PacmanGame::Init(ResourceManager& res_manager) {
	tiles[TT_BLOCK].tex = res_manager.GetTexture("block-temp.png");
	tiles[TT_EMPTY].tex = res_manager.GetTexture("empty-tile1.png");
	tiles[TT_FOOD].tex = res_manager.GetTexture("food.png");
	tiles[TT_PACMAN].tex = res_manager.GetTexture("empty-tile1.png");
	tiles[TT_GHOST].tex = res_manager.GetTexture("empty-tile1.png");
	sprite_sheet = res_manager.GetTexture("sprite_sheet.png");
}
void PacmanGame::SetTilemap(TileMap* map) {
	delete tile_map;
	tile_map = map;
	//this is also the "reset" function
	pacman.current_state = 0;
	pacman.ori = Vector2df(-1, 0);
	pacman.next_ori = pacman.ori;
	pacman.sheet_offset = Vector2d(96, 0);
	pacman.sheet_stride = Vector2d(0, 16 * 2);
	pacman.sprite_size = Vector2d(15, 16);
	
	Vector2d tile = tile_map->Search(TT_PACMAN);
	pacman.position = Vector2df(tile.x * tile_size, tile.y * tile_size);
	pacman.target_pos = pacman.position;
}

void PacmanGame::Update(float dt, uint32_t input_state, uint32_t input_events, PacmanApp* app) {
	if(input_events & INPUT_ESC) {
		app->LoadStartScreen();
		return;
	}
	if(input_state & INPUT_LEFT) {
		pacman.next_ori.x = -1;
		pacman.next_ori.y = 0;
	}else if(input_state & INPUT_RIGHT) {
		pacman.next_ori.x = 1;
		pacman.next_ori.y = 0;
	}else if(input_state & INPUT_UP) {
		pacman.next_ori.y = -1;
		pacman.next_ori.x = 0;
	}else if(input_state & INPUT_DOWN) {
		pacman.next_ori.y = 1;
		pacman.next_ori.x = 0;
	}

	Vector2df pacman_center = pacman.position + Vector2df(tile_size / 2, tile_size / 2);
	Vector2df pacman_tile = pacman_center * (1.0 / tile_size);
	pacman_tile.x = floor(pacman_tile.x);
	pacman_tile.y = floor(pacman_tile.y);

	//pacman movement code
	if(pacman.target_pos.x == pacman.position.x
		&& pacman.target_pos.y == pacman.position.y) {

			Vector2df next_tile = pacman_tile + pacman.next_ori;
			if(is_walkable(tile_map->GetTile(next_tile.x, next_tile.y))) {
				pacman.target_pos = next_tile * tile_size;
				pacman.ori = pacman.next_ori;
			}else{
				next_tile = pacman_tile + pacman.ori;
				if(is_walkable(tile_map->GetTile(next_tile.x, next_tile.y))) {
					pacman.target_pos = next_tile * tile_size;
				}
			}
	}

	//food collection
	if(tile_map->GetTile(pacman_tile.x, pacman_tile.y) == TT_FOOD) {
		tile_map->SetTile(pacman_tile.x, pacman_tile.y, TT_EMPTY);
	}


	Vector2df velo = pacman.target_pos - pacman.position;
	float dist = velo.len();
	pacman.position = pacman.position + velo.normalized() * std::min(dist, 100 * dt);
}
void PacmanGame::Render(float dt, SDL_Renderer* renderer) {
	if(!tile_map) { 
		return;
	}
	SDL_Rect rect;
	SDL_Rect src;
	for(int x = 0; x < tile_map->GetWidth(); ++x) {
		for(int y = 0; y < tile_map->GetHeight(); ++y) {
			rect.x = x * tile_size;
			rect.y = y * tile_size;
			rect.w = tile_size;
			rect.h = tile_size;
			SDL_RenderCopy(renderer, tiles[tile_map->GetTile(x, y)].tex.ptr, 0, &rect);
		}
	}
	pacman.current_state += dt * 5;
	if(pacman.current_state > 3) {
		pacman.current_state = pacman.current_state - 6;
	}
	Vector2d src_pos = pacman.sheet_offset + pacman.sheet_stride * (int)abs(pacman.current_state);
	src.x = src_pos.x;
	src.y = src_pos.y;
	src.w = pacman.sprite_size.x;
	src.h = pacman.sprite_size.y;
	rect.x = pacman.position.x;
	rect.y = pacman.position.y;
	rect.w = tile_size;
	rect.h = tile_size;

	SDL_RenderCopy(renderer, sprite_sheet.ptr, &src, &rect);
}