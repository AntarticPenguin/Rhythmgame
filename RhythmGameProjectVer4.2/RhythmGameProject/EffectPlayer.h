#pragma once
#include <vector>

class Sprite;

//enum eEffect
//{
//	eMISS,
//	eGREAT,
//	ePERFECT,
//};

enum eJudge
{
	MISS,
	GREAT,
	PERFECT,
	NONE,	
};

class EffectPlayer
{
	//Singleton
private:
	static EffectPlayer* _instance;
	std::vector<Sprite*>* _effectList;
	Sprite* _curEffect;
	Sprite* _oldEffect;

public:
	static EffectPlayer* GetInstance();
	void Update(int deltaTime);
	void Render();
	void Play(eJudge judge);
	void Stop();

private:
	EffectPlayer();
};