#pragma once
#include <list>

#include "GameObject.h"

class Note;
class Sprite;

enum eJudge;

enum struct eKeyType
{
	NEUTRAL,
	PRESS,
	HOLDING,
};

class Track : public GameObject
{

private:
	std::list<Note*> _noteList;
	std::list<Sprite*> _bgSpriteList;

	Sprite* _judgeEffectSprite;
	Note* _curJudgeNote;

	int _curBarNum;
	int _playTimeTick;
	int _x;
	int _y;

	//판정 관련
private:
	eJudge _judge;
	eKeyType _keyType;
	bool _isPass;

	//판정 범위
	int _judgeDeltaLine;
	int _judgeTick;
	int _judgeStartTick;
	int _judgeEndTick;

	int _judgePerfect_s;
	int _judgePerfect_e;
	int _judgeGreat_s;
	int _judgeGreat_e;

	int _oldDuration;
	int _holdDuration;

public:
	Track(int xPos, int yPos);
	~Track();

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();

	std::list<Note*>& GetNoteList();
	void TrackPosition(int x, int y);
	bool IsPass();
	void AddNoteToTrack(float sec, float duration, int judgeDeltaLine, int barNum);
	void SetPlayBarInfo(int barNum, int playTimeTick);

	//Input
public:
	void JudgeProcess(Note* note, eJudge judge);
	void KeyDown();
	void KeyHold();
	void KeyUp();
};