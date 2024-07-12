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
		galaxy* belongsTo;
	};

	galaxy(b2World* world, const sprites& sprite, float radius, const b2Vec2& position, float mass);
	~galaxy();
	//中心星系
	stella mainStella;
	//卫星星系
	std::vector<galaxy*> satellites;

	//std::vector<b2Vec3> attributes;

	galaxy* belongsTo = nullptr;

	std::vector<galaxy*> aroundGalaxies;

	std::vector<galaxy*> collisionGalaxies;

	b2Vec2 calculateGravitationalAcceleration(float gravitationalConstant);
	void applyOrbitConstraints(float inv_dt);
	bool linkSubGalaxy(galaxy* subGalaxy);
	bool removeSubGalaxy(galaxy* subGalaxy);
	void destroy();
	void applyMainStellarAcceleration(b2Vec2 acceleration);
	void applystallitesAcceleration(b2Vec2 acceleration);
	void draw(SDL_Renderer *renderer, camera &camera);
	bool visible = true;
	b2Joint* OrbitalLinkage = nullptr;
	void addSubGalaxy();
private:
	enum class State
	{
		ALIVE,
		DESTROYED
	} state;
	int level = 0;
};


