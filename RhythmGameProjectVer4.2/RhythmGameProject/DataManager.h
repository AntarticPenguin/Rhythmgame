#pragma once
#include <string>

enum eJudge;

class DataManager
{
	//Singleton
private:
	static DataManager* _instance;

public:
	static DataManager* GetInstance();

public:
	int _combo;
	int _maxCombo;
	int _score;

	int _missCount;
	int _greatCount;
	int _perfectCount;

	//SCORE & COMBO & JUDGE
public:
	int GetCombo();
	int GetMaxCombo();
	void IncreaseCombo();

	int GetScore();
	void SetScore(int score);
	void IncreaseScore(eJudge judge);

	void ResetCombo();
	void ResetScore();

	int GetMissCount();
	int GetGreatCount();
	int GetPerfectCount();

	//Music Title
private:
	std::string _musicTitle;

public:
	void SetMusicTitle(std::string title);
	std::string GetMusicTitle();

private:
	DataManager();
};