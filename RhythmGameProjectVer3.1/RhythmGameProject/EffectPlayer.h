#pragma once
#include "Array.h"

class Sprite;

enum eEffect
{
	eMISS,
	eGREAT,
	ePERFECT,
};

class EffectPlayer
{
	//Singleton
private:
	static EffectPlayer* _instance;
	Array<Sprite*>* _effectList;
	Sprite* _curEffect;
	Sprite* _oldEffect;

public:
	static EffectPlayer* GetInstance();
	void Update(int deltaTime);
	void Render();
	void Play(int code);

private:
	EffectPlayer();
};