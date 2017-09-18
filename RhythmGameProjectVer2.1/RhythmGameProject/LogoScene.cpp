#include "SDL.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include "LogoScene.h"
#include "Sprite.h"

LogoScene::LogoScene()
{
	_backgroundSprite = NULL;
}

LogoScene::~LogoScene()
{
	Deinit();
}

void LogoScene::Init()
{
	_backgroundSprite = new Sprite("logoData.csv", true);
	_backgroundSprite->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() / 2);
}

void LogoScene::Deinit()
{
	if (NULL != _backgroundSprite)
	{
		delete _backgroundSprite;
		_backgroundSprite = NULL;
	}
}

void LogoScene::Update(int deltaTime)
{
	_backgroundSprite->Update(deltaTime);
}

void LogoScene::Render()
{
	_backgroundSprite->Render();
}

void LogoScene::KeyDown(int keyCode)
{
	switch (keyCode)
	{
	case SDLK_SPACE:
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_TITLE);
		break;
	case SDLK_ESCAPE:
		GameSystem::GetInstance()->Quit();
		break;
	}
}

void LogoScene::KeyUp(int keyCode)
{

}