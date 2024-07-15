#include <print>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "StellarChronicles/game.h"
#include "StellarChronicles/sprites.h"
#include "StellarChronicles/manager.h"
#include "StellarChronicles/stella.h"
#include "StellarChronicles/quadTree.h"
#include <random>

bool QuadTree::isEntityInRange(const galaxy &galaxy, const Rect &rect)
{
	auto &x = galaxy.Position.x;
	auto &y = galaxy.Position.y;
	auto &r = galaxy.mainStar.radius;
	float minX = x - r;
	float minY = y - r;
	float maxX = x + r;
	float maxY = y + r;

	return !(minX > rect.centerX + rect.halfW || maxX < rect.centerX - rect.halfW || minY > rect.centerY + rect.halfH || maxY < rect.centerY - rect.halfH);
}

const vec2 vec2_zero{0.0f, 0.0f};
Uint64 frequency = SDL_GetPerformanceFrequency();
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<float> random(0.0f, 1.0f);
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
		sprites ������ͼ1(texManager.GetTex("01"), {1, 1}, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites ������ͼ2(texManager.GetTex("02"), {1, 1}, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites ̫����ͼ(texManager.GetTex("03"), {1, 1}, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		̫�� = new galaxy{{-3.0f, 0.0f}, 5.0f, 1.0f, ������ͼ1};
		���� = new galaxy{{0.0f, 0.0f}, 2.0f, 0.5f, ������ͼ1};
		���� = new galaxy{{5.0f, 0.0f}, 0.5f, 0.2f, ������ͼ2};
		for (int i = 0; i < 500;i++)
		{
			galaxies.push_back(new galaxy{
				vec2{20.0f + 10.0f * random(gen), 20.0f + 10.0f * random(gen)},
				0.5f,
				0.2f + 0.5f * random(gen),
				������ͼ2});
		}
			// ����->Velocity = { 0.0f,1.0f };
			����->linkSubGalaxy(����);
		̫��->linkSubGalaxy(����);
		gameCamera = {vec2_zero, 1.0f, 0.0f};
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
					̫��->removeSubGalaxy(����);
					// ����->linkSubGalaxy();
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
		{
			force += {-1.0f, 0.0f};
		}
		if (keyboardState[SDL_SCANCODE_S])
			force += {0.0f, 1.0f};
		if (keyboardState[SDL_SCANCODE_D])
			force += {1.0f, 0.0f};
		if (keyboardState[SDL_SCANCODE_W])
			force += {0.0f, -1.0f};
		if (keyboardState[SDL_SCANCODE_Q])
			Torque += -1.0f;
		if (keyboardState[SDL_SCANCODE_E])
			Torque += 1.0f;

		QuadTree starTree{{50.0f, 50.0f,50.0f, 50.0f}};
		starTree.insert(*̫��);
		starTree.insert(*����);
		starTree.insert(*����);
		for(auto&s:galaxies)
			starTree.insert(*s);
		auto aroundGalaxies = starTree.query({gameCamera.position.x, gameCamera.position.y, 15.0f, 15.0f});
		static float time = 1 / 60.0f;
		̫��->applyAccleration(force);
		̫��->contactProcess(starTree, time);
		����->contactProcess(starTree, time);
		����->contactProcess(starTree, time);
		for(auto&s:galaxies)
			s->contactProcess(starTree, time);
		̫��->gravitationProcess(starTree);
		����->gravitationProcess(starTree);
		����->gravitationProcess(starTree);
		for(auto&s:galaxies)
			s->gravitationProcess(starTree);

		//̫��->PhysicStep(time);
		//����->PhysicStep(time);
		//����->PhysicStep(time);
		//for(auto&s:galaxies)
		//	s->PhysicStep(time);
		// ̫��->update();
		// ����->update();
		// ����->update();

		switch (cameraindex)
		{
		case 0:
			gameCamera.position = vec2_zero;
			break;
		case 1:
			gameCamera.position = ̫��->getPosition();
			break;
		case 2:
			gameCamera.position = ����->getPosition();
			break;
		case 3:
			gameCamera.position = ����->getPosition();
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
		for (auto& s : galaxies)
			s->draw(renderer, gameCamera);
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

	manager texManager;
	camera gameCamera;
	galaxy *̫��;
	galaxy *����;
	galaxy *����;
	std::vector<galaxy*> galaxies;

	Uint64 lastTime;

	bool loop = false;
	~gameInstance()
	{
		delete ����;
		delete ̫��;
		delete ����;
		for (auto& s : galaxies)
			delete s;
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