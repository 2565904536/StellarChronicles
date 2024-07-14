#pragma once
#include "SDL2/SDL.h"
struct vec2
{
	float x = 0.0f;
	float y = 0.0f;
	vec2 operator+(const vec2& b) const
	{
		return { this->x + b.x,this->y + b.y };
	}

	vec2 operator-(const vec2& b) const
	{
		return { this->x - b.x,this->y - b.y };
	}

	vec2 operator*(float b) const
	{
		return { b * this->x,b * this->y };
	}
	inline void operator+=(const vec2& b) 
	{
		*this = *this+ b;
	}
	inline float lengthSqure() const
	{
		return this->x * this->x + this->y * this->y;
	}

	inline float length() const
	{
		return sqrtf(lengthSqure());
	}
	float operator*(const vec2& b) const
	{
		return x* b.x + y * b.y;
	}
	vec2 operator/(float b) const
	{
		return vec2{ x / b,y / b };
	}
	float angle() const
	{
		return atan2f(y, x);
	}
};
class camera
{
public:
	camera(vec2 position,float scale,float angle);
	camera() = default;
	vec2 position = { 0.0f,0.0f };
	float scale = 1.0f;
	float angle = 0.0f;
};