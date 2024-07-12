#include "StellarChronicles/contactListener.h"

void gameContactListener::BeginContact(b2Contact *contact)
{
	b2Fixture *fixtureA = contact->GetFixtureA();
	b2Fixture *fixtureB = contact->GetFixtureB();
	auto data = fixtureA->GetBody()->GetUserData().pointer;
	if (!data)
		return;
	galaxy *A = reinterpret_cast<galaxy *>(data);
	data = fixtureB->GetBody()->GetUserData().pointer;
	if (!data)
		return;
	galaxy *B = reinterpret_cast<galaxy *>(data);

	if (fixtureA->IsSensor() && !fixtureB->IsSensor())
	{
		A->aroundGalaxies.push_back(B);
	}

	if (fixtureB->IsSensor() && !fixtureA->IsSensor())
	{
		B->aroundGalaxies.push_back(A);
	}
	if (!fixtureA->IsSensor() && !fixtureB->IsSensor())
	{
		A->collisionGalaxies.push_back(B);
		B->collisionGalaxies.push_back(A);
	}

}
