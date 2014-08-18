#include "PacmanGame.h"
#include <fstream>
#include <string>
#include <vector>
#include "PacmanApp.h"
#include <sstream>

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
	}else if(ch == '!') {
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
}
void PacmanGame::Init(ResourceManager& res_manager) {
	tiles[TT_BLOCK].tex = res_manager.GetTexture("block-temp.png");
	tiles[TT_EMPTY].tex = res_manager.GetTexture("empty-tile1.png");
	tiles[TT_FOOD].tex = res_manager.GetTexture("food.png");
	tiles[TT_PACMAN].tex = res_manager.GetTexture("empty-tile1.png");
	tiles[TT_GHOST].tex = res_manager.GetTexture("empty-tile1.png");
	sprite_sheet = res_manager.GetTexture("sprite_sheet.png");
}
Vector2df tile_to_pos(Vector2d tile, float tile_size) {
	return Vector2df(tile.x * tile_size, tile.y * tile_size);
}
void PacmanGame::SetTilemap(TileMap* map) {
	delete tile_map;
	tile_map = map;

	tile_size = 640 / tile_map->GetWidth();
	tile_size = std::min(tile_size, 380 / tile_map->GetHeight());

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
	food_left = tile_map->Count(TT_FOOD);
	score = 0;
	score_text.UpdateText("Score: 0");
	score_text.SetFont(font_man.GetFont("micross.ttf", 24));

	//init ghosts
	Vector2d ghost_spawn = tile_map->Search(TT_GHOST);
	ghost[GT_RED].position = tile_to_pos(ghost_spawn + Vector2d(0, -1), tile_size);
	ghost[GT_RED].scatter_tile = Vector2d(tile_map->GetWidth(), 0);

	ghost[GT_PINK].position = tile_to_pos(ghost_spawn + Vector2d(0, 0), tile_size);
	ghost[GT_PINK].scatter_tile = Vector2d(0, 0);

	ghost[GT_BLUE].position = tile_to_pos(ghost_spawn + Vector2d(-1, 0), tile_size);
	ghost[GT_BLUE].scatter_tile = Vector2d(tile_map->GetWidth(), tile_map->GetHeight());

	ghost[GT_ORANGE].position = tile_to_pos(ghost_spawn + Vector2d(1, 0), tile_size);
	ghost[GT_ORANGE].scatter_tile = Vector2d(0, tile_map->GetHeight());

	for(int i = 0; i < GT_COUNT; ++i) {
		ghost[i].sprite_size = Vector2df(16,16);
		ghost[i].sheet_offset = Vector2df(16 * i, 0);
		ghost[i].short_target = ghost[i].position;
		ghost[i].ori = Vector2df(0, -1);
		ghost[i].target_tile = Vector2df(ghost[i].scatter_tile.x, ghost[i].scatter_tile.y);
	}
	mode = CHASE;
	time_in_mode = 0;
 
}
Vector2df GetTileFromPosition(Vector2df pos, float tile_size) {
	Vector2df r;
	r.x = floor((pos.x + tile_size / 2) / tile_size);
	r.y = floor((pos.y + tile_size / 2) / tile_size);
	return r;
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


	//update ghosts
	if(time_in_mode > 10) {
		time_in_mode = 0;
		if(mode == SCATTER)
			mode = CHASE;
		else
			mode = SCATTER;
	}
	time_in_mode += dt;

	for(int i = 0; i < GT_COUNT; ++i) {
		UpdateGhost(ghost[i], dt);
		Vector2d tile = GetTileFromPosition(ghost[i].position, tile_size);
		if(tile.x == pacman_tile.x && tile.y == pacman_tile.y)
			app->LoadStartScreen();
	}
	if(mode == SCATTER) {
		for(int i = 0; i < GT_COUNT; ++i) {
			ghost[i].target_tile = Vector2df(ghost[i].scatter_tile.x, ghost[i].scatter_tile.y);
		}
	}else{
		ghost[GT_RED].target_tile = pacman_tile;
		ghost[GT_PINK].target_tile = pacman_tile + pacman.ori * 4;
		Vector2df red_tile = GetTileFromPosition(ghost[GT_RED].position, tile_size);
		ghost[GT_BLUE].target_tile = red_tile + ((pacman_tile + pacman.ori * 2) - red_tile) * 2;
		if((pacman.position - ghost[GT_ORANGE].position).len() < tile_size * 8) {
			ghost[GT_ORANGE].target_tile = Vector2df(ghost[GT_ORANGE].scatter_tile.x, ghost[GT_ORANGE].scatter_tile.y);
		}else{
			ghost[GT_ORANGE].target_tile = pacman_tile;
		}
	}

	//food collection
	if(tile_map->GetTile(pacman_tile.x, pacman_tile.y) == TT_FOOD) {
		tile_map->SetTile(pacman_tile.x, pacman_tile.y, TT_EMPTY);
		food_left--;
		score += 10;

		std::stringstream s;
		s << "Score: ";
		s << score;
		score_text.UpdateText(s.str());
		if(food_left == 0) {
			app->LoadStartScreen();
		}
	}

	UpdatePacman(pacman_tile, dt);
}
void PacmanGame::UpdatePacman(Vector2df pacman_tile, float dt) {
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
	Vector2df velo = pacman.target_pos - pacman.position;
	float dist = velo.len();
	pacman.position = pacman.position + velo.normalized() * std::min(dist, 100 * dt);
}
void PacmanGame::UpdateGhost(Ghost& gh, float dt) {
	if(gh.short_target.x == gh.position.x) {
		if(gh.short_target.y == gh.position.y) {
			Vector2d potential_tiles[3];
			Vector2d ghost_tile;
			Vector2df ghost_center = gh.position + Vector2df(tile_size / 2, tile_size / 2);
			ghost_tile.x = floor(ghost_center.x / tile_size);
			ghost_tile.y = floor(ghost_center.y / tile_size);

			backward:
			int potentials = 0;
			//check left
			if(Vector2d(1, 0) != gh.ori) {
				Vector2d v = Vector2d(-1, 0) + ghost_tile;
				if(is_walkable(tile_map->GetTile(v.x, v.y)))
					potential_tiles[potentials++] = v;
			}
			//check right
			if(Vector2d(-1, 0) != gh.ori)  {
				Vector2d v = Vector2d(1, 0) + ghost_tile;
				if(is_walkable(tile_map->GetTile(v.x, v.y)))
					potential_tiles[potentials++] = v;
			}
			//check up
			if(Vector2d(0, 1) != gh.ori) {
				Vector2d v = Vector2d(0, -1) + ghost_tile;
				if(is_walkable(tile_map->GetTile(v.x, v.y)))
					potential_tiles[potentials++] = v;
			}
			//check down
			if(Vector2d(0, -1) != gh.ori)  {
				Vector2d v = Vector2d(0, 1) + ghost_tile;
				if(is_walkable(tile_map->GetTile(v.x, v.y)))
					potential_tiles[potentials++] = v;
			}
			if(potentials == 0) {
				gh.ori = Vector2df(-gh.ori.x, -gh.ori.y);
				goto backward;
			}

			Vector2d best = potential_tiles[0];
			for(int u = 1; u < potentials; ++u) {
				float curr_fitness = (gh.target_tile - Vector2df(potential_tiles[u].x, potential_tiles[u].y)).len();
				float best_fitness = (gh.target_tile - Vector2df(best.x, best.y)).len();
				if(curr_fitness < best_fitness) {
					best = potential_tiles[u];
				}
			}

			//update short term + ori
			gh.short_target = Vector2df(best.x, best.y) * tile_size;
			gh.ori = Vector2d(best.x - ghost_tile.x, best.y - ghost_tile.y);
		}
	}
	Vector2df velo = gh.short_target - gh.position;
	float dist = velo.len();
	gh.position = gh.position + velo.normalized() * std::min(dist, 100 * dt);
}
void PacmanGame::Render(float dt, SDL_Renderer* renderer) {
	if(!tile_map) { 
		return;
	}
	int offset_y = 100;


	SDL_Rect rect;
	SDL_Rect src;
	for(int x = 0; x < tile_map->GetWidth(); ++x) {
		for(int y = 0; y < tile_map->GetHeight(); ++y) {
			rect.x = x * tile_size;
			rect.y = y * tile_size + offset_y;
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
	rect.y = pacman.position.y + offset_y;
	rect.w = tile_size;
	rect.h = tile_size;

	SDL_RenderCopy(renderer, sprite_sheet.ptr, &src, &rect);

	//render ghosts
	for(int i = 0; i < GT_COUNT; ++i) {
		src.x = ghost[i].sheet_offset.x;
		src.y = ghost[i].sheet_offset.y;
		src.w = ghost[i].sprite_size.x;
		src.h = ghost[i].sprite_size.y;
		rect.x = ghost[i].position.x;
		rect.y = ghost[i].position.y + offset_y;
		rect.w = tile_size;
		rect.h = tile_size;
		SDL_RenderCopy(renderer, sprite_sheet.ptr, &src, &rect);
	}

	//render score
	
	rect.x = 0;
	rect.y = 0;
	rect.w = score_text.GetTexture(renderer).width;
	rect.h = score_text.GetTexture(renderer).height;
	SDL_RenderCopy(renderer, score_text.GetTexture(renderer).ptr, 0, &rect);
}