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
	if (state != State::Alive&&!isPlayer)
		return;
	if (!owner)
	{
		Velocity += Accleration * time;
		Position += (Velocity)*time;
		isUpdated = true;
		for (auto &s : satellites)
		{
			s->PhysicStep(time);
		}
		isUpdated = false;
		Accleration = {0.0f, 0.0f};
	}
	else
	{
		if (!owner->isUpdated)
			return;
		orbitAngVel += orbitAngAcc * time;
		orbitAng += (orbitAngVel)*time;
		Position = owner->Position + vec2{SDL_cosf(orbitAng), SDL_sinf(orbitAng)} * orbitRadius;
		isUpdated = true;
		for (auto &s : satellites)
		{
			s->PhysicStep(time);
		}
		isUpdated = false;
		orbitAngAcc = 0.0f;
	}
	iscontact = false;
	islink = false;
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
	if (a)
		subGalaxy->orbitAngVel = vel.length() / subGalaxy->orbitRadius;
	else
		subGalaxy->orbitAngVel = -vel.length() / subGalaxy->orbitRadius;
	islink = true;
}

void galaxy::removeSubGalaxy(galaxy *subGalaxy)
{
	if (!subGalaxy || subGalaxy->owner != this)
	{
		return;
	}
	subGalaxy->owner = nullptr;
	std::erase(satellites, subGalaxy);
	subGalaxy->Velocity = vec2{-SDL_sinf(subGalaxy->orbitAng), SDL_cosf(subGalaxy->orbitAng)} * (subGalaxy->orbitRadius * subGalaxy->orbitAngVel);
}

void galaxy::leaveGalaxy()
{
	if (owner)
		owner->removeSubGalaxy(this);
	owner = nullptr;
}

void galaxy::applyAccleration(const vec2 &acc)
{
	if (owner)
	{
		vec2 rv = {SDL_cosf(orbitAng), SDL_sinf(orbitAng)};
		auto accr = acc * rv;
		auto acct = rv.x * acc.y - rv.y * acc.x;

		orbitAngAcc += acct / orbitRadius;
	}
	else
	{
		Accleration += acc;
	}
}

void galaxy::applyImpulse(const vec2 &imp)
{
	if (owner)
	{
		vec2 rv = {SDL_cosf(orbitAng), SDL_sinf(orbitAng)};
		auto impr = imp * rv;
		auto impt = rv.x * imp.y - rv.y * imp.x;
		orbitAngVel += impt / (orbitRadius * mainStar.mass);
	}
	else
	{
		Velocity += imp / mainStar.mass;
	}
}

void galaxy::applyDisplace(const vec2 &disp)
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

void galaxy::contactProcess(std::vector<galaxy *> &aroundGalaxies)
{
	if (state != State::Alive)
		return;
	// 得到周围的galaxies

	for (auto &s : aroundGalaxies)
	{
		// 不和自己碰撞
		if (s == this || s->state != State::Alive)
			continue;
		// 仔细判断碰撞
		auto pos = s->Position - Position;
		auto le = pos.length();
		if (le > 2 * mainStar.radius + s->mainStar.radius)
			continue;
		auto d = le - (s->mainStar.radius + mainStar.radius);
		if (d > 0)
			continue;
		// 处理碰撞后果
		auto dv = s->getVelocity() - getVelocity();
		auto dvlen = dv.length();
		auto &m1 = mainStar.mass;
		auto &m2 = s->mainStar.mass;
		auto &&e = 1.0f;
		auto imp = dv * (m1 * m2 / (m1 + m2) * (1 + e));
		applyDisplace(pos * (m2 * d *1.01f/ (m1 + m2) / le));
		applyImpulse(imp);
		futlife = life;
		switch (type)
		{
		case galaxy::Type::asiderite:
			switch (s->type)
			{
			case galaxy::Type::asiderite:
				if (true)
				{
					if (isPlayer)
					{
						futlife += s->life;
						break;
					}
					if (s->isPlayer)
					{
						futlife = 0;
						break;
					}
					if (life >= s->life)
					{
						futlife += s->life;
						s->state = State::Dead;
					}
				}
				break;
			case galaxy::Type::planet:
				futlife = 0;

				break;
			case galaxy::Type::star:
				futlife = 0;
				break;
			default:
				break;
			}
			if (futlife <= 0)
				state = State::Dead;
			break;
		case galaxy::Type::planet:
			switch (s->type)
			{
			case galaxy::Type::asiderite:
				futlife -= s->life;

				break;
			case galaxy::Type::planet:
				futlife -= 5;
				break;
			case galaxy::Type::star:
				futlife = 0;
				break;
			default:
				break;
			}
			if (futlife <= 70)
				state = State::Dead;
			break;
		case galaxy::Type::star:
			switch (s->type)
			{
			case galaxy::Type::asiderite:
				break;
			case galaxy::Type::planet:
				futlife -= s->life;
				break;
			case galaxy::Type::star:
				futlife -= 100;
				break;
			default:
				break;
			}
			if (futlife <= 700)
				state = State::Dead;
			break;
		default:
			break;
		}
	iscontact = true;
	}

}

void galaxy::gravitationProcess(std::vector<galaxy *> &aroundGalaxies)
{
	if (state != State::Alive)
		return;

	for (auto &s : aroundGalaxies)
	{
		auto pos = s->Position - Position;
		auto d = pos.lengthSqure();
		if (d > 100 * mainStar.radius * mainStar.radius)
			continue;
		if (s == this ||   s->type == Type::asiderite|| s->state != State::Alive || isGalaxyInSatellites(s))
			continue;

		applyAccleration(pos * (0.1 * s->mainStar.mass / d));
	}
}

void galaxy::linkProcess(std::vector<galaxy *> &aroundGalaxies)
{
	if (state != State::Alive)
		return;
	for (auto &s : aroundGalaxies)
	{
		if (s == this || s->state != State::Alive)
			continue;
		auto pos = s->Position - Position;
		auto d = pos.lengthSqure();
		if (d > 100 * mainStar.radius * mainStar.radius||satellites.size()>10)
			continue;
		if (s->isPlayer)
			continue;
		switch (this->type)
		{
		case Type::asiderite:

			break;
		case Type::planet:
			if (s->type == Type::asiderite)
			{
				auto vel = s->getVelocity() - getVelocity();
				auto vellen = vel.length();
				if (d > 4.0f * mainStar.radius * mainStar.radius && vellen < 4.0f)
					linkSubGalaxy(s);
			}
			break;
		case Type::star:
			if (s->type == Type::planet)
			{
				auto vel = s->getVelocity() - getVelocity();
				auto vellen = vel.length();
				if (d > 4.0f * mainStar.radius * mainStar.radius && vellen < 4.0f)
					linkSubGalaxy(s);
			}
			break;
		default:
			break;
		}
	}
}

bool galaxy::isGalaxyInSatellites(galaxy *subgalaxy)
{
	if (!subgalaxy)
		return false;
	// 检查subgalaxy是否是当前星系的直接或间接子星系
	galaxy *currentOwner = subgalaxy->owner;
	while (currentOwner != nullptr)
	{
		if (currentOwner == this)
		{
			return true; // 找到了，subgalaxy是当前星系的子星系
		}
		currentOwner = currentOwner->owner; // 继续向上查找父星系
	}

	// 没有找到，subgalaxy不是当前星系的子星系
	return false;
}

vec2 galaxy::getPosition() const
{
	if (owner)
		return owner->getPosition() + vec2{SDL_cosf(orbitAng), SDL_sinf(orbitAng)} * (orbitRadius);
	return Position;
}

vec2 galaxy::getVelocity() const
{
	if (owner)
		return owner->getVelocity() + vec2{-SDL_sinf(orbitAng), SDL_cosf(orbitAng)} * (orbitRadius * orbitAngVel);
	return Velocity;
}

void galaxy::upgrade()
{
	switch (type)
	{
	case galaxy::Type::asiderite:
		type = Type::planet;
		life = 100;
		mainStar.mass = 10.0f;
		mainStar.radius = 4.0f;
		mainStar.sprite.scale = mainStar.radius * 100 / mainStar.sprite.size.x;
		mainStar.sprite.nextFrame();
		break;
	case galaxy::Type::planet:
		type = Type::star;
		life = 1000;
		mainStar.mass = 100.0f;
		mainStar.radius = 15.0f;
		mainStar.sprite.scale = mainStar.radius * 100 / mainStar.sprite.size.x;
		mainStar.sprite.nextFrame();
		break;
	case galaxy::Type::star:
		break;
	default:
		break;
	}
}

void galaxy::absorb()
{
	if (satellites.empty())
		return;
	auto s = satellites.front();
	futlife += s->life;
	s->futlife = 0;
	s->state = State::Dead;
}

galaxy *galaxy::destroy()
{
	leaveGalaxy();
	for (auto &s : satellites)
		s->owner = nullptr;
	satellites.clear();
	state = State::Destroyed;
	Position = vec2{0.0f, 0.0f};
	Velocity = vec2{0.0f, 0.0f};
	life = 0;
	iscontact = false;
	islink = false;
	return this;
}

void galaxy::tp(vec2 position)
{
	Position = position;
}

stella::stella(float mass, float radius, sprites &sprite, galaxy *owner)
	: mass(mass), radius(radius), sprite(sprite), owner(owner)
{
}
