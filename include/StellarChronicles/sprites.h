#pragma once
#include <SDL2/SDL.h>
// 这个类用于渲染一个精灵，需要它在屏幕里的数据
class sprites
{
private:
    Uint32 index;
    SDL_Texture *texture;
    SDL_Point frameSize;
    Uint32 frameNum;

public:
    sprites(SDL_Texture *texture, SDL_Point frameSize, Uint32 frameNum, float angle, float scale, SDL_RendererFlip flip);
    sprites() = default;
    sprites(const sprites&) = default;
    ~sprites() = default;

    // 精灵的大小
    SDL_Point size;
    // 精灵的角度
    float angle;
    // 精灵的缩放
    float scale;
    // 精灵的翻转类型
    SDL_RendererFlip flip;

    void draw(SDL_Renderer* renderer, SDL_Point position, float scale);
    void nextFrame();


};
