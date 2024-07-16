#include "StellarChronicles/stella.h"
#include "StellarChronicles/quadTree.h"
#include <print>
galaxy::galaxy(vec2 position, float mass, float radius, sprites &sprite)
	: mainStar{mass, radius, sprite, this}, Position(position)
{
	mainStar.sprite.scale = radius * 100 / sprite.size.x;
}

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

void galaxy::leaveGalaxy()
{
	if (owner)
		owner->removeSubGalaxy(this);
	owner = nullptr;
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

void galaxy::applyImpulse(const vec2& imp)
{
	if (owner)
	{
		vec2 rv = { SDL_cosf(orbitAng),SDL_sinf(orbitAng) };
		auto impr = imp * rv;
		auto impt = rv.x * imp.y - rv.y * imp.x;
		orbitAngVel += impt / (orbitRadius * mainStar.mass);
	}
	else
	{
		Velocity += imp/mainStar.mass;
	}
}

void galaxy::applyDisplace(const vec2& disp)
{
	if (owner)
	{
		auto xt = SDL_cosf(orbitAng) * disp.y - SDL_sinf(orbitAng) * disp.x;
		orbitAng += xt / orbitRadius;
	}
	else
	{
		Position += disp;
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

void galaxy::contactProcess(QuadTree &tree)
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
		auto dv = s->getVelocity() - getVelocity();
		auto &m1 = mainStar.mass;
		auto &m2 = s->mainStar.mass;
		auto&& e = 0.5;
		auto imp = dv * (m1 * m2 / (m1 + m2) * (1 + e));
		applyImpulse(imp);
		applyDisplace(pos * (m2 *d/ (m1 + m2) / le));
		switch (type)
		{
		case galaxy::Type::asiderite:
			switch (s->type)
			{
			case galaxy::Type::asiderite:
				break;
			case galaxy::Type::planet:
				break;
			case galaxy::Type::star:
				break;
			default:
				break;
			}
			break;
		case galaxy::Type::planet:
			switch (s->type)
			{
			case galaxy::Type::asiderite:
				break;
			case galaxy::Type::planet:
				break;
			case galaxy::Type::star:
				break;
			default:
				break;
			}
			break;
		case galaxy::Type::star:
			switch (s->type)
			{
			case galaxy::Type::asiderite:
				break;
			case galaxy::Type::planet:
				break;
			case galaxy::Type::star:
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}
}

void galaxy::gravitationProcess(QuadTree& tree)
{	
	auto  arroundGalaxies = tree.query({ Position.x,Position.y,10 * mainStar.radius, 10 * mainStar.radius });
	for(auto&s:arroundGalaxies)
	{
		if (s == this||s->type==Type::asiderite||isGalaxyInSatellites(s))
			continue;
		auto pos = s->Position - Position;
		auto d = pos.lengthSqure();
		applyAccleration(pos * (0.1*s->mainStar.mass / d ));
	}
}

bool galaxy::isGalaxyInSatellites(galaxy* subgalaxy)
{
	// 首先检查当前galaxy的直接卫星  
	if (std::find(satellites.begin(), satellites.end(), subgalaxy) != satellites.end()) {
		return true; // 如果找到了，直接返回true  
	}

	// 如果当前galaxy没有直接卫星，或者subgalaxy不是直接卫星之一  
	// 则递归检查所有直接卫星  
	for (auto& s : satellites) {
		if (s->isGalaxyInSatellites(subgalaxy)) {
			return true; // 如果任一直接卫星包含subgalaxy，则返回true  
		}
	}

	// 如果没有任何直接卫星包含subgalaxy，则返回false  
	return false;
}

vec2 galaxy::getPosition() const
{
	if (owner)
		return owner->getPosition() + vec2{ SDL_cosf(orbitAng),SDL_sinf(orbitAng) }*(orbitRadius);
	return Position;
}

vec2 galaxy::getVelocity() const
{
	if (owner)
		return owner->getVelocity() + vec2{-SDL_sinf(orbitAng),SDL_cosf(orbitAng) }*(orbitRadius * orbitAngVel);
	return Velocity;
}

void galaxy::telePosition(const vec2& location)
{
	leaveGalaxy();
	Position = location;
}

stella::stella(float mass, float radius, sprites &sprite, galaxy *owner)
	: mass(mass), radius(radius), sprite(sprite), owner(owner)
{
}
