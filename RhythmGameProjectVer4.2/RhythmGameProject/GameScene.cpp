#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_mixer.h"
#include "GameSystem.h"
#include "DataManager.h"
#include "SceneManager.h"
#include "EffectPlayer.h"
#include "GameScene.h"
#include "Sprite.h"
#include "TrackManager.h"
#include "InputSystem.h"
#include "Font.h"

GameScene::GameScene()
{
	//_backgroundSprite = NULL;
	_isPause = false;

	_combofont = NULL;
	_scorefont = NULL;
}

GameScene::~GameScene()
{
	Deinit();
}

void GameScene::Init()
{
	char filePath[256];
	sprintf(filePath, "../../Resource/%s", "gameplay.csv");
	FILE* fp = fopen(filePath, "r");
	if (NULL == fp)
	{
		printf("파일열기 실패: %s\n", filePath);
	}

	char backgroundSpriteName[256];
	char musicName[256];

	char buffer[256];
	char* record = fgets(buffer, sizeof(buffer), fp);

	while (true)
	{
		record = fgets(buffer, sizeof(buffer), fp);
		if (NULL == record)
			break;

		{
			char* token = strtok(record, ",\n");
			if (!strcmp(token, "Background"))
			{
				token = strtok(NULL, ",\n");
				strcpy(backgroundSpriteName, token);
			}
			else if (!strcmp(token, "MusicName"))
			{
				token = strtok(NULL, ",\n");
				strcpy(musicName, token);
			}
		}
	}
	fclose(fp);

	//_backgroundSprite = new Sprite(backgroundSpriteName, true);
	//_backgroundSprite->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() / 2);

	DataManager::GetInstance()->ResetScore();
	DataManager::GetInstance()->ResetCombo();
	InputSystem::GetInstance()->Init();
	_trackManager = new TrackManager();
	_trackManager->Init();

	_gameDuration = 0;

	int result = Mix_Init(MIX_INIT_MP3);
	if (MIX_INIT_MP3 == result)
	{
		printf("Init Mix Ready\n");
	}
	else
	{
		printf("Init Error\n");
	}

	_combofont = new Font("arialbd.ttf", 40);
	_combofont->SetPosition(GameSystem::GetInstance()->GetWindowWidth() - 250, 150);

	_scorefont = new Font("arialbd.ttf", 32);
	_scorefont->SetPosition(0, 0);
	char text[50];
	sprintf(text, "SCORE %08d", DataManager::GetInstance()->GetScore());
	_scorefont->SetText(text);
}

void GameScene::Deinit()
{
	_trackManager->Deinit();
	/*if (NULL != _backgroundSprite)
	{
		delete _backgroundSprite;
		_backgroundSprite = NULL;
	}*/
	if (NULL != _combofont)
	{
		delete _combofont;
		_combofont = NULL;
	}

	if (NULL != _scorefont)
	{
		delete _scorefont;
		_scorefont = NULL;
	}
}

void GameScene::Update(int deltaTime)
{
	//3초 정도 여유
	if (_gameDuration <= GameSystem::GetInstance()->GetPlayTimeTick() + 3000)
	{
		//_backgroundSprite->Update(deltaTime);
		_trackManager->Update(deltaTime);
		EffectPlayer::GetInstance()->Update(deltaTime);

		_gameDuration += deltaTime;
	}
	else
	{
		//STOP MUSIC
		Mix_HaltMusic();
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_RESULT);
	}

	{
		char text[50];
		sprintf(text, "COMBO %d", DataManager::GetInstance()->GetCombo());
		_combofont->SetText(text);
	}
	{
		char text[50];
		sprintf(text, "SCORE %08d", DataManager::GetInstance()->GetScore());
		_scorefont->SetText(text);
	}
}

void GameScene::Render()
{
	//_backgroundSprite->Render();
	_trackManager->Render();
	EffectPlayer::GetInstance()->Render();
	_combofont->Render();
	_scorefont->Render();
}

void GameScene::PauseGame()
{
	Mix_PauseMusic();

	SDL_Event sdlEvent;
	while (true)
	{
		if (false == _isPause)
			break;

		SDL_PollEvent(&sdlEvent);
		if (SDLK_F5 == sdlEvent.key.keysym.sym)
			ResumeGame();
	}
}

void GameScene::ResumeGame()
{
	Mix_ResumeMusic();
	_isPause = false;
}

void GameScene::KeyDown(int keyCode)
{
	int musicVolume = GameSystem::GetInstance()->GetMusicVolume();
	switch (keyCode)
	{
	case SDLK_F1:				//CONTROL MUSIC VOLUME
		musicVolume -= 10;
		GameSystem::GetInstance()->SetMusicVolume(musicVolume);
		break;
	case SDLK_F2:
		musicVolume += 10;
		GameSystem::GetInstance()->SetMusicVolume(musicVolume);
		break;
	case SDLK_F4:
		_isPause = true;
		PauseGame();
		break;
	case SDLK_ESCAPE:
		Mix_HaltChannel(-1);
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_TITLE);
		break;
	case SDLK_COMMA:
		GameSystem::GetInstance()->DecreaseTrackHeight();
		break;
	case SDLK_PERIOD:
		GameSystem::GetInstance()->IncreaseTrackHeight();
		break;
	default:
		InputSystem::GetInstance()->KeyDown(keyCode);
		break;
	}
}

void GameScene::KeyUp(int keyCode)
{
	InputSystem::GetInstance()->KeyUp(keyCode);
}