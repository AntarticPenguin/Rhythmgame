#include <stdio.h>

#include "SDL.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "Sprite.h"
#include "Texture.h"

TitleScene::TitleScene()
{
	_backgroundSprite = NULL;
}

TitleScene::~TitleScene()
{
	Deinit();
}

void TitleScene::Init()
{
	_backgroundSprite = new Sprite("titleTest.csv", true);
	_backgroundSprite->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() / 2);

	TrackDefaultKeyInit();
}

void TitleScene::Deinit()
{
	if (NULL != _backgroundSprite)
	{
		delete _backgroundSprite;
		_backgroundSprite = NULL;
	}
}

void TitleScene::Update(int deltaTime)
{
	_backgroundSprite->Update(deltaTime);
}

void TitleScene::Render()
{
	_backgroundSprite->Render();
}

void TitleScene::TrackDefaultKeyInit()
{
	_trackButton[SDLK_d] = eTrackButton::TRACK1;
	_trackButton[SDLK_f] = eTrackButton::TRACK2;
	_trackButton[SDLK_SPACE] = eTrackButton::TRACK3;
	_trackButton[SDLK_j] = eTrackButton::TRACK4;
	_trackButton[SDLK_k] = eTrackButton::TRACK5;
}

void TitleScene::ChangeKey()
{
	SDL_Event sdlEvent;
	while (true)
	{
		if (SDL_PollEvent(&sdlEvent))
		{
			if (SDL_KEYDOWN == sdlEvent.type)
			{
				switch (sdlEvent.key.keysym.sym)
				{

				}
			}
		}
	}
}

void TitleScene::KeyDown(int keyCode)
{
	switch (keyCode)
	{
	case SDLK_SPACE:
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_GAME);
		break;
	case SDLK_ESCAPE:
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_LOGO);
		break;
	case SDLK_F1:
		ChangeKey();
		break;
	default:
		break;
	}
}

void TitleScene::KeyUp(int keyCode)
{

}