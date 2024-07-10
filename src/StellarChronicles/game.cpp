#include "StellarChronicles/game.h"

game::game(std::string_view name, int x, int y, int w, int h, Uint32 flags)
{
	window = SDL_CreateWindow(name.data(), x, y, w, h, flags);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC| SDL_RENDERER_TARGETTEXTURE);
}