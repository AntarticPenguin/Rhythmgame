#pragma once
#include "DLinkedList.h"
#include "GameObject.h"

class Note;
class Sprite;

enum eJudge;

enum eKeyType
{
	NEUTRAL,
	PRESS,
	HOLDING,
};

class Track : public GameObject
{

private:
	DLinkedList<Note*> _noteList;
	DLinkedList<Sprite*> _bgSpriteList;

	Sprite* _judgeEffectSprite;
	Note* _curJudgeNote;

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

public:
	Track();
	~Track();

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();
	DLinkedList<Note*>& GetNoteList();
	void SetPosition(int x, int y);
	bool IsPass();
	void resetPass();

	//Input
public:
	void KeyDown();
	void KeyUp();
};