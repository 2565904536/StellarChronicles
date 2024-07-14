#pragma once
#include "StellarChronicles/sprites.h"
#include "StellarChronicles/camera.h"
#include <vector>

class galaxy;

class stella
{
public:
	stella(float mass, float radius, sprites& sprite,galaxy* owner);
	~stella() = default;
	friend class galaxy;
private:

	float mass;
	float radius;
	float spinAng=0.0f;
	float spinVel=0.0f;

	sprites sprite;
	galaxy* owner;
};

class galaxy
{
public:
	galaxy(vec2 position, float mass, float radius, sprites& sprite);

	//cur���ǵ�ǰ��fut����δ��
	//curPosition��Ϊ��ȷ��satellites��״̬
	vec2 Position;
	vec2 Velocity;
	//��Ϊ����ϵ������
	galaxy* owner= nullptr;
	float orbitRadius=0.0f;
	float orbitAng=0.0f;
	float orbitAngVel=0.0f;
	bool isUpdated = false;


	stella mainStar;
	std::vector<galaxy*> satellites;

	void applyOrbitConstraints(float time);
	void linkSubGalaxy(galaxy* subGalaxy);
	void removeSubGalaxy(galaxy* subGalaxy);
	void update();
	void draw(SDL_Renderer *renderer, camera &camera);
private:

};