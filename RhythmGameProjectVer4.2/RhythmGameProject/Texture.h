#pragma once
#include "SDL_image.h"

class Texture
{
public:
	Texture();
	Texture(const char* fileName);
	~Texture();

protected:
	SDL_Texture* _texture;
	SDL_Rect _srcRect;
	SDL_Rect _destRect;

public:
	void SetScale(float wScale, float hScale);
	void SetLength(int length);
	void SetPosition(int x, int y);
	void SetAlpha(int value);
	void Render();
	int GetRenderWidth();
	int GetRenderHeight();
};