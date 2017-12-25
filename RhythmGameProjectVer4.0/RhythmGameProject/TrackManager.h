#pragma once
#include "GameObject.h"
#include "Array.h"
#include "DLinkedList.h"

class Track;
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

typedef struct sNoteLine
{
	char line[1024];
	int BarNum;
}sNoteLine;

typedef struct sNoteInfo
{
	char note[3];
	int startTick;
	int durationTick;
}sNoteInfo;

class TrackManager : public GameObject
{
public:
	enum eTrackNum
	{
		TRACK01,
		TRACK02,
		TRACK03,
		TRACK04,
		TRACK05,
	};

private:
	Array<Track*>* _trackList;

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

	//BMS Parsing & Create Note
private:
	DLinkedList<sNoteLine*>* _trackNoteList;

	int _BPM;				// BMSE 스크립트를 통해 BPM을 구한다.
	float _SecondPerBar;	// BPM에 따른 1마디당 초를 구한다.	(1/32박자 기준)
	char _longNoteKey[3];

public:
	void ParsingBMS(const char* fileName);
	void CreateGameNote();

	//input
public:
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};