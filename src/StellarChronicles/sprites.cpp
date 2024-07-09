#include "StellarChronicles/sprites.h"

sprites::sprites(SDL_Texture *texture, SDL_Point frameSize, Uint32 frameNum, SDL_Point position, float angle, float scale, SDL_RendererFlip flip)
	: texture(texture), frameSize(frameSize), frameNum(frameNum), position(position), angle(angle), scale(scale), flip(flip), index(0)
{
	int Ox = 0;
	int Oy = 0;
	SDL_QueryTexture(texture, nullptr, nullptr, &Ox, &Oy);
	size = {Ox / frameSize.x, Oy / frameSize.y};
	center = {size.x / 2, size.y / 2};
}

void sprites::draw(SDL_Renderer *renderer)
{
	int x = index / frameSize.x;
	int y = index % frameSize.y;
	SDL_Rect srcArea{x * size.x, y * size.y, size.x, size.y};
	SDL_Rect dstArea{position.x - center.x * scale, position.y - center.y * scale, size.x * scale, size.y * scale};
	SDL_Point scaledCenter{ center.x * scale,center.y * scale };
	SDL_RenderCopyEx(renderer, texture, &srcArea, &dstArea, angle, &scaledCenter, flip);
}

void sprites::nextFrame()
{
	index++;
	if (index >= frameNum)
		index = index % frameNum;
}
