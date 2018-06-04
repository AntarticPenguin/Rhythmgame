#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_mixer.h"
#include "GameSystem.h"
#include "DataManager.h"
#include "TrackManager.h"
#include "Track.h"
#include "Wav.h"
#include "Sprite.h"

TrackManager::TrackManager()
{
	_judgeLineSprite = NULL;
}

TrackManager::~TrackManager()
{
	Deinit();
}

void TrackManager::Init()
{
	_bgSprite = new Sprite("trackBackground.csv", true);
	_bgSprite->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2,
		GameSystem::GetInstance()->GetWindowHeight() / 2);

	_judgeLineSprite = new Sprite("judgeData.csv", true);
	_judgeLineSprite->SetPosition((GameSystem::GetInstance()->GetWindowWidth() / 2),
		GameSystem::GetInstance()->GetWindowHeight() - 100);
	
	//트랙을 받아온다.
	_trackList = DataManager::GetInstance()->GetTrackList();
	_autoWavList = DataManager::GetInstance()->GetAutoWavList();
	_SecondPerBar = DataManager::GetInstance()->GetSecondPerBar();

	for (int i = 0; i < _trackList.size(); i++)
	{
		_trackList[i]->Init();
		_trackList[i]->SetTrackNumber(i);
	}

	//CreateGameNote();
	
	for (int i = 0; i < _trackList.size(); i++)
	{
		_trackList[i]->LastInit();
	}

	_playTimeTick = 0;
	_curBarNum = 0;
}

void TrackManager::Deinit()
{
	if (NULL != _bgSprite)
	{
		delete _bgSprite;
		_bgSprite = NULL;
	}
	
	if (NULL != _judgeLineSprite)
	{
		delete _judgeLineSprite;
		_judgeLineSprite = NULL;
	}
}

void TrackManager::Update(int deltaTime)
{
	_playTimeTick += deltaTime;

	//Track
	{
		for (int i = 0; i < _trackList.size(); i++)
		{
			_trackList[i]->SetPlayBarInfo(_curBarNum, _playTimeTick);
			_trackList[i]->Update(deltaTime);
		}

		if ((_SecondPerBar * (_curBarNum) * 1000) <= _playTimeTick)
			_curBarNum++;
	}

	for (int i = 0; i < _autoWavList.size(); i++)
		_autoWavList[i]->Update(deltaTime);
}

void TrackManager::Render()
{
	_bgSprite->Render();
	_judgeLineSprite->Render();

	for (int i = 0; i < _trackList.size(); i++)
		_trackList[i]->Render();
}