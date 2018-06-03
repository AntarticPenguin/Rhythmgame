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
#include "Note.h"
#include "Sprite.h"

TrackManager::TrackManager()
{
	//_trackList = NULL;
	//_trackNoteList = NULL;
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

	//_trackNoteList = new std::list<sNoteLine*>[_trackList.size()];

	//BMS파싱 및 노트 생성
	//memset(_longNoteKey, 0, sizeof(_longNoteKey));
	//ParsingBMS(DataManager::GetInstance()->GetMusicTitle().c_str());

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
	//{
	//	std::list<Wav*>::iterator itr;
	//	for (itr = _autoWavList.begin(); itr != _autoWavList.end(); itr++)
	//		delete (*itr);
	//	_autoWavList.clear();
	//}

	//_wavMap.clear();

	//if (NULL != _trackList)
	//{
	//	for (int i = 0; i < _trackList->size(); i++)
	//	{
	//		delete _trackList->at(i);
	//	}
	//	_trackList = NULL;
	//}

	//std::list<sNoteLine*>::iterator itr;
	//for (itr = _trackNoteList->begin(); itr != _trackNoteList->end(); itr++)
	//{
	//	delete (*itr);
	//}
	//_trackNoteList->clear();

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

	std::list<Wav*>::iterator itr;
	for (itr = _autoWavList.begin(); itr != _autoWavList.end(); itr++)
		(*itr)->Update(deltaTime);
}

void TrackManager::Render()
{
	_bgSprite->Render();
	_judgeLineSprite->Render();

	for (int i = 0; i < _trackList.size(); i++)
		_trackList[i]->Render();
}