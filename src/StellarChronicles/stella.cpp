#include "StellarChronicles/stella.h"

asteroid::asteroid::asteroid(b2World* world, const sprites& sprite, float radius, const b2Vec2& position, float mass)
	:world(world), sprite(sprite)
{	
	b2BodyDef asteroidDef;
	asteroidDef.type = b2_dynamicBody;
	asteroidDef.userData.pointer = reinterpret_cast<uintptr_t> (this);
	asteroidDef.position = position;
	body= world->CreateBody(&asteroidDef);
	b2CircleShape Shape;
	Shape.m_radius = radius;
	b2FixtureDef bodFix;
	bodFix.shape = &Shape;
	body->CreateFixture(&bodFix);
	b2MassData massData = { mass,b2Vec2_zero,2 * mass * radius * radius / 5.0f };
	body->SetMassData(&massData);
	//Shape.m_radius = gravite;
	//bodFix.shape = &Shape;
	//bodFix.isSensor = true;
	//body->CreateFixture(&bodFix);

	this-> sprite.scale = radius *100/ sprite.size.x;
}

asteroid::~asteroid()
{
	world->DestroyBody(body);
}

void asteroid::draw(SDL_Renderer* renderer, const camera& camera)
{
	// 把物理世界的坐标转换成SDL渲染的坐标
	auto position = body->GetPosition() - camera.position;
	position = {
		position.x * SDL_cosf(camera.angle) - position.y * SDL_sinf(camera.angle)
		,position.x * SDL_sinf(camera.angle) + position.y * SDL_cosf(camera.angle) 
	};
	SDL_Point pos = { static_cast<int>(position.x *camera.scale* 50) + 960, static_cast<int>(position.y* camera.scale * 50) + 540 };
	
	//更新贴图的角度
	sprite.angle = body->GetAngle() * 180 / M_PI;
	
	sprite.draw(renderer, pos , camera.scale);
}

b2Vec2 asteroid::computeForce()
{
	b2Vec2 force = { 0.0f,0.0f };
	for (auto& a : aroundAsteroids)
	{
		auto mass = a->body->GetMass();
		auto vec = a->body->GetPosition() - this->body->GetPosition();
		auto distance = vec.Normalize();
		force += body->GetMass() * mass / (distance * distance) * vec;
	}
	return force;
}
