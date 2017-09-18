#include <stdio.h>

#include "SDL_image.h"
#include "GameSystem.h"
#include "ResourceManager.h"
#include "Texture.h"
Texture::Texture() { }
Texture::Texture(const char* fileName)
{
	char filePath[256];
	sprintf(filePath, "../../Resource/image/%s", fileName);

	//_texture = IMG_LoadTexture(GameSystem::GetInstance()->GetRenderer(), filePath);
	_texture = ResourceManager::GetInstance()->FindTexture(filePath);
	SDL_QueryTexture(_texture, NULL, NULL, &_srcRect.w, &_srcRect.h);

	_srcRect.x = 0;
	_srcRect.y = 0;

	_destRect.w = _srcRect.w;
	_destRect.h = _srcRect.h;
}

Texture::~Texture()
{

}

void Texture::SetScale(float wScale, float hScale)
{
	_destRect.w = _srcRect.w * wScale;
	_destRect.h = _srcRect.h * hScale;
}

void Texture::SetPosition(int x, int y)
{
	_destRect.x = x - (GetRenderWidth() / 2);
	_destRect.y = y - (GetRenderHeight() / 2);
}


void Texture::Render()
{
	SDL_RenderCopy(GameSystem::GetInstance()->GetRenderer(), _texture, &_srcRect, &_destRect);
}

int Texture::GetRenderWidth()
{
	return _destRect.w;
}

int Texture::GetRenderHeight()
{
	return _destRect.h;
}