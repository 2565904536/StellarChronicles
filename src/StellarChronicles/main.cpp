#include <print>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "StellarChronicles/game.h"
#include "StellarChronicles/sprites.h"
#include "StellarChronicles/manager.h"
#include "StellarChronicles/stella.h"

const vec2 vec2_zero{0.0f, 0.0f};
Uint64 frequency = SDL_GetPerformanceFrequency();
class gameInstance : public game
{
public:
	gameInstance(std::string_view name, int x, int y, int w, int h, Uint32 flags)
		: game(name, x, y, w, h, flags) {}
	void INIT()
	{
		lastTime = SDL_GetPerformanceCounter();

		texManager.LoadTex(renderer, "planet0.png", "01");
		texManager.LoadTex(renderer, "planet1.png", "02");
		texManager.LoadTex(renderer, "0001.png", "03");
		sprites ������ͼ1(texManager.GetTex("01"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites ������ͼ2(texManager.GetTex("02"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites ̫����ͼ(texManager.GetTex("03"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		̫�� = new galaxy{ {-3.0f,0.0f},5.0f,1.0f,̫����ͼ };
		���� = new galaxy{ {0.0f,0.0f},2.0f,0.5f,������ͼ1 };
		���� = new galaxy{ {1.0f,0.0f},0.5f,0.2f,������ͼ2 };
		//����->Velocity = { 0.0f,1.0f };
		����->linkSubGalaxy(����);
		̫��->linkSubGalaxy(����);
		gameCamera={ vec2_zero,1.0f,0.0f };


	}
	bool debugBreak = false;
	int cameraindex = 0;
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
				case SDLK_b:
					//����->destroy();
					//̫��->linkSubGalaxy(����);
					break;
				case SDLK_LEFT:
					cameraindex--;
					break;
				case SDLK_RIGHT:
					cameraindex++;
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
		vec2 force = vec2_zero;
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

		//auto vec = [](float angle) -> b2Vec2
		//{
		//	return b2Vec2{ SDL_cosf(angle),SDL_sinf(angle) };
		//};

		//̫��->applyMainStellarAcceleration(force);
		//̫��->applystallitesAcceleration(force);
		//̫��->mainStella.body->ApplyTorque(Torque, true);
		//̫��->satellites[0]->mainStella.body->ApplyTorque(0.5f * Torque, true);
		//̫��->applyOrbitConstraints(60.0f);
		//// ����ģ���������㣬
		////  ���벽�����ٶ�Լ��������ĵ���������λ��Լ��������ĵ���������
		//world.Step(1 / 60.0f, 10, 8);

		static float time = 1 / 60.0f;
		̫��->applyOrbitConstraints(time);
		����->applyOrbitConstraints(time);
		����->applyOrbitConstraints(time);
		̫��->update();
		����->update();
		����->update();

		switch (cameraindex)
		{
		case 0:
			gameCamera.position = vec2_zero;
			break;
		case 1:
			gameCamera.position = ̫��->Position;
			break;
		case 2:
			gameCamera.position = ����->Position;
			break;
		case 3:
			gameCamera.position = ����->Position;
			break;
		default:
			cameraindex = 0;
			break;
		}

		// �����Ļ
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);

			̫��->draw(renderer, gameCamera);

			����->draw(renderer, gameCamera);

			����->draw(renderer, gameCamera);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderDrawLine(renderer, 0, 540, 1920, 540);
		SDL_RenderDrawLine(renderer, 960, 0, 960, 1080);
		// ��ʾ��Ⱦ����
		SDL_RenderPresent(renderer);
		auto d = ����->Position - ����->Position - vec2{ 1.0f,0.0f };
		std::print("\rfps:{:.4f} distance={:.4f}", fps, d.length());
		// ��С����0�жϳ���
		if (debugBreak)
			debugBreak = false;
	}

	manager texManager;
	camera gameCamera;
	galaxy* ̫��;
	galaxy *����;
	galaxy* ����;

	Uint64 lastTime;

	bool loop = false;
	~gameInstance()
	{	
		delete ����;
		delete ̫��;
		delete ����;
	}
};

int main(int argc, char *argv[])
{
	// ��ʼ����Ƶϵͳ��ʱ��ϵͳ
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	// ��ʼ��ͼ�����������������pngͼ��
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