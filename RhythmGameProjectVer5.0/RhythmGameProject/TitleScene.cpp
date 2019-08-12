#include <stdio.h>

#include "SDL.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include "TitleScene.h"
#include "Sprite.h"
#include "Texture.h"
#include "Font.h"

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

	_font = new Font("arialbd.ttf", 32);
	_font->SetPosition(100, 100);

	_font->SetText("타이틀 화면");
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
	_font->Render();
}

void TitleScene::InputProcess()
{
	if (Input::GetInstance().IsPressed(eGameKey::BTN_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_SELECT);
	}
	else if (Input::GetInstance().IsPressed(eGameKey::MENU))
	{
		GameSystem::GetInstance()->Quit();
	}
}