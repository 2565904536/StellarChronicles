#include "StellarChronicles/sprites.h"

sprites::sprites(SDL_Texture *texture, SDL_Point frameSize, Uint32 frameNum,  float angle, float scale,SDL_RendererFlip flip)
	: texture(texture), frameSize(frameSize), frameNum(frameNum), angle(angle), scale(scale), flip(flip), index(0)
{
	int Ox = 0;
	int Oy = 0;
	SDL_QueryTexture(texture, nullptr, nullptr, &Ox, &Oy);
	size = {Ox / frameSize.x, Oy / frameSize.y};

}

void sprites::draw(SDL_Renderer *renderer, SDL_Point position,float visScale)
{
	if (index == 1)
		int a = 1;

	int x = index / frameSize.x;
	int y = index % frameSize.x;
	SDL_Point center = {size.x / 2, size.y / 2};
	SDL_Rect srcArea{y * size.x, x * size.y, size.x, size.y};
	auto totalScale = scale * visScale;
	SDL_Rect dstArea{position.x - center.x * totalScale, position.y - center.y * totalScale, size.x * totalScale, size.y * totalScale };
	SDL_Point scaledCenter{ center.x * totalScale,center.y *totalScale };
	SDL_RenderCopyEx(renderer, texture, &srcArea, &dstArea, angle, &scaledCenter, flip);
}

void sprites::nextFrame()
{
	index++;
	if (index >= frameNum)
		index = index % frameNum;
}
