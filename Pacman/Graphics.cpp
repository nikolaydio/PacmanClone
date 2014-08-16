#include "Graphics.h"
#include <SDL_image.h>

Texture& Texture::operator=(const Texture& sec) {
	ptr = sec.ptr;
	width = sec.width;
	height = sec.height;
	return *this;
}

Texture ResourceManager::loadTexture(const std::string& fn) const {
	Texture tex;
	SDL_Surface* img = IMG_Load((prefix + fn).c_str());
	if(img != 0) {

		tex.ptr = SDL_CreateTextureFromSurface(ren, img);
		tex.width = img->w;
		tex.height = img->h;
		SDL_FreeSurface(img);
		if(!tex.ptr) {
			printf("Failed to convert surface of %s", fn.c_str());
		}
	}else {
		printf("Failed to load %s", fn.c_str());
	}
	return tex;
}
ResourceManager::ResourceManager() : fails(0) {}
void ResourceManager::Init(SDL_Renderer* renderer, char* prefix) {
	ren = renderer;
	this->prefix = prefix;
}
Texture ResourceManager::GetTexture(const std::string& fn) {
	auto i = textures.find(fn);
	if(i != textures.end()) {
		return (*i).second;
	}
	Texture tex = loadTexture(fn);
	if(tex.ptr != 0) {
		textures[fn] = tex;
	}
	if(tex.ptr == 0)
		++fails;
	return tex;
}
int ResourceManager::HasFailed() {
	int fail_count = fails;
	fails = 0;
	return fail_count;
}
void ResourceManager::AddTexture(const std::string& key, Texture tex) {
	textures[key] = tex;
}
Texture ResourceManager::AddSurface(const std::string& key, SDL_Surface* surf) {
	Texture tex;
	if(!surf) {
		tex.ptr = 0;
	}else {
		tex.ptr = SDL_CreateTextureFromSurface(ren, surf);
		tex.width = surf->w;
		tex.height = surf->h;
		SDL_FreeSurface(surf);
		if(tex.ptr) AddTexture(key, tex);
	}
	return tex;
}
void ResourceManager::Cleanup() {
	for(auto i = textures.begin(), end = textures.end(); i != end; ++i) {
		SDL_DestroyTexture(i->second.ptr);
	}
	textures.clear();
}