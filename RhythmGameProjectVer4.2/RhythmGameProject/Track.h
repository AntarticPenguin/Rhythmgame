#pragma once
#include <list>

#include "GameObject.h"

class Note;
class Sprite;
struct Mix_Chunk;

class Track : public GameObject
{

private:
	std::list<Note*> _noteList;
	Sprite* _trackEffectSprite;
	Sprite* _judgeEffectSprite;

	int _trackNumber;
	int _curBarNum;
	int _playTimeTick;
	int _x;
	int _y;

	//판정 관련
private:
	std::list<Note*>::iterator _curNote;
	bool _isJudging;

	eJudge _judge;

	//판정 범위
	int _judgeDeltaLine;
	int _judgeTick;
	int _judgeStartTick;
	int _judgeEndTick;

	int _judgePerfect_s;
	int _judgePerfect_e;
	int _judgeGreat_s;
	int _judgeGreat_e;

public:
	Track(int xPos, int yPos);
	~Track();

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();

	void LastInit();
	void UpdateNoteList(int deltaTime);

	std::list<Note*>& GetNoteList();
	void TrackPosition(int x, int y);
	void SetTrackNumber(int trackNumber);
	void AddNoteToTrack(float sec, float duration, int judgeDeltaLine, int barNum, Mix_Chunk* wavFile);
	void SetPlayBarInfo(int barNum, int playTimeTick);

	//Judge 관련
public:
	void UpdateInput();
	eJudge CheckJudge(int noteTime);
	void Judge(eJudge judge);
};