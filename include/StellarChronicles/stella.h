#pragma once
#include "box2d/box2d.h"
#include "StellarChronicles/manager.h"
#include "StellarChronicles/sprites.h"

class asteroid
{
public:
	asteroid(sprites* sprite,float radius,b2Vec2 position,float gravite);
	~asteroid();
	sprites* sprite;
	b2Body* body;

private:

};
