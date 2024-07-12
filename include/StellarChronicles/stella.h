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
	//������ϵ
	stella mainStella;
	//������ϵ
	std::vector<galaxy*> satellites;

	//std::vector<b2Vec3> attributes;

	galaxy* belongsTo = nullptr;

	std::vector<galaxy*> aroundGalaxies;

	b2Vec2 calculateGravitationalAcceleration(float gravitationalConstant);
	void applyOrbitConstraints(float inv_dt);
	bool addSubGalaxy(galaxy* subGalaxy);
	bool removeSubGalaxy(galaxy* subGalaxy);
	void destroy();
	void applyMainStellarAcceleration(b2Vec2 acceleration);
	void applystallitesAcceleration(b2Vec2 acceleration);
	void draw(SDL_Renderer *renderer, camera &camera);
	bool visible = true;
	b2Joint* OrbitalLinkage = nullptr;
private:
	enum class State
	{
		ALIVE,
		DESTROYED
	} state;
	int level = 0;
};


