#include <print>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "box2d/box2d.h"
#include "StellarChronicles/game.h"
#include "StellarChronicles/sprites.h"
#include "StellarChronicles/manager.h"
// #include "StellarChronicles/stella.h"
const b2Vec2 b2Vec2_zero{0.0f, 0.0f};
class gameInstance : public game
{
public:
	gameInstance(std::string_view name, int x, int y, int w, int h, Uint32 flags)
		: game(name, x, y, w, h, flags), world(b2Vec2_zero) {}
	void INIT()
	{
		texManager.LoadTex(renderer, "0001.png", "01");
		asteroid = new sprites(texManager.GetTex("01"), {1, 1}, 1, {760, 540}, 0.0f, 1.0f, SDL_FLIP_NONE);
		b2BodyDef bodydef;
		bodydef.position = { -4.0f,0.0f };
		bodydef.type = b2_dynamicBody;
		asteroidBody = world.CreateBody(&bodydef);
		b2CircleShape shape;
		shape.m_radius = 1.0f;
		b2FixtureDef fix;
		fix.shape = &shape;
		fix.density = 1.0f;
		asteroidBody->CreateFixture(&fix);
		shape.m_radius = 5.0f;
		fix.shape = &shape;
		fix.isSensor = true;
		fix.density = 0.0f;
		asteroidBody->CreateFixture(&fix);

		asteroid2 = new sprites(texManager.GetTex("01"), {1, 1}, 1, {1160, 540}, 0.0f, 1.0f, SDL_FLIP_NONE);
		bodydef.position = { 4.0f,0.0f };
		bodydef.type = b2_dynamicBody;
		asteroid2Body = world.CreateBody(&bodydef);
		shape.m_radius = 1.0f;
		fix.shape = &shape;
		fix.density = 1.0f;
		fix.isSensor = false;
		asteroid2Body->CreateFixture(&fix);

		b2Vec2 v[4] = {
			{ 19.2f, -10.8f }, // 右上角  
			{ -19.2f,  -10.8f }, // 左上角  
			{-19.2f,  10.8f }, // 左下角  
			{ 19.2f, 10.8f }  // 右下角  
		};
		b2ChainShape chain;
		chain.CreateLoop(v, 4);
		b2BodyDef chainDef;
		chainDef.type = b2_staticBody;
		chainDef.position = b2Vec2_zero;
		
		Edge = world.CreateBody(&chainDef);
		b2FixtureDef edgeDef;
		edgeDef.shape = &chain;
		edgeDef.restitution = 0.9;
		Edge->CreateFixture(&edgeDef);
	}
	bool debugBreak = false;
	void LOOP()
	{
		loop = true;
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
			}
		}

		auto keyboardState = SDL_GetKeyboardState(NULL);
		b2Vec2 force = b2Vec2_zero;
		float Torque = 0.0f;
		if (keyboardState[SDL_SCANCODE_A])
			force += {-10.0f, 0.0f};
		if (keyboardState[SDL_SCANCODE_S])
			force += {0.0f, 10.0f};
		if (keyboardState[SDL_SCANCODE_D])
			force += {10.0f, 0.0f};
		if (keyboardState[SDL_SCANCODE_W])
			force += {0.0f, -10.0f};
		if (keyboardState[SDL_SCANCODE_Q])
			Torque += -1.0f;
		if (keyboardState[SDL_SCANCODE_E])
			Torque += 1.0f;

		asteroidBody->ApplyForceToCenter(force, true);
		asteroidBody->ApplyTorque(Torque, true);
		// 步进模拟物理计算，
		//  传入步长、速度约束求解器的迭代次数和位置约束求解器的迭代次数。
		world.Step(1 / 60.0f, 10, 8);
		// 清空屏幕
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
		SDL_RenderClear(renderer);
		// 把物理世界的坐标转换成SDL渲染的坐标
		// 获取物块的位置和角度
		b2Vec2 position = asteroidBody->GetPosition();
		float angle = asteroidBody->GetAngle();

		asteroid->position = {static_cast<int>(position.x * 50) + 960, static_cast<int>(position.y * 50) + 540};
		asteroid->angle = angle * 180 / 3.14;
		asteroid->draw(renderer);
				SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_Rect aabb = { asteroid->position.x - 50,asteroid->position.y - 50, 100, 100 };
		SDL_RenderDrawRect(renderer, &aabb);
		

		position = asteroid2Body->GetPosition();
		angle = asteroid2Body->GetAngle();

		asteroid2->position = {static_cast<int>(position.x * 50) + 960, static_cast<int>(position.y * 50) + 540};
		asteroid2->angle = angle * 180 / 3.14;

		asteroid2->draw(renderer);

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderDrawLine(renderer, 0, 540, 1920, 540);
		SDL_RenderDrawLine(renderer, 910, 0, 910, 1080);
		// 显示渲染内容
		SDL_RenderPresent(renderer);
		// 按小键盘0中断程序
		if (debugBreak)
			debugBreak = false;
	}
	b2World world;
	manager texManager;
	sprites *asteroid;
	b2Body *asteroidBody;
	sprites *asteroid2;
	b2Body *asteroid2Body;
	b2Body* Edge;

	bool loop = false;
	~gameInstance()
	{
		delete asteroid;
		delete asteroid2;
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