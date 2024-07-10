#include"StellarChronicles/contactListener.h"

void gameContactListener::EndContact(b2Contact* contact)
{
	b2Fixture* fixtureA = contact->GetFixtureA();
	b2Fixture* fixtureB = contact->GetFixtureB();
	auto data = fixtureA->GetBody()->GetUserData().pointer;
	if (!data)
		return;
	asteroid* A = reinterpret_cast<asteroid*>(data);
	data = fixtureB->GetBody()->GetUserData().pointer;
	if (!data)
		return;
	asteroid* B = reinterpret_cast<asteroid*>(data);

	if (fixtureA->IsSensor() && !fixtureB->IsSensor())
	{
		std::erase(A->aroundAsteroids, B);
		return;
	}

	if (fixtureB->IsSensor() && !fixtureA->IsSensor())
	{
		std::erase(B->aroundAsteroids, A);
		return;
	}
}