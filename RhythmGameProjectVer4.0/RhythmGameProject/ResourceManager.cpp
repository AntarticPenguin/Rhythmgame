#include <stdio.h>

#include "ResourceManager.h"
#include "GameSystem.h"

using namespace std;

ResourceManager* ResourceManager::_instance = NULL;

ResourceManager::ResourceManager(){ }

ResourceManager::~ResourceManager()
{
	map<string, SDL_Texture*>::iterator itr;
	for (itr = _textureMap.begin(); itr != _textureMap.end(); itr++)
	{
		delete itr->second;
	}
	_textureMap.clear();
}

ResourceManager* ResourceManager::GetInstance()
{
	if (NULL == _instance)
		_instance = new ResourceManager();
	return _instance;
}

SDL_Texture* ResourceManager::FindTexture(string filePath)
{
	map<string, SDL_Texture*>::iterator itr = _textureMap.find(filePath);

	if (itr != _textureMap.end())		//찾는 값이 있을 때
	{
		return itr->second;
	}
	
	SDL_Texture* texture = IMG_LoadTexture(GameSystem::GetInstance()->GetRenderer(), filePath.c_str());	//const char* 로 변환
	_textureMap[filePath] = texture;

	return texture;
}