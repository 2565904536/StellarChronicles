#pragma once
#include "SDL2/SDL_image.h"
#include <map>
#include <string_view>

class manager
{
public:
	manager() = default;
	~manager();
	bool LoadTex(SDL_Renderer *renderer, std::string_view Path, std::string_view Name);
	SDL_Texture *GetTex(std::string_view Name);

private:
	std::map<std::string_view, SDL_Texture *> textures;
};