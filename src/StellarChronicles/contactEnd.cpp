#include "StellarChronicles/contactListener.h"

void gameContactListener::EndContact(b2Contact *contact)
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
		std::erase(A->aroundGalaxies, B);

	}

	if (fixtureB->IsSensor() && !fixtureA->IsSensor())
	{
		std::erase(B->aroundGalaxies, A);

	}
	if (!fixtureA->IsSensor()&&!fixtureB->IsSensor())
	{
		std::erase(A->collisionGalaxies, B);
		std::erase(B->collisionGalaxies, A);
	}
}