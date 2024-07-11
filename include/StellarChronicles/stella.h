#pragma once
#include "box2d/box2d.h"
#include "StellarChronicles/manager.h"
#include "StellarChronicles/sprites.h"
#include "StellarChronicles/camera.h"
#include <vector>

class galaxy
{
public:
	class stella
	{
		stella(b2World* world, const sprites& sprite, float radius, const b2Vec2& position, float mass);
		friend galaxy;
	public:
		~stella();
		sprites sprite;
		b2Body* body;
		b2World* world;
		galaxy* belongsTo;
	};

	galaxy(b2World* world, const sprites& sprite, float radius, const b2Vec2& position, float mass);
	~galaxy();
	//中心星系
	stella mainStella;
	//卫星星系
	std::vector<galaxy*> satellites;
	//卫星星系的属性
	std::vector<b2Vec2> attributes;

	galaxy* belongsTo;

	std::vector<galaxy*> aroundGalaxies;

	b2Vec2 computeForce();
	void addSubGalaxy(galaxy* subGalaxy);
	void destroy();
	void applyOverAllForce(b2Vec2 Force);
	void applyForce(b2Vec2 Force);
	void draw(SDL_Renderer *renderer, camera &camera);
	bool visible;
private:
	enum class State
	{
		ALIVE,
		DESTROYED
	} state;
};


