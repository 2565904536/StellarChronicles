#pragma once
#include "SDL2/SDL_image.h"
#include <map>
#include <string_view>
#include "StellarChronicles/stella.h"
#include <queue>

class manager
{
public:
	manager() = default;
	~manager();
	bool LoadTex(SDL_Renderer *renderer, std::string_view Path, std::string_view Name);
	SDL_Texture *GetTex(std::string_view Name);
	// Ŀǰ�������Ǵ�ӡ�ַ�����Ļָ�������ϣ�����ÿ�δ�ӡ���ܶ��ᴴ�������ٲ��ʣ�����̫��
	// ������ҪԤ��ÿ���ַ��Ĳ��ʣ������µ��ַ�Ҫ���µĶ�Ӧ���ʽ�ȥ
	//  ����manager.cpp��ɶ�Ӧʵ��
	// todo:
	bool loadFontTex(SDL_Renderer *renderer, char ch);
	// todo:
	SDL_Texture *GetFontTex(char ch);

private:
	std::map<std::string_view, SDL_Texture *> textures;

	// �Ҳ�ϣ���ַ������ܱ�ֱ�ӷ���,����Ԥ��Ĳ��ʴ洢����
	// todo:
};

class objectPool
{
public:
	objectPool() = default;
	~objectPool();
	std::queue<galaxy *> pool;
	galaxy* getObj();
	void recircleObj(galaxy *obj);
	void operator+=(galaxy *obj);
	private:
};