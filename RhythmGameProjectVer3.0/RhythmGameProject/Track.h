#pragma once
#include "DLinkedList.h"
#include "GameObject.h"

class Note;
class Sprite;

enum eJudge;

class Track : public GameObject
{

private:
	DLinkedList<Note*> _noteList;
	DLinkedList<Sprite*> _bgSpriteList;
	Sprite* _effectSprite;
	Note* _curJudgeNote;

	int _x;
	int _y;

	//���� ����
private:
	bool _isKeyDown;
	eJudge _judge;
	eJudge _resultJudge;
	bool _isPass;
	bool _isHold;

	//���� ����
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
	eJudge GetResultJudge();
	void ResetResultJudge();
	bool IsPass();
	void resetPass();
	bool isHolding();

	//Input
public:
	void KeyDown();
	void KeyUp();
};