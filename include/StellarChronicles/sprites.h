#pragma once
#include <SDL2/SDL.h>
// �����������Ⱦһ�����飬��Ҫ������Ļ�������
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

    // ����Ĵ�С
    SDL_Point size;
    // ����ĽǶ�
    float angle;
    // ���������
    float scale;
    // ����ķ�ת����
    SDL_RendererFlip flip;

    void draw(SDL_Renderer* renderer, SDL_Point position, float scale);
    void nextFrame();


};
