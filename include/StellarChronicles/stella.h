#pragma once
#include "StellarChronicles/sprites.h"
#include "StellarChronicles/camera.h"
#include <vector>

class galaxy;
class QuadTree;
class stella
{
public:
	stella(float mass, float radius, sprites &sprite, galaxy *owner);
	~stella() = default;
	friend class galaxy;
	float radius;

private:
	float mass;

	float spinAng = 0.0f;
	float spinVel = 0.0f;

	sprites sprite;
	galaxy *owner;
};

class galaxy
{
public:
	galaxy(vec2 position, float mass, float radius, sprites &sprite);
	friend class QuadTree;
	// 作为子星系的属性
	galaxy *owner = nullptr;
	float orbitRadius = 0.0f;
	float orbitAng = 0.0f;
	float orbitAngVel = 0.0f;
	float orbitAngAcc = 0.0f;
	bool isUpdated = false;

	stella mainStar;
	std::vector<galaxy *> satellites;

	void PhysicStep(float time);
	void linkSubGalaxy(galaxy *subGalaxy);
	void removeSubGalaxy(galaxy *subGalaxy);
	void leaveGalaxy();
	void applyAccleration(const vec2 &acc);
	void applyImpulse(const vec2 &imp);
	void applyDisplace(const vec2 &disp);
	void update();
	void draw(SDL_Renderer *renderer, camera &camera);
	void contactProcess(QuadTree &tree);
	void gravitationProcess(QuadTree &tree);
	bool isGalaxyInSatellites(galaxy *subgalaxy);
	vec2 getPosition() const;
	vec2 getVelocity() const;
	void telePosition(const vec2 &location);
	galaxy *destroy();
	enum class Type
	{
		asiderite,
		planet,
		star
	} type = Type::asiderite;
	bool isPlayer = false;
	enum class State
	{
		Destroyed,
		Alive,
		Dead
	} state = State::Alive;

private:
	vec2 Position;
	vec2 Velocity;
	vec2 Accleration;
	int life = 2;
};
