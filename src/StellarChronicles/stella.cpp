#include "StellarChronicles/stella.h"
galaxy::stella::stella(b2World *world, const sprites &sprite, float radius, const b2Vec2 &position, float mass)
	: sprite(sprite), belongsTo(nullptr)
{
	b2BodyDef stellaDef;
	stellaDef.type = b2_dynamicBody;

	stellaDef.position = position;
	body = world->CreateBody(&stellaDef);
	b2CircleShape Shape;
	Shape.m_radius = radius;
	b2FixtureDef bodyFix;
	bodyFix.shape = &Shape;
	body->CreateFixture(&bodyFix);
	b2MassData massData = {mass, b2Vec2_zero, 0.4f * mass * radius * radius};
	body->SetMassData(&massData);
	Shape.m_radius = 10.0f;
	bodyFix.shape = &Shape;
	bodyFix.isSensor = true;
	body->CreateFixture(&bodyFix);

	this->sprite.scale = radius * 100 / sprite.size.x;
}

galaxy::stella::~stella()
{
}

galaxy::galaxy(b2World* world, const sprites& sprite, float radius, const b2Vec2& position, float mass)
	: belongsTo(nullptr), state(State::ALIVE), visible(true),mainStella(world,sprite,radius,position,mass)
{
	mainStella.belongsTo = this;
	mainStella.body->GetUserData().pointer = reinterpret_cast<uintptr_t>(this);
}

galaxy::~galaxy()
{
		destroy();
}

void galaxy::destroy()
{
	if (state == State::ALIVE)
	{
	//body���ٺ󣬴���endcontact,����galaxy�Ƴ�aroundGalaxies�еĴ�galaxy
	//��������body�Ĺؽ�Ҳ�ᱻ����
	this->mainStella.body->GetWorld()->DestroyBody(this->mainStella.body);
	if (this->belongsTo)
	{
		std::erase(this->belongsTo->satellites, this);
		this->belongsTo = nullptr;
	}
	//�������ϵ�������Ƕ���
	for (auto& g : satellites)
	{
		g->belongsTo = nullptr;
	}
	//�������ϵ
	satellites.clear();
	state = State::DESTROYED;
	visible = false;
	}
}

//����ϵ��������
void galaxy::applyAcceleration(b2Vec2 acceleration)
{
	auto& body = this->mainStella.body;
	body->ApplyForce(body->GetMass() * acceleration, body->GetPosition(), true);
	for (auto& s : satellites)
	{
		s->applyAcceleration(acceleration);
	}
}

void galaxy::applyForce(b2Vec2 Force)
{
	auto& body = this->mainStella.body;
	body->ApplyForce(Force, body->GetPosition(), true);
}

void galaxy::draw(SDL_Renderer *renderer, camera &camera)
{
	for (auto& s : satellites)
	{
		s->draw(renderer,camera);
	}
	if (!visible)
		return;
	auto& body = this->mainStella.body;
	// ���������������ת����SDL��Ⱦ������
	auto position = body->GetPosition() - camera.position;
	position = {
		position.x * SDL_cosf(camera.angle) - position.y * SDL_sinf(camera.angle), 
		position.x * SDL_sinf(camera.angle) + position.y * SDL_cosf(camera.angle) 
	};
	SDL_Point pos = { static_cast<int>(position.x * camera.scale * 50) + 960, static_cast<int>(position.y * camera.scale * 50) + 540 };
	auto& sprite = this->mainStella.sprite;
	// ������ͼ�ĽǶ�
	sprite.angle = body->GetAngle() * 180 / M_PI;

	sprite.draw(renderer, pos, camera.scale);
}

b2Vec2 galaxy::computeForce()
{
	b2Vec2 force = { 0.0f, 0.0f };
	for (auto& a : aroundGalaxies)
	{
		auto mass = a->mainStella.body->GetMass();
		auto vec = a->mainStella.body->GetPosition() - this->mainStella.body->GetPosition();
		auto distance = vec.Normalize();
		if (distance < b2_epsilon)
			continue;
		force += 10.0f * mainStella.body->GetMass() * mass / (distance * distance) * vec;
	}
	return force;
}

bool galaxy::addSubGalaxy(galaxy* subGalaxy)
{
	//1. ��ֹ��������ϵ
	//2. ��ֹ�ظ�����
	if (subGalaxy->belongsTo)
		return false;

	satellites.push_back(subGalaxy);
	subGalaxy->belongsTo = this;
	//�������Ǻ�����ϵ
	b2DistanceJointDef jd;
	jd.Initialize(this->mainStella.body, subGalaxy->mainStella.body,
		this->mainStella.body->GetPosition(), subGalaxy->mainStella.body->GetPosition());
	subGalaxy->OrbitalLinkage = this->mainStella.body->GetWorld()->CreateJoint(&jd);
	return true;
}

bool galaxy::removeSubGalaxy(galaxy* subGalaxy)
{
	if (!subGalaxy||!subGalaxy->OrbitalLinkage)
		return false;
	subGalaxy->mainStella.body->GetWorld()->DestroyJoint(subGalaxy->OrbitalLinkage);
	subGalaxy->OrbitalLinkage = nullptr;
	std::erase(subGalaxy->belongsTo->satellites, subGalaxy);
	subGalaxy->belongsTo = nullptr;
	return true;
}
