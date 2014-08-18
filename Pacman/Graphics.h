#pragma once

#include <SDL.h>
#undef main
#include <SDL_ttf.h>
#include <unordered_map>
#include <string>

class Texture {
public:
	SDL_Texture* ptr;
	int width;
	int height;
	Texture() { ptr = 0; width = 0; height = 0; }
	Texture& operator=(const Texture& sec);
};


class ResourceManager {
	Texture loadTexture(const std::string& fn) const;

	SDL_Renderer* ren;
	int fails;
	std::unordered_map<std::string, Texture> textures;
	std::string prefix;
public:
	ResourceManager();
	void Init(SDL_Renderer* renderer, char* prefix);
	Texture GetTexture(const std::string& fn);
	int HasFailed();
	void AddTexture(const std::string& key, Texture tex);
	Texture AddSurface(const std::string& key, SDL_Surface* surf);
	void Cleanup();
};

class FontManager {
	std::unordered_map<std::string, std::unordered_map<int, TTF_Font*>> fonts;
	std::string prefix;
public:
	TTF_Font* GetFont(const char* name, int size);
	void Cleanup();
	void SetPrefix(const std::string& p);
};

class GraphText {
	std::string text;
	bool dirty;
	TTF_Font* font;
	Texture tex;
public:
	GraphText();
	~GraphText();
	void UpdateText(const std::string& str);
	Texture GetTexture(SDL_Renderer* renderer);
	void SetFont(TTF_Font* f);
};