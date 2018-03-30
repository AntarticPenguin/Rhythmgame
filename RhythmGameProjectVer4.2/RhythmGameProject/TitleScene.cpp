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
	_trackKeyFont = NULL;
}

TitleScene::~TitleScene()
{
	Deinit();
}

void TitleScene::Init()
{
	_backgroundSprite = new Sprite("titleTest.csv", true);
	_backgroundSprite->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() / 2);

	_trackKeyFont = new Font("arialbd.ttf", 32);
	_trackKeyFont->SetPosition(0, 0);

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
	_trackKeyFont->Render();
}

void TitleScene::TrackDefaultKeyInit()
{
	_trackButton[SDLK_d] = eTrackButton::TRACK1;
	_trackButton[SDLK_f] = eTrackButton::TRACK2;
	_trackButton[SDLK_SPACE] = eTrackButton::TRACK3;
	_trackButton[SDLK_j] = eTrackButton::TRACK4;
	_trackButton[SDLK_k] = eTrackButton::TRACK5;

	_inputkeyTest[0] = SDLK_d;
	_inputkeyTest[1] = SDLK_f;
	_inputkeyTest[2] = SDLK_SPACE;
	_inputkeyTest[3] = SDLK_j;
	_inputkeyTest[4] = SDLK_k;

	char text[100];
	sprintf(text, "Track1: %c\n Track2: %c\n Track3: %c\n Track4: %c\n Track5: %c\n",
		_inputkeyTest[0], _inputkeyTest[1], _inputkeyTest[2], _inputkeyTest[3], _inputkeyTest[4]);
		
	_trackKeyFont->SetText(text);
}

void TitleScene::ChangeKey()
{
	SDL_Event sdlEvent;
	int trackNumber = eTrackButton::TRACK1;
	while (true)
	{
		if (SDL_PollEvent(&sdlEvent))
		{
			if (SDL_KEYDOWN == sdlEvent.type)
			{
				int keycode = sdlEvent.key.keysym.sym;

				_trackButton[keycode] = (eTrackButton)trackNumber;
				_inputkeyTest[trackNumber] = keycode;
				trackNumber++;
			}
		}

		if (5 == trackNumber)
		{
			char text[100];
			sprintf(text, "Track1: %c\n Track2: %c\n Track3: %c\n Track4: %c\n Track5: %c\n",
				_inputkeyTest[0], _inputkeyTest[1], _inputkeyTest[2], _inputkeyTest[3], _inputkeyTest[4]);

			_trackKeyFont->SetText(text);
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
		ChangeKey();
		break;
	default:
		break;
	}
}

void TitleScene::KeyUp(int keyCode)
{

}