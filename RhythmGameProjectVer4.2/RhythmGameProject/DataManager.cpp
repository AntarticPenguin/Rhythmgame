#include <stdio.h>

#include "DataManager.h"
#include "EffectPlayer.h"

DataManager* DataManager::_instance = NULL;

DataManager::DataManager() { }

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

void DataManager::SetCombo(int combo)
{
	_combo = combo;
}

void DataManager::IncreaseCombo()
{
	_combo++;
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
		break;
	case eJudge::GREAT:
		_score += (int)(100 * 0.8) + (_combo * 10);
		break;
	}
}

void DataManager::ResetCombo()
{
	_combo = 0;
}

void DataManager::ResetScore()
{
	_score = 0;
}

void DataManager::SetMusicTitle(std::string title)
{
	_musicTitle = title;
}

std::string DataManager::GetMusicTitle()
{
	return _musicTitle;
}