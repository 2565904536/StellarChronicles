#pragma once
#include "SDL2/SDL.h"
#include <string_view>
class game
{
	SDL_Window *window;

public:
	game(std::string_view name, int x, int y, int w, int h, Uint32 flags);
	~game() = default;
	SDL_Renderer *renderer;
	SDL_Event windowEvent;
};