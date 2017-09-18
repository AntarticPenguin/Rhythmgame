#pragma once
#include "GameObject.h"
#include "Array.h"

class Track;
class Sprite;
class Font;

enum eJudge
{
	NONE,
	PERFECT,
	GREAT,
	MISS,
	JUDGE_START_PERFECT,
	JUDGE_START_GREAT,
};

class TrackManager : public GameObject
{
public:
	enum eEffect
	{
		MISS,
		GREAT,
		PERFECT,
	};

	enum eTrackNum
	{
		TRACK01,
		TRACK02,
		TRACK03,
		TRACK04,
	};

private:
	Array<Track*>* _trackList;
	Array<Sprite*>* _effectList;
	Sprite* _curEffect;

	int _combo;
	eJudge _resultJudge;

	int _score;

	Font* _combofont;
	Font* _scorefont;

public:
	TrackManager();
	~TrackManager();

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();
	void checkPass(Track* track);

	//input
public:
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};