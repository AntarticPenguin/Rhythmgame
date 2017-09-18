#pragma once
#include "GameObject.h"

class Sprite;

class Note : public GameObject
{
public:
	Note(float startTime, int judgeDeltaLine);
	~Note();

private:
	Sprite* _sprite;
	int _updateDuration;
	int _x;
	int _y;
	int _startTick;
	bool _isLive;
	int _judgeDeltaLine;
	bool _isPass;

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();
	void UpdatePosition(int deltaTime);
	bool IsLive();
	int GetNoteTime();
	bool IsPass();
	void Pass();
	void SetXPosition(int x);
};