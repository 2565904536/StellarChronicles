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
		sprites –––«Ã˘Õº1(texManager.GetTex("01"), {1, 1}, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites –––«Ã˘Õº2(texManager.GetTex("02"), {1, 1}, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		sprites Ã´—ÙÃ˘Õº(texManager.GetTex("03"), {1, 1}, 1, 0.0f, 1.0f, SDL_FLIP_NONE);
		Ã´—Ù = new galaxy{{-3.0f, 0.0f}, 5.0f, 1.0f, –––«Ã˘Õº1};
		–––« = new galaxy{{0.0f, 0.0f}, 2.0f, 0.5f, –––«Ã˘Õº1};
		Œ¿–« = new galaxy{{5.0f, 0.0f}, 0.5f, 0.2f, –––«Ã˘Õº2};
		for (int i = 0; i < 500;i++)
		{
			galaxies.push_back(new galaxy{
				vec2{20.0f + 10.0f * random(gen), 20.0f + 10.0f * random(gen)},
				0.5f,
				0.2f + 0.5f * random(gen),
				–––«Ã˘Õº2});
		}
			// –––«->Velocity = { 0.0f,1.0f };
			–––«->linkSubGalaxy(Œ¿–«);
		Ã´—Ù->linkSubGalaxy(–––«);
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
					Ã´—Ù->removeSubGalaxy(–––«);
					// Œ¿–«->linkSubGalaxy();
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
		starTree.insert(*Ã´—Ù);
		starTree.insert(*–––«);
		starTree.insert(*Œ¿–«);
		for(auto&s:galaxies)
			starTree.insert(*s);
		auto aroundGalaxies = starTree.query({gameCamera.position.x, gameCamera.position.y, 15.0f, 15.0f});
		static float time = 1 / 60.0f;
		Ã´—Ù->applyAccleration(force);
		Ã´—Ù->contactProcess(starTree, time);
		–––«->contactProcess(starTree, time);
		Œ¿–«->contactProcess(starTree, time);
		for(auto&s:galaxies)
			s->contactProcess(starTree, time);
		Ã´—Ù->gravitationProcess(starTree);
		–––«->gravitationProcess(starTree);
		Œ¿–«->gravitationProcess(starTree);
		for(auto&s:galaxies)
			s->gravitationProcess(starTree);

		//Ã´—Ù->PhysicStep(time);
		//–––«->PhysicStep(time);
		//Œ¿–«->PhysicStep(time);
		//for(auto&s:galaxies)
		//	s->PhysicStep(time);
		// Ã´—Ù->update();
		// –––«->update();
		// Œ¿–«->update();

		switch (cameraindex)
		{
		case 0:
			gameCamera.position = vec2_zero;
			break;
		case 1:
			gameCamera.position = Ã´—Ù->getPosition();
			break;
		case 2:
			gameCamera.position = –––«->getPosition();
			break;
		case 3:
			gameCamera.position = Œ¿–«->getPosition();
			break;
		default:
			cameraindex = 0;
			break;
		}

		// «Âø’∆¡ƒª
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);

		Ã´—Ù->draw(renderer, gameCamera);

		–––«->draw(renderer, gameCamera);

		Œ¿–«->draw(renderer, gameCamera);
		for (auto& s : galaxies)
			s->draw(renderer, gameCamera);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderDrawLine(renderer, 0, 540, 1920, 540);
		SDL_RenderDrawLine(renderer, 960, 0, 960, 1080);
		// œ‘ æ‰÷»æƒ⁄»›
		SDL_RenderPresent(renderer);
		std::print("\rfps:{:.4f}", fps);
		// ∞¥–°º¸≈Ã0÷–∂œ≥Ã–Ú
		if (debugBreak)
			debugBreak = false;
	}

	manager texManager;
	camera gameCamera;
	galaxy *Ã´—Ù;
	galaxy *–––«;
	galaxy *Œ¿–«;
	std::vector<galaxy*> galaxies;

	Uint64 lastTime;

	bool loop = false;
	~gameInstance()
	{
		delete –––«;
		delete Ã´—Ù;
		delete Œ¿–«;
		for (auto& s : galaxies)
			delete s;
	}
};

int main(int argc, char *argv[])
{
	// ≥ı ºªØ ”∆µœµÕ≥∫Õ ±º‰œµÕ≥
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	// ≥ı ºªØÕºœÒº”‘ÿ∆˜£¨‘ –Ìº”‘ÿpngÕºœÒ
	IMG_Init(IMG_INIT_PNG);
	// ≥ı ºªØ◊÷ÃÂ
	TTF_Init();
	gameInstance stellarChronicles("Hello Stellar Chroicles", 100, 100, 1920, 1080, 0);

	std::println("hello world");
	stellarChronicles.INIT();

	do
	{
		stellarChronicles.LOOP();
	} while (stellarChronicles.loop);
	// ªÿ ’◊ ‘¥
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}