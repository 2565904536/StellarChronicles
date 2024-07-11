#pragma once
#include "box2d/box2d.h"
#include "StellarChronicles/stella.h"

class gameContactListener:public b2ContactListener
{
public:
	gameContactListener() = default;
	~gameContactListener()=default;
	void BeginContact(b2Contact* contact) override;
	void EndContact(b2Contact* contact) override;

private:

};