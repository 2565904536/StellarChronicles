#include <print>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "StellarChronicles/game.h"
#include "StellarChronicles/sprites.h"
#include "StellarChronicles/manager.h"
#include "StellarChronicles/stella.h"
#include "StellarChronicles/quadTree.h"


	bool QuadTree::isEntityInRange(const galaxy& galaxy, const Rect& rect)
	{
		auto& x = galaxy.Position.x;
		auto& y = galaxy.Position.y;
		auto& r = galaxy.mainStar.radius;
		float minX = x - r;
		float minY = y - r;
		float maxX = x + r;
		float maxY = y + r;

		return !(minX > rect.centerX + rect.halfW || maxX < rect.centerX - rect.halfW &&
			minY > rect.centerY + rect.halfH || maxY < rect.centerY - rect.halfH);
	}

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
		sprites 行星贴图1(texManager.GetTex("01"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites 行星贴图2(texManager.GetTex("02"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites 太阳贴图(texManager.GetTex("03"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		太阳 = new galaxy{ {-3.0f,0.0f},5.0f,1.0f,行星贴图1 };
		行星 = new galaxy{ {0.0f,0.0f},2.0f,0.5f,行星贴图1 };
		卫星 = new galaxy{ {5.0f,0.0f},0.5f,0.2f,行星贴图2 };
		//行星->Velocity = { 0.0f,1.0f };
		行星->linkSubGalaxy(卫星);
		太阳->linkSubGalaxy(行星);
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
					太阳->removeSubGalaxy(行星);
					//卫星->linkSubGalaxy();
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


		QuadTree starTree{{0.0f,0.0f,10000.0f,10000.0f}};
		starTree.insert(*太阳);
		starTree.insert(*行星);
		starTree.insert(*卫星);
		auto aroundGalaxies = starTree.query({ gameCamera.position.x,gameCamera.position.y,15.0f,15.0f });
		static float time = 1 / 60.0f;
		太阳->applyAccleration(force);
		太阳->contactProcess(starTree, time);
		行星->contactProcess(starTree, time);
		卫星->contactProcess(starTree, time);
		太阳->gravitationProcess(starTree);
		行星->gravitationProcess(starTree);
		卫星->gravitationProcess(starTree);

		太阳->PhysicStep(time);
		行星->PhysicStep(time);
		卫星->PhysicStep(time);
		//太阳->update();
		//行星->update();
		//卫星->update();

		switch (cameraindex)
		{
		case 0:
			gameCamera.position = vec2_zero;
			break;
		case 1:
			gameCamera.position = 太阳->getPosition();
			break;
		case 2:
			gameCamera.position = 行星->getPosition();
			break;
		case 3:
			gameCamera.position = 卫星->getPosition();
			break;
		default:
			cameraindex = 0;
			break;
		}

		// 清空屏幕
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);

			太阳->draw(renderer, gameCamera);

			行星->draw(renderer, gameCamera);

			卫星->draw(renderer, gameCamera);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderDrawLine(renderer, 0, 540, 1920, 540);
		SDL_RenderDrawLine(renderer, 960, 0, 960, 1080);
		// 显示渲染内容
		SDL_RenderPresent(renderer);
		std::print("\rfps:{:.4f}", fps);
		// 按小键盘0中断程序
		if (debugBreak)
			debugBreak = false;
	}

	manager texManager;
	camera gameCamera;
	galaxy* 太阳;
	galaxy *行星;
	galaxy* 卫星;

	Uint64 lastTime;

	bool loop = false;
	~gameInstance()
	{	
		delete 行星;
		delete 太阳;
		delete 卫星;
	}
};

int main(int argc, char *argv[])
{
	// 初始化视频系统和时间系统
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	// 初始化图像加载器，允许加载png图像
	IMG_Init(IMG_INIT_PNG);
	// 初始化字体
	TTF_Init();
	gameInstance stellarChronicles("Hello Stellar Chroicles", 100, 100, 1920, 1080, 0);

	std::println("hello world");
	stellarChronicles.INIT();

	do
	{
		stellarChronicles.LOOP();
	} while (stellarChronicles.loop);
	// 回收资源
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}