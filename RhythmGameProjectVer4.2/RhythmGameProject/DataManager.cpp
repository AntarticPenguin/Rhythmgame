#include <stdio.h>

#include "DataManager.h"
#include "EffectPlayer.h"
#include "GameSystem.h"
#include "SDL_mixer.h"
#include "Track.h"
#include "Wav.h"

DataManager* DataManager::_instance = NULL;

DataManager::DataManager() 
{
	_combo = 0;
	_score = 0;
	_maxCombo = 0;

	_missCount = 0;
	_greatCount = 0;
	_perfectCount = 0;

	_SecondPerBar = 0.0f;
}

DataManager::~DataManager()
{
	if (0 < _trackList.size())
	{
		for (int i = 0; i < _trackList.size(); i++)
		{
			delete _trackList[i];
		}
		_trackList.clear();
	}

	if (0 < _wavMap.size())
	{
		auto itr = _wavMap.begin();
		for (; itr != _wavMap.end(); itr++)
		{
			delete itr->second;
		}
		_wavMap.clear();
	}

	if (0 < _autoWavList.size())
	{
		for (int i = 0; i < _autoWavList.size(); i++)
		{
			delete _autoWavList[i];
		}
		_autoWavList.clear();
	}
}

DataManager* DataManager::GetInstance()
{
	if (NULL == _instance)
		_instance = new DataManager();
	return _instance;
}

int DataManager::GetCombo()
{
	return _combo;
}

int DataManager::GetMaxCombo()
{
	return _maxCombo;
}

void DataManager::IncreaseCombo()
{
	_combo++;
	
	if (_maxCombo < _combo)
		_maxCombo = _combo;
}

int DataManager::GetScore()
{
	return _score;
}

void DataManager::SetScore(int score)
{
	_score = score;
}

void DataManager::IncreaseScore(eJudge judge)
{
	switch (judge)
	{
	case eJudge::PERFECT:
		_score += (int)(100 * 1.0) + (_combo * 10);
		_perfectCount++;
		break;
	case eJudge::GREAT:
		_score += (int)(100 * 0.8) + (_combo * 10);
		_greatCount++;
		break;
	}
}

void DataManager::ResetCombo()
{
	_combo = 0;
	_missCount++;
}

void DataManager::ResetScore()
{
	_score = 0;
}

int DataManager::GetMissCount()
{
	return _missCount;
}

int DataManager::GetGreatCount()
{
	return _greatCount;
}

int DataManager::GetPerfectCount()
{
	return _perfectCount;
}

void DataManager::SetMusicTitle(std::string title)
{
	_musicTitle = title;
}

std::string DataManager::GetMusicTitle()
{
	return _musicTitle;
}

void DataManager::CreateTracks()
{
	if (0 < _trackList.size())
	{
		for (int i = 0; i < _trackList.size(); i++)
		{
			delete _trackList[i];
		}
		_trackList.clear();
	}

	if (0 < _autoWavList.size())
	{
		_wavMap.clear();
		_autoWavList.clear();
	}

	int trackInterval = -158;
	for (int i = 0; i < 5; i++)
	{
		Track* track = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2) + trackInterval,
			GameSystem::GetInstance()->GetWindowHeight());
		_trackList.push_back(track);
		trackInterval += 79;
	}
}

std::vector<Track*>& DataManager::GetTrackList()
{
	return _trackList;
}

std::vector<Wav*>& DataManager::GetAutoWavList()
{
	return _autoWavList;
}

float DataManager::GetSecondPerBar()
{
	return _SecondPerBar;
}

void DataManager::SetSecondPerBar(float secondPerbar)
{
	_SecondPerBar = secondPerbar;
}

void DataManager::AddWavToMap(const char* filePath, std::string key)
{
	Mix_Chunk* wavChunk = Mix_LoadWAV(filePath);
	if (NULL == wavChunk)
	{
		printf(Mix_GetError());
		printf("\n");
	}

	if (NULL != wavChunk)
		_wavMap[key] = wavChunk;
}

void DataManager::AddNoteToTrack(int trackNum, float sec, float duration, int judgeDeltaLine, int barNum, std::string code)
{
	_trackList[trackNum]->AddNoteToTrack(sec, duration, judgeDeltaLine, barNum, _wavMap[code]);
}

void DataManager::AddAutoNote(float sec, char* code)
{
	Wav* wav = new Wav(sec, _wavMap[code]);
	wav->Init();
	_autoWavList.push_back(wav);
}