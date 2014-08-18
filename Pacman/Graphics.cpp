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


GraphText::GraphText() {
	this->dirty = true;
}
GraphText::~GraphText() {
	if(tex.ptr)
		SDL_DestroyTexture(tex.ptr);
}
void GraphText::UpdateText(const std::string& str) {
	if(str != this->text)
	{
		this->text = str;
		this->dirty = true;
	}
}
Texture GraphText::GetTexture(SDL_Renderer* renderer) {
	if(dirty) {
		SDL_Color clr;
		clr.a = 255; clr.r = 255; clr.g = 255; clr.b = 255;
		SDL_Surface* s = TTF_RenderText_Blended_Wrapped(font, text.c_str(), clr, 600);
		if(!s) {
			return tex;
		}
		if(tex.ptr)
			SDL_DestroyTexture(tex.ptr);
		tex.ptr = SDL_CreateTextureFromSurface(renderer, s);
		tex.height = s->h;
		tex.width = s->w;
		SDL_FreeSurface(s);
		dirty = false;
	}
	return tex;
}
void GraphText::SetFont(TTF_Font* f) {
	font = f;
}

TTF_Font* FontManager::GetFont(const char* name, int size) {
	auto i = fonts.find(name);
	if(i != fonts.end()) {
		auto j = i->second.find(size);
		if(j != i->second.end()) {
			return j->second;
		}
	}
	TTF_Font* ttf_f = TTF_OpenFont((prefix + name).c_str(), size);
	if(!ttf_f)
		return 0;
	fonts[name][size] = ttf_f;
	return ttf_f;
}
void FontManager::Cleanup() {
	auto i = fonts.begin();
	auto end = fonts.end();
	for(; i != end; ++i) {
		auto i1 = i->second.begin();
		auto end1 = i->second.end();
		for(; i1 != end1; ++i1) {
			TTF_CloseFont(i1->second);
		}
	}
	fonts.clear();
}
void FontManager::SetPrefix(const std::string& p) {
	prefix = p;
}