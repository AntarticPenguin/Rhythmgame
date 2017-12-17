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

typedef struct sNoteInfo
{
	char noteLine[1024];
	int BarNum;
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
	DLinkedList<sNoteInfo*>* _trackNoteList;

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
	//void CreateGameNote(const char* fileName);
	void ParsingBMS(const char* fileName);
	void CreateGameNote();

	//input
public:
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};