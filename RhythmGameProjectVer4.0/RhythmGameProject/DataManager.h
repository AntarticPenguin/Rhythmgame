#pragma once

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

public:
	int GetCombo();
	void SetCombo(int combo);
	void IncreaseCombo();

	int GetScore();
	void SetScore(int score);
	void ScoreGreat();
	void ScorePerfect();

	void ResetCombo();
	void ResetScore();

private:
	DataManager();
};