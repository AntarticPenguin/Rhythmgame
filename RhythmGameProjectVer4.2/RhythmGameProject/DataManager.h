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
	int _score;

	//SCORE & COMBO
public:
	int GetCombo();
	void SetCombo(int combo);
	void IncreaseCombo();

	int GetScore();
	void SetScore(int score);
	void IncreaseScore(eJudge judge);

	void ResetCombo();
	void ResetScore();

	//Music Title
private:
	std::string _musicTitle;

public:
	void SetMusicTitle(std::string title);
	std::string GetMusicTitle();

private:
	DataManager();
};