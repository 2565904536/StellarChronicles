#include "StellarChronicles/manager.h"

manager::~manager()
{
	for (auto &[_, texture] : textures)
	{
		SDL_DestroyTexture(texture);
	}
}

bool manager::LoadTex(SDL_Renderer *renderer, std::string_view Path, std::string_view Name)
{
	SDL_Texture *texture = IMG_LoadTexture(renderer, Path.data());
	if (!texture)
		return false;
	textures.insert(std::make_pair(Name.data(), texture));
	return true;
}

SDL_Texture *manager::GetTex(std::string_view Name)
{
	return textures.at(Name);
}