#include "StellarChronicles/stella.h"

asteroid::asteroid(sprites* sprite,float radius, b2Vec2 position, float gravite)
	:sprite(sprite)
{	
	b2BodyDef asteroidDef;
	asteroidDef.type = b2_dynamicBody;

	asteroidDef.position = position;
	b2CircleShape Shape;
	Shape.m_radius = radius;
	b2FixtureDef bodFix;
	bodFix.shape = &Shape;
	bodFix.density = gravite / radius / radius;
	body->CreateFixture(&bodFix);
	Shape.m_radius = gravite;
	bodFix.shape = &Shape;
	bodFix.isSensor = true;
	body->CreateFixture(&bodFix);

}