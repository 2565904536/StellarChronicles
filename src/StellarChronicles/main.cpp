#include<print>
#include"SDL2/SDL.h"
#include"SDL2/SDL_image.h"
#include"SDL2/SDL_ttf.h"
#include"box2d/box2d.h"

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	IMG_Init(IMG_INIT_PNG);
	TTF_Init();

	SDL_Window *window = SDL_CreateWindow("Hello world", 100, 100, 1280, 720, false);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	std::println("hello world");

	b2Vec2 gravity(0.0f, -10.0f);
	b2World world(gravity);

	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -9.0f);
	b2Body* groundBody = world.CreateBody(&groundBodyDef);

	b2PolygonShape groundBox;
	groundBox.SetAsBox(48.0f, 9.0f);

	groundBody->CreateFixture(&groundBox, 0.0f);

	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 9.0f);
	b2Body* body = world.CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	fixtureDef.restitution = 0.9f;
	body->CreateFixture(&fixtureDef);

	const float timeStep = 1.0f / 165.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	const float pixsPerMeter = 1280 / 48.0f;

	SDL_Texture* block = IMG_LoadTexture(renderer, "box.png");
	if (!block)
		return -1;
	SDL_Event event;
	bool loop = true;
	do
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				loop = false;
				break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					loop = false;
					break;
				}
				break;
			}
		}

		world.Step(timeStep, velocityIterations, positionIterations);
		b2Vec2 position = body->GetPosition();
		float angle = body->GetAngle();
		SDL_RenderClear(renderer);

		SDL_FRect dstRect{ 640 - position.x * pixsPerMeter,360 - position.y * pixsPerMeter,2 * pixsPerMeter,2 * pixsPerMeter };
		SDL_FPoint center{ pixsPerMeter,pixsPerMeter };
		SDL_RenderCopyExF(renderer, block, nullptr, &dstRect, -angle, &center, SDL_FLIP_NONE);

		SDL_RenderPresent(renderer);
	} while (loop);

	SDL_DestroyTexture(block);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}