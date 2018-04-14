#include <stdio.h>

#include "DataManager.h"
#include "EffectPlayer.h"

DataManager* DataManager::_instance = NULL;

DataManager::DataManager() 
{
	_combo = 0;
	_score = 0;
	_maxCombo = 0;

	_missCount = 0;
	_greatCount = 0;
	_perfectCount = 0;
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