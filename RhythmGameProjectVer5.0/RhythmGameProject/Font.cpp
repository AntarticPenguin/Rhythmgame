#include <stdio.h>

#include "SDL_ttf.h"
#include "GameSystem.h"
#include "Font.h"

Font::Font(const char* fileName, int fontSize)
{
	char filePath[256];
	sprintf(filePath, "../../Resource/Font/%s", fileName);

	_sansFont = TTF_OpenFont(filePath, fontSize);
}

Font::~Font()
{

}

void Font::SetText(const char* text)
{
	SDL_Color fontcolor = { 255, 255, 255 };
	SDL_Surface* surfaceMessage = TTF_RenderText_Solid(_sansFont, text, fontcolor);
	_texture = SDL_CreateTextureFromSurface(GameSystem::GetInstance()->GetRenderer(), surfaceMessage);
	SDL_QueryTexture(_texture, NULL, NULL, &_srcRect.w, &_srcRect.h);

	_srcRect.x = 0;
	_srcRect.y = 0;

	_destRect.w = _srcRect.w;
	_destRect.h = _srcRect.h;
}