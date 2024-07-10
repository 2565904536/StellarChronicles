#pragma once
#include "box2d/box2d.h"
#include "SDL2/SDL.h"

class camera
{
public:
	camera(b2Vec2 position,float scale,float angle);
	camera() = default;
	b2Vec2 position = { 0.0f,0.0f };
	float scale = 1.0f;
	float angle = 0.0f;
};