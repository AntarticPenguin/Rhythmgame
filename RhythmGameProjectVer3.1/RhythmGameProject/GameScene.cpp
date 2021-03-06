#include <stdio.h>
#include <string.h>

#include "SDL.h"
#include "SDL_mixer.h"
#include "GameSystem.h"
#include "DataManager.h"
#include "SceneManager.h"
#include "GameScene.h"
#include "Track.h"
#include "Sprite.h"
#include "TrackManager.h"

GameScene::GameScene()
{
	_backgroundSprite = NULL;
	_isPause = false;
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
	float playtimeMin;

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
			else if (!strcmp(token, "PlayTime"))
			{
				token = strtok(NULL, ",\n");
				playtimeMin = atof(token);
			}
		}
	}
	fclose(fp);

	_backgroundSprite = new Sprite(backgroundSpriteName, true);
	_backgroundSprite->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() / 2);

	//노래 시간, 트랙 길이 세팅
	int BPM = 120;
	int tempo = BPM / 60;
	float playTimeSec = playtimeMin * 60.0f;
	int trackheight = GameSystem::GetInstance()->GetWindowHeight() * playTimeSec * tempo;
	trackheight /= 2;
	GameSystem::GetInstance()->SetPlayTimeTick(playTimeSec);
	GameSystem::GetInstance()->SetTrackHeight(trackheight);


	DataManager::GetInstance()->ResetScore();
	DataManager::GetInstance()->ResetCombo();
	_trackManager = new TrackManager();
	_trackManager->Init();

	_gameDuration = 0;

	int result = Mix_Init(MIX_INIT_MP3);
	if (MIX_INIT_MP3 == result)
	{
		char musicPath[256];
		sprintf(musicPath, "../../Resource/music/%s", musicName);
		Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
		Mix_Music* music = Mix_LoadMUS(musicPath);
		Mix_VolumeMusic(GameSystem::GetInstance()->GetMusicVolume());
		if (NULL != music)
		{
			Mix_PlayMusic(music, 0);
		}
		else
		{
			printf("Failed to load mp3 file\n");
		}
	}
	else
	{
		printf("Init Error\n");
	}
}

void GameScene::Deinit()
{
	_trackManager->Deinit();
	if (NULL != _backgroundSprite)
	{
		delete _backgroundSprite;
		_backgroundSprite = NULL;
	}
}

void GameScene::Update(int deltaTime)
{
	//2초 정도 여유
	if (_gameDuration <= GameSystem::GetInstance()->GetPlayTimeTick() + 2000)
	{
		_backgroundSprite->Update(deltaTime);
		_trackManager->Update(deltaTime);

		_gameDuration += deltaTime;
	}
	else
	{
		//STOP MUSIC
		Mix_HaltMusic();
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_TITLE);
	}
}

void GameScene::Render()
{
	_backgroundSprite->Render();
	_trackManager->Render();
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
		musicVolume -= 20;
		Mix_VolumeMusic(musicVolume);
		GameSystem::GetInstance()->SetMusicVolume(musicVolume);
		break;
	case SDLK_F2:
		musicVolume += 20;
		Mix_VolumeMusic(musicVolume);
		GameSystem::GetInstance()->SetMusicVolume(musicVolume);
		break;
	case SDLK_F4:
		_isPause = true;
		PauseGame();
		break;
	case SDLK_ESCAPE:
		Mix_HaltMusic();
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_TITLE);
		break;
	case SDLK_d:
	case SDLK_f:
	case SDLK_j:
	case SDLK_k:
		_trackManager->KeyDown(keyCode);
	default:
		break;
	}
}

void GameScene::KeyUp(int keyCode)
{
	_trackManager->KeyUp(keyCode);
}