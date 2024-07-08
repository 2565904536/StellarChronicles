#include<print>
#include"SDL2/SDL.h"
#include"SDL2/SDL_image.h"
#include"SDL2/SDL_ttf.h"
#include"box2d/box2d.h"

int main(int argc, char* argv[])
{
	//初始化视频系统和时间系统
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	//初始化图像加载器，允许加载png图像
	IMG_Init(IMG_INIT_PNG);
	//初始化字体
	TTF_Init();

	SDL_Window *window = SDL_CreateWindow("Hello world", 100, 100, 1280, 720, false);
	//创建渲染器，开启硬件加速和垂直同步
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	std::println("hello world");

	b2Vec2 gravity(0.0f, -10.0f);
	//用重力初始化世界
	b2World world(gravity);

	//定义一个刚体
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -9.0f);
	//用刚体定义创建世界上的刚体
	b2Body* groundBody = world.CreateBody(&groundBodyDef);
	//设定多边形形状
	b2PolygonShape groundBox;
	//设定为矩形，传入半高和半宽
	groundBox.SetAsBox(48.0f, 9.0f);
	//设定密度，把形状作为夹具附在刚体上
	//夹具是刚体（body）上的一个组件，它定义了刚体如何与周围环境进行交互。
	// 具体来说，夹具包含了形状（shape）、密度（density）、
	// 摩擦系数（friction）、恢复系数（restitution，也称为弹性系数）等物理属性。
	groundBody->CreateFixture(&groundBox, 0.0f);

	//定义一个刚体
	b2BodyDef bodyDef;
	//设定为动态刚体，其运动完全由物理引擎的求解器根据作用在刚体上的力和力矩来计算。
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 9.0f);
	bodyDef.angle = 3.14 / 3.0;
	//在世界中创建刚体
	b2Body* body = world.CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);
	//定义一个夹具
	b2FixtureDef fixtureDef;
	//设定夹具的形状
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	//设定摩擦系数
	fixtureDef.friction = 0.3f;
	//设定弹性系数
	fixtureDef.restitution = 0.9f;
	body->CreateFixture(&fixtureDef);
	//模拟时间步长
	const float timeStep = 1.0f / 165.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	const float pixsPerMeter = 1280 / 48.0f;

	//读取一张图片，得到材质
	//读取失败会返回空指针
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
		//步进模拟物理计算，
		// 传入步长、速度约束求解器的迭代次数和位置约束求解器的迭代次数。
		world.Step(timeStep, velocityIterations, positionIterations);
		//获取物块的位置和角度
		b2Vec2 position = body->GetPosition();
		float angle = body->GetAngle();
		//清空屏幕
		SDL_RenderClear(renderer);
		//把物理世界的坐标转换成SDL渲染的坐标
		SDL_FRect dstRect{ 640 - position.x * pixsPerMeter,360 - position.y * pixsPerMeter,2 * pixsPerMeter,2 * pixsPerMeter };
		SDL_FPoint center{ pixsPerMeter,pixsPerMeter };
		//画出材质
		SDL_RenderCopyExF(renderer, block, nullptr, &dstRect, -angle*180/3.14, &center, SDL_FLIP_NONE);
		//显示渲染内容
		SDL_RenderPresent(renderer);
	} while (loop);
	//回收资源
	SDL_DestroyTexture(block);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}