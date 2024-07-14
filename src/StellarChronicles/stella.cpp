#include "StellarChronicles/stella.h"
#include "StellarChronicles/quadTree.h"
#include <print>
galaxy::galaxy(vec2 position, float mass, float radius, sprites &sprite)
	: mainStar{mass, radius, sprite, this}, Position(position)
{
	mainStar.sprite.scale = radius * 100 / sprite.size.x;
}


// void galaxy::destroy()
//{
//	if (state == State::ALIVE)
//	{
//	//body销毁后，触发endcontact,其他galaxy移除aroundGalaxies中的此galaxy
//	//所有连接body的关节也会被销毁
//	this->mainStella.body->GetWorld()->DestroyBody(this->mainStella.body);
//	if (this->belongsTo)
//	{
//		std::erase(this->belongsTo->satellites, this);
//		this->belongsTo = nullptr;
//	}
//	//解放卫星系，让他们独立
//	for (auto& g : satellites)
//	{
//		g->belongsTo = nullptr;
//	}
//	//清空卫星系
//	satellites.clear();
//	state = State::DESTROYED;
//	visible = false;
//	}
// 
//
// b2Vec2 galaxy::calculateGravitationalAcceleration(float gravitationalConstant)
//{
//	b2Vec2 acc = { 0.0f, 0.0f };
//	for (auto& a : aroundGalaxies)
//	{
//		auto mass = a->mainStella.body->GetMass();
//		auto vec = a->mainStella.body->GetPosition() - this->mainStella.body->GetPosition();
//		auto distance = vec.Normalize();
//		if (distance < b2_epsilon)
//			continue;
//		acc += gravitationalConstant * mass / (distance * distance) * vec;
//	}
//	return acc;
// }
//
void galaxy::PhysicStep(float time)
{
	if (!owner)
	{
		Velocity += Accleration * time;
		Position += (Velocity) * time;
		isUpdated = true;
		for (auto& s : satellites)
		{
			s->PhysicStep(time);
		}
		isUpdated = false;
		Accleration = { 0.0f,0.0f };
	}
	else
	{
		if (!owner->isUpdated)
			return;
		orbitAngVel += orbitAngAcc * time;
		orbitAng += (orbitAngVel)*time;
		Position = owner->Position + vec2{ SDL_cosf(orbitAng),SDL_sinf(orbitAng) }*orbitRadius;
		isUpdated = true;
		for (auto& s : satellites)
		{
			s->PhysicStep(time);
		}
		isUpdated = false;
		orbitAngAcc = 0.0f;
	}
}

void galaxy::linkSubGalaxy(galaxy *subGalaxy)
{
	if (!subGalaxy || subGalaxy->owner)
		return;
	satellites.push_back(subGalaxy);
	subGalaxy->owner = this;
	auto pos = subGalaxy->Position - Position;
	auto vel = subGalaxy->Velocity - Velocity;
	subGalaxy->orbitRadius = pos.length();
	subGalaxy->orbitAng = pos.angle();
	bool a = (pos.x * vel.y - pos.y * vel.x) > 0;
	if(a)
		subGalaxy->orbitAngVel = vel.length() / subGalaxy->orbitRadius;
	else
		subGalaxy->orbitAngVel = -vel.length() / subGalaxy->orbitRadius;
}

void galaxy::removeSubGalaxy(galaxy *subGalaxy)
{
	if (!subGalaxy || subGalaxy->owner!=this)
	{
		return;
	}
	subGalaxy->owner = nullptr;
	std::erase(satellites, subGalaxy);
	subGalaxy->Velocity = vec2{ -SDL_sinf(subGalaxy->orbitAng),SDL_cosf(subGalaxy->orbitAng) }*(subGalaxy->orbitRadius * subGalaxy->orbitAngVel);
}

void galaxy::applyAccleration(const vec2& acc)
{
	if (owner)
	{
		vec2 rv = { SDL_cosf(orbitAng),SDL_sinf(orbitAng) };
		auto accr = acc * rv;
		auto acct = rv.x * acc.y - rv.y * acc.x;

		orbitAngAcc += acct / orbitRadius;
	}
	else
	{
		Accleration += acc;
	}

}

void galaxy::update()
{
}

void galaxy::draw(SDL_Renderer *renderer, camera &camera)
{
	auto pos = Position - camera.position;
	pos = {
		pos.x * SDL_cosf(camera.angle) - pos.y * SDL_sinf(camera.angle),
		pos.x * SDL_sinf(camera.angle) + pos.y * SDL_cosf(camera.angle)};
	SDL_Point pos2 = {static_cast<int>(pos.x * camera.scale * 50) + 960, static_cast<int>(pos.y * camera.scale * 50) + 540};
	auto &sprite = this->mainStar.sprite;
	sprite.angle = mainStar.spinAng * 180 / M_PI;
	sprite.draw(renderer, pos2, camera.scale);
}

void galaxy::contactProcess(QuadTree &tree, float time)
{
	//得到周围的galaxies
	auto arroundGalaxies = tree.query({ Position.x,Position.y,2*mainStar.radius, 2*mainStar.radius });
	for (auto& s : arroundGalaxies)
	{
		//不和自己碰撞
		if (s == this)
			continue;
		//仔细判断碰撞
		auto pos = s->Position - Position;
		auto le = pos.length();
		auto d = le - (s->mainStar.radius + mainStar.radius);
		if (d>0)
			continue;
		//处理碰撞后果
		auto ang = pos.angle();
		auto acc=vec2{SDL_cosf(ang),SDL_sinf(ang)};
		applyAccleration(acc*d/time);
	}
}

vec2 galaxy::getPosition() const
{
	return Position;
}

stella::stella(float mass, float radius, sprites &sprite, galaxy *owner)
	: mass(mass), radius(radius), sprite(sprite), owner(owner)
{
}
