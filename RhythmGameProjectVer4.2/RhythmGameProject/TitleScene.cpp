#include <stdio.h>

#include "SDL.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include "InputSystem.h"
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
	_font->Render();
}

void TitleScene::TrackDefaultKeyInit()
{
	InputSystem::GetInstance()->Init();
	testPrintKey();
}

void TitleScene::testPrintKey()
{
	std::map<int, eTrackButton> map = InputSystem::GetInstance()->GetTrackButton();
	std::map<int, eTrackButton>::iterator itr;

	int trackNumber = 0;

	while (true)
	{
		for (itr = map.begin(); itr != map.end(); itr++)
		{
			if (itr->second == (eTrackButton)trackNumber)
			{
				_inputkey[trackNumber] = (char)(itr->first);
				trackNumber++;
				break;
			}
		}

		if (5 == trackNumber)
		{
			char text[100];
			sprintf(text, "Track1: %c	Track2: %c	Track3: %c	Track4: %c	Track5: %c",
				_inputkey[0], _inputkey[1], _inputkey[2], _inputkey[3], _inputkey[4]);

			_font->SetText(text);
			break;
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
		InputSystem::GetInstance()->ChangeKey();
		testPrintKey();
		break;
	default:
		break;
	}
}

void TitleScene::KeyUp(int keyCode)
{

}