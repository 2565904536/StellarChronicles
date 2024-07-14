#include "StellarChronicles/stella.h"
galaxy::galaxy(vec2 position, float mass, float radius, sprites &sprite)
	: mainStar{mass, radius, sprite, this}, Position(position)
{
	mainStar.sprite.scale = radius * 100 / sprite.size.x;
}

// galaxy::~galaxy()
//{
//		destroy();
// }
//
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
// }
//
// void galaxy::applyMainStellarAcceleration(b2Vec2 acceleration)
//{
//	auto& body = this->mainStella.body;
//	body->ApplyForce(body->GetMass() * acceleration, body->GetPosition(), true);
//
// }
//
// void galaxy::applystallitesAcceleration(b2Vec2 acceleration)
//{
//	for (auto& s : satellites)
//	{
//		s->applyMainStellarAcceleration(acceleration);
//		s->applystallitesAcceleration(acceleration);
//	}
//
// }
//

//
// void galaxy::addSubGalaxy()
//{
//		auto& c = this->collisionGalaxies;
//	for (auto& g : aroundGalaxies)
//	{
//		if (std::find(c.begin(), c.end(), g) == c.end()&&[&]()-> bool {
//			auto v=this->mainStella.body->GetLinearVelocity()-g->mainStella.body->GetLinearVelocity();
//			if(v.LengthSquared()<100.0f)
//				return true;
//			return false;
//			}() && [&]()-> bool {
//				auto d=this->mainStella.body->GetPosition()-g->mainStella.body->GetPosition();
//				if (d.LengthSquared()<121.f)
//				return true;
//				return false;
//				}())
//		{
//			linkSubGalaxy(g);
//		}
//	}
//	return;
// }
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
void galaxy::applyOrbitConstraints(float time)
{
	if (!owner)
	{
		Position += Velocity * time;
		isUpdated = true;
		for (auto& s : satellites)
		{
			s->applyOrbitConstraints(time);
		}
		isUpdated = false;
	}
	else
	{
		if (!owner->isUpdated)
			return;
		orbitAng += orbitAngVel * time;
		Position = owner->Position + vec2{ SDL_cosf(orbitAng),SDL_sinf(orbitAng) }*orbitRadius;
		isUpdated = true;
		for (auto& s : satellites)
		{
			s->applyOrbitConstraints(time);
		}
		isUpdated = false;
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
	subGalaxy->orbitAngVel = vel.length() / subGalaxy->orbitRadius;
}

void galaxy::removeSubGalaxy(galaxy *subGalaxy)
{
	if (!subGalaxy || !subGalaxy->owner)
	{
		return;
	}
	subGalaxy->owner = nullptr;
	std::erase(satellites, subGalaxy);
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

stella::stella(float mass, float radius, sprites &sprite, galaxy *owner)
	: mass(mass), radius(radius), sprite(sprite), owner(owner)
{
}
