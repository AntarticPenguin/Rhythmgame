#pragma once
#include <iostream>
#include <map>
#include <string>

#include "SDL.h"
#include "SDL_image.h"

class ResourceManager
{
	//Singleton
private:
	static ResourceManager* _instance;

public:
	static ResourceManager* GetInstance();
	SDL_Texture* FindTexture(std::string filePath);
	~ResourceManager();

private:
	std::map<std::string, SDL_Texture*> _textureMap;

private:
	ResourceManager();
};