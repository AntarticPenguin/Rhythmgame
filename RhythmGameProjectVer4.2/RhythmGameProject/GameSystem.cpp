#include <stdio.h>
#include "GameSystem.h"
#include "SDL.h"

GameSystem* GameSystem::_instance = NULL;

GameSystem::GameSystem()
{
	_musicVolume = 80;
	_pauseTime = 0;
}

GameSystem* GameSystem::GetInstance()
{
	if (NULL == _instance)
		_instance = new GameSystem();
	return _instance;
}

int GameSystem::GetWindowWidth()
{
	return 1024;
}

int GameSystem::GetWindowHeight()
{
	return 800;
}

void GameSystem::Quit()
{
	SDL_Event quitEvent;
	quitEvent.type = SDL_QUIT;
	SDL_PushEvent(&quitEvent);		//ť�� �̺�Ʈ Push
}

void GameSystem::SetRenderer(SDL_Renderer* sdlRenderer)
{
	_sdlRenderer = sdlRenderer;
}

SDL_Renderer* GameSystem::GetRenderer()
{
	return _sdlRenderer;
}

void GameSystem::SetPlayTimeTick(float sec)
{
	_playTimeTick = (int)(sec * 1000.0f);
}

int GameSystem::GetPlayTimeTick()
{
	return _playTimeTick;	//��ü ���� �ð�(�÷��� �ð�)
}


void GameSystem::SetTrackHeight(int height)
{
	_trackHeight = height;
}

int GameSystem::GetTrackHeight()
{
	return _trackHeight;	//Ʈ������
}

void GameSystem::DecreaseTrackHeight()
{
	_trackHeight -= _trackHeight / 10;
}

void GameSystem::IncreaseTrackHeight()
{
	_trackHeight += _trackHeight / 10;
}

int GameSystem::GetMusicVolume()
{
	return _musicVolume;
}

void GameSystem::SetMusicVolume(int volume)
{
	_musicVolume = volume;
}

int GameSystem::GetPauseTime()
{
	return _pauseTime;
}

void GameSystem::SetPauseTime(int pauseTime)
{
	_pauseTime = pauseTime;
}