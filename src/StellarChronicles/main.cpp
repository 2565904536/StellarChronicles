#include <print>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "box2d/box2d.h"
#include "StellarChronicles/game.h"
#include "StellarChronicles/sprites.h"
#include "StellarChronicles/manager.h"
#include "StellarChronicles/contactListener.h"
#include "StellarChronicles/stella.h"

const b2Vec2 b2Vec2_zero{0.0f, 0.0f};
Uint64 frequency = SDL_GetPerformanceFrequency();
class gameInstance : public game
{
public:
	gameInstance(std::string_view name, int x, int y, int w, int h, Uint32 flags)
		: game(name, x, y, w, h, flags), world(b2Vec2_zero) {}
	void INIT()
	{
		lastTime = SDL_GetPerformanceCounter();

		texManager.LoadTex(renderer, "planet0.png", "01");
		texManager.LoadTex(renderer, "planet1.png", "02");
		sprites ������ͼ1(texManager.GetTex("01"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites ������ͼ2(texManager.GetTex("02"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		���� = new galaxy(&world, ������ͼ1, 1.0f, b2Vec2_zero, 1.0f);
		���� = new galaxy(&world, ������ͼ2, 1.0f, b2Vec2{3.0f,0.0f}, 1.0f);
		����->addSubGalaxy(����);
		gameCamera={ b2Vec2_zero,1.0f,0.0f };
		world.SetContactListener(&contact);

		//b2Vec2 v[4] = {
		//	{ 19.2f, -10.8f }, // ���Ͻ�  
		//	{ -19.2f,  -10.8f }, // ���Ͻ�  
		//	{-19.2f,  10.8f }, // ���½�  
		//	{ 19.2f, 10.8f }  // ���½�  
		//};
		//b2ChainShape chain;
		//chain.CreateLoop(v, 4);
		//b2BodyDef chainDef;
		//chainDef.type = b2_staticBody;
		//chainDef.position = b2Vec2_zero;
		//
		//Edge = world.CreateBody(&chainDef);
		//b2FixtureDef edgeDef;
		//edgeDef.shape = &chain;
		//edgeDef.restitution = 0.9;
		//Edge->CreateFixture(&edgeDef);
	}
	bool debugBreak = false;
	float theta = 0;
	void LOOP()
	{
		loop = true;
		float fps = frequency / static_cast<float>(SDL_GetPerformanceCounter() - lastTime);
		lastTime = SDL_GetPerformanceCounter();

		while (SDL_PollEvent(&windowEvent))
		{
			switch (windowEvent.type)
			{
			case SDL_QUIT:
				loop = false;
				break;
			case SDL_KEYDOWN:
				switch (windowEvent.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					loop = false;
					break;
				case SDLK_KP_0:
					debugBreak = true;
					break;
				}
				break;
			case SDL_MOUSEWHEEL:
				if (windowEvent.wheel.y > 0)
				{
					gameCamera.scale *= 1.1f;
				}
				else
				{
					gameCamera.scale /= 1.1f;
				}
				break;
			}
		}

		auto keyboardState = SDL_GetKeyboardState(NULL);
		b2Vec2 force = b2Vec2_zero;
		float Torque = 0.0f;
		if (keyboardState[SDL_SCANCODE_A])
			force += {-5.0f, 0.0f};
		if (keyboardState[SDL_SCANCODE_S])
			force += {0.0f, 5.0f};
		if (keyboardState[SDL_SCANCODE_D])
			force += {5.0f, 0.0f};
		if (keyboardState[SDL_SCANCODE_W])
			force += {0.0f, -5.0f};
		if (keyboardState[SDL_SCANCODE_Q])
			Torque += -1.0f;
		if (keyboardState[SDL_SCANCODE_E])
			Torque += 1.0f;

		auto vec = [](float angle) -> b2Vec2
		{
			return b2Vec2{ SDL_cosf(angle),SDL_sinf(angle) };
		};
		//theta += M_PI / 180.0;
		//asteroid2Body->SetTransform(asteroidBody->GetPosition() + 3.0f*vec(theta), 0);
		// 
		����->applyOverAllForce(force);
		//����->applyForce(����->computeForce());
		//����->applyForce(����->computeForce());
		// ����ģ��������㣬
		//  ���벽�����ٶ�Լ��������ĵ���������λ��Լ��������ĵ���������
		world.Step(1 / 60.0f, 10, 8);
		// �����Ļ
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);
		����->draw(renderer, gameCamera);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderDrawLine(renderer, 0, 540, 1920, 540);
		SDL_RenderDrawLine(renderer, 960, 0, 960, 1080);
		// ��ʾ��Ⱦ����
		SDL_RenderPresent(renderer);
		    std::print("\rfps:{:.4f}", fps);
		// ��С����0�жϳ���
		if (debugBreak)
			debugBreak = false;
	}
	b2World world;
	manager texManager;
	camera gameCamera;
	gameContactListener contact;
	galaxy *����;
	galaxy* ����;
	b2Body* Edge;

	Uint64 lastTime;

	bool loop = false;
	~gameInstance()
	{
		delete ����;
	}
};

int main(int argc, char *argv[])
{
	// ��ʼ����Ƶϵͳ��ʱ��ϵͳ
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	// ��ʼ��ͼ����������������pngͼ��
	IMG_Init(IMG_INIT_PNG);
	// ��ʼ������
	TTF_Init();
	gameInstance stellarChronicles("Hello Stellar Chroicles", 100, 100, 1920, 1080, 0);

	std::println("hello world");
	stellarChronicles.INIT();

	do
	{
		stellarChronicles.LOOP();
	} while (stellarChronicles.loop);
	// ������Դ
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}