#include<print>
#include"SDL2/SDL.h"
#include"SDL2/SDL_image.h"
#include"SDL2/SDL_ttf.h"
#include"box2d/box2d.h"

int main(int argc, char* argv[])
{
	//��ʼ����Ƶϵͳ��ʱ��ϵͳ
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
	//��ʼ��ͼ����������������pngͼ��
	IMG_Init(IMG_INIT_PNG);
	//��ʼ������
	TTF_Init();

	SDL_Window *window = SDL_CreateWindow("Hello world", 100, 100, 1280, 720, false);
	//������Ⱦ��������Ӳ�����ٺʹ�ֱͬ��
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

	std::println("hello world");

	b2Vec2 gravity(0.0f, -10.0f);
	//��������ʼ������
	b2World world(gravity);

	//����һ������
	b2BodyDef groundBodyDef;
	groundBodyDef.position.Set(0.0f, -9.0f);
	//�ø��嶨�崴�������ϵĸ���
	b2Body* groundBody = world.CreateBody(&groundBodyDef);
	//�趨�������״
	b2PolygonShape groundBox;
	//�趨Ϊ���Σ������ߺͰ��
	groundBox.SetAsBox(48.0f, 9.0f);
	//�趨�ܶȣ�����״��Ϊ�о߸��ڸ�����
	//�о��Ǹ��壨body���ϵ�һ��������������˸����������Χ�������н�����
	// ������˵���о߰�������״��shape�����ܶȣ�density����
	// Ħ��ϵ����friction�����ָ�ϵ����restitution��Ҳ��Ϊ����ϵ�������������ԡ�
	groundBody->CreateFixture(&groundBox, 0.0f);

	//����һ������
	b2BodyDef bodyDef;
	//�趨Ϊ��̬���壬���˶���ȫ�������������������������ڸ����ϵ��������������㡣
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(0.0f, 9.0f);
	bodyDef.angle = 3.14 / 3.0;
	//�������д�������
	b2Body* body = world.CreateBody(&bodyDef);

	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(1.0f, 1.0f);
	//����һ���о�
	b2FixtureDef fixtureDef;
	//�趨�оߵ���״
	fixtureDef.shape = &dynamicBox;
	fixtureDef.density = 1.0f;
	//�趨Ħ��ϵ��
	fixtureDef.friction = 0.3f;
	//�趨����ϵ��
	fixtureDef.restitution = 0.9f;
	body->CreateFixture(&fixtureDef);
	//ģ��ʱ�䲽��
	const float timeStep = 1.0f / 165.0f;

	int32 velocityIterations = 6;
	int32 positionIterations = 2;

	const float pixsPerMeter = 1280 / 48.0f;

	//��ȡһ��ͼƬ���õ�����
	//��ȡʧ�ܻ᷵�ؿ�ָ��
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
		//����ģ��������㣬
		// ���벽�����ٶ�Լ��������ĵ���������λ��Լ��������ĵ���������
		world.Step(timeStep, velocityIterations, positionIterations);
		//��ȡ����λ�úͽǶ�
		b2Vec2 position = body->GetPosition();
		float angle = body->GetAngle();
		//�����Ļ
		SDL_RenderClear(renderer);
		//���������������ת����SDL��Ⱦ������
		SDL_FRect dstRect{ 640 - position.x * pixsPerMeter,360 - position.y * pixsPerMeter,2 * pixsPerMeter,2 * pixsPerMeter };
		SDL_FPoint center{ pixsPerMeter,pixsPerMeter };
		//��������
		SDL_RenderCopyExF(renderer, block, nullptr, &dstRect, -angle*180/3.14, &center, SDL_FLIP_NONE);
		//��ʾ��Ⱦ����
		SDL_RenderPresent(renderer);
	} while (loop);
	//������Դ
	SDL_DestroyTexture(block);
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
	return 0;
}