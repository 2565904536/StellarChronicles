#include <print>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_mixer.h"
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
Uint64 gameTime = 0;
class gameInstance : public game
{
public:
	gameInstance(std::string_view name, int x, int y, int w, int h, Uint32 flags)
		: game(name, x, y, w, h, flags) {}
	void INIT()
	{
		lastTime = SDL_GetPerformanceCounter();
		texManager.LoadTex(renderer, "texture.png", "00");
		texManager.LoadTex(renderer, "background.png", "01");
		texManager.loadWAV("absorb.wav", "absorb");
		texManager.loadWAV("contact.wav", "contact");
		texManager.loadWAV("destroy.wav", "destroy");
		texManager.loadWAV("link.wav", "link");
		font = TTF_OpenFont("consola.ttf", 72);

		sprites Texture{ texManager.GetTex("00"),{3,1},3,0.0f,1.0f,SDL_FLIP_NONE };
		player = new galaxy{ vec2_zero,1.0f,1.0f,Texture };
		player->isPlayer = true;
		galaxies.push_back(player);
		for (int i = 0; i < 1000; i++)
		{
			galaxies.push_back(new galaxy{
				vec2{-100.0f+ 200.0f * random(gen), -100.0f+ 200.0f * random(gen)},
				1.0f,
				0.3f + 0.2f * random(gen),
				Texture });
		}

		// 行星->Velocity = { 0.0f,1.0f };
		// 行星->linkSubGalaxy(卫星);

		gameCamera = {vec2_zero, 1.0f, 0.0f};
		gameCamera.scale = 1.0f;
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
					if (gameState == gameState::game)
						gameState = gameState::Pause;
					else {
						gameState = gameState::game;
						if (player->state == galaxy::State::Destroyed)
							loop = false;
					}
					break;
				case SDLK_KP_0:
					debugBreak = true;
					break;
				case SDLK_b:

					// 卫星->linkSubGalaxy();
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
		bool absorb =false;
		if (keyboardState[SDL_SCANCODE_A])
		{
			force += {-5.0f, 0.0f};
		}
		if (keyboardState[SDL_SCANCODE_S])
			force += {0.0f, 5.0f};
		if (keyboardState[SDL_SCANCODE_D])
			force += {5.0f, 0.0f};
		if (keyboardState[SDL_SCANCODE_W])
			force += {0.0f, -5.0f};
		if (keyboardState[SDL_SCANCODE_SPACE])
			absorb = true;


		QuadTree starTree{{gameCamera.position.x, gameCamera.position.y, 1000.0f, 1000.0f}};
		for (auto &s : galaxies)
			if (s->state == galaxy::State::Alive)
				starTree.insert(*s);
		auto aroundGalaxies = starTree.query({ gameCamera.position.x, gameCamera.position.y, 32.0f / gameCamera.scale, 18.0f / gameCamera.scale });
		float time = 1/fps;
		if(gameState==gameState::game)
		{

			player->applyAccleration(force);
			if (absorb)
			{
				player->absorb();
				Mix_PlayChannel(-1, texManager.getMusic("absorb"), 0);
			}

			for (auto& s : galaxies)
			{
				auto stars = starTree.query({ s->getPosition().x,s->getPosition().y,10 * s->mainStar.radius,10 * s->mainStar.radius });
				s->contactProcess(stars);
				s->linkProcess(stars);
				s->gravitationProcess(stars);
				if (s->iscontact)
				{
					Mix_PlayChannel(-1, texManager.getMusic("contact"), 0);
					s->iscontact = false;
				}

				if (s->islink)
					Mix_PlayChannel(-1, texManager.getMusic("link"), 0);
			}

			for (auto& s : galaxies)
				std::swap(s->life, s->futlife);

			for (auto& s : aroundGalaxies)
			{
				switch (s->type)
				{
				case galaxy::Type::asiderite:
					if (s->life > 20)
					{
						s->upgrade();
						if (s->isPlayer)
							score += 1000;
					}
					break;
				case galaxy::Type::planet:
					if (s->life > 250)
					{
						s->upgrade();
						if (s->isPlayer)
							score += 5000;
					}
					break;
				case galaxy::Type::star:
					break;
				default:
					break;
				}
			}

			for (auto& s : galaxies)
				s->PhysicStep(time);
				if (player->getVelocity().lengthSqure() > 10.0f&&pool.pool.size()>30)
				{
					auto newGalaxy = pool.getObj();
					newGalaxy->type = galaxy::Type::asiderite;
					newGalaxy->state = galaxy::State::Alive;
					newGalaxy->life = 2;
					newGalaxy->tp(vec2{ -100.0f + 200.0f * random(gen), -100.0f + 200.0f * random(gen) } + player->getPosition());
				}


			gameCamera.position = player->getPosition();
			for (auto& s : aroundGalaxies)
				if (s->state == galaxy::State::Dead)
				{
					s->destroy();
					Mix_PlayChannel(-1, texManager.getMusic("destroy"), 0);
					pool += s;
				}
			//gameTime += time;
			score +=time * 10;
			
		}
		SDL_Surface* scorePrint = TTF_RenderText_Blended(font, std::format("score: {:.0f}", score).c_str(), { 0xFF,0x00,0xFF,0xFF });
		SDL_Texture* scoreTexture = SDL_CreateTextureFromSurface(renderer, scorePrint);
		SDL_FreeSurface(scorePrint);
		// 清空屏幕
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderCopy(renderer, texManager.GetTex("01"), nullptr, nullptr);

		for (auto &s : aroundGalaxies)
			if (s->state == galaxy::State::Alive) [[likely]]
			{
				s->draw(renderer, gameCamera);
			}
		SDL_Rect scoreDest{ 0,0,256,64 };
		SDL_RenderCopy(renderer, scoreTexture, nullptr, &scoreDest);
		//  显示渲染内容
		SDL_RenderPresent(renderer);
		std::print("\rfps:{:.4f}", fps);
		SDL_DestroyTexture(scoreTexture);
		// 按小键盘0中断程序
		if (debugBreak)
			debugBreak = false;
	}

	manager texManager;
	TTF_Font* font;
	camera gameCamera;
	std::vector<galaxy *> galaxies;
	galaxy* player;
	objectPool pool;
	Uint64 lastTime;
	float score = 0.0;
	enum class gameState
	{
		Pause,
		game
	} gameState = gameState::Pause;

	bool loop = false;
	~gameInstance()
	{
		for (auto &s : galaxies)
			delete s;
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
	Mix_Init(MIX_INIT_MP3);
	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
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