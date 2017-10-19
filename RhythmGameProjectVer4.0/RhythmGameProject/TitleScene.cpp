#include <stdio.h>

#include "SDL.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "Sprite.h"

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
	default:
		break;
	}
}

void TitleScene::KeyUp(int keyCode)
{

}