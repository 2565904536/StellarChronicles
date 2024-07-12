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
		texManager.LoadTex(renderer, "0001.png", "03");
		sprites 行星贴图1(texManager.GetTex("01"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites 行星贴图2(texManager.GetTex("02"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites 太阳贴图(texManager.GetTex("03"), { 1,1 }, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		行星 = new galaxy(&world, 行星贴图1, 0.5f, b2Vec2_zero, 1.0f);
		卫星 = new galaxy(&world, 行星贴图2, 0.1f, b2Vec2{1.0f,0.0f}, 0.3f);
		太阳 = new galaxy(&world, 太阳贴图, 1.0f, b2Vec2{ -3.0f,0.0f }, 5.0f);

		行星->linkSubGalaxy(卫星);
		太阳->linkSubGalaxy(行星);
		卫星->mainStella.body->SetLinearVelocity({ 0.0f,1.0f });
		行星->mainStella.body->SetLinearVelocity({ 0.0f,3.0f });
		gameCamera={ b2Vec2_zero,1.0f,0.0f };
		world.SetContactListener(&contact);
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
					行星->destroy();
					太阳->linkSubGalaxy(卫星);
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

		太阳->applyMainStellarAcceleration(force);
		太阳->applystallitesAcceleration(force);
		太阳->mainStella.body->ApplyTorque(Torque, true);
		太阳->satellites[0]->mainStella.body->ApplyTorque(0.5f * Torque, true);
		太阳->applyOrbitConstraints(60.0f);
		// 步进模拟物理计算，
		//  传入步长、速度约束求解器的迭代次数和位置约束求解器的迭代次数。
		world.Step(1 / 60.0f, 10, 8);
		switch (cameraindex)
		{
		case 0:
			gameCamera.position = b2Vec2_zero;
			break;
		case 1:
			gameCamera.position = 太阳->mainStella.body->GetPosition();
			break;
		case 2:
			gameCamera.position = 行星->mainStella.body->GetPosition();
			break;
		case 3:
			gameCamera.position = 卫星->mainStella.body->GetPosition();
			break;
		default:
			cameraindex = 0;
			break;
		}

		// 清空屏幕
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);
		if (!(太阳->belongsTo) && 太阳->visible)
			太阳->draw(renderer, gameCamera);
		if((!行星->belongsTo) && 行星->visible)
			行星->draw(renderer, gameCamera);
		if ((!卫星->belongsTo) && 卫星->visible)
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
	b2World world;
	manager texManager;
	camera gameCamera;
	gameContactListener contact;
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