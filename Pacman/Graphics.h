#pragma once

#include <SDL.h>
#undef main
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