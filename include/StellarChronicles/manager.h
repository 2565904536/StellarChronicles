#pragma once
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
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
	// 目前的需求是打印字符到屏幕指定区域上，但是每次打印可能都会创建和销毁材质，开销太大
	// 所以需要预存每个字符的材质，遇到新的字符要存新的对应材质进去
	//  请在manager.cpp完成对应实现
	// todo:
	bool loadFontTex(SDL_Renderer *renderer, char ch);
	// todo:
	SDL_Texture *GetFontTex(char ch);

	bool loadWAV(std::string_view Path, std::string_view name);
	Mix_Chunk* getMusic(std::string_view name);

private:
	std::map<std::string_view, SDL_Texture *> textures;
	std::map<std::string_view, Mix_Chunk*> musics;
	// 我不希望字符材质能被直接访问,所以预存的材质存储在这
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