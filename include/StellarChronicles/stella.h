#pragma once
#include "box2d/box2d.h"
#include "StellarChronicles/manager.h"
#include "StellarChronicles/sprites.h"
#include "StellarChronicles/camera.h"
#include <vector>

class asteroid
{
public:
	asteroid(b2World* world, const sprites& sprite,float radius,const b2Vec2 &position,float mass);
	~asteroid();
	b2World* world;
	sprites sprite;
	b2Body* body;
	void draw(SDL_Renderer* renderer,const camera &camera);
	//在每帧物理解算前要记得调用std::vector::clear();
	std::vector<asteroid*> aroundAsteroids;
	b2Vec2 computeForce();
private:

};
