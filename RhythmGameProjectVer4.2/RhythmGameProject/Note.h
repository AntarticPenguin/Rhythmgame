#pragma once

#include "GameObject.h"

class Sprite;
struct Mix_Chunk;

class Note : public GameObject
{
public:
	Note(float startTime, float duration, int judgeDeltaLine, int barNum, Mix_Chunk* wavFile);
	~Note();

private:
	Sprite* _sprite;
	Sprite* _longSprite;
	int _updateDuration;
	int _x;
	int _y;
	int _startTick;
	int _longDurTick;
	int _barNum;
	Mix_Chunk* _wavFile;

	bool _isStart;

	//JUDGE
	bool _isLive;
	int _judgeDeltaLine;
	bool _isPass;
	bool _isReduceDuration;

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();
	void UpdatePosition(int deltaTime);
	void UpdateLongnoteLength();
	void AdjustmentLength();

public:
	bool IsLive();
	int GetNoteTime();

	int GetDuration();
	void EnableReduceDuration();

	bool IsPass();
	void Pass();
	void SetLive(bool isLive);

	void SetXPosition(int x);

	int GetBarNum();
	void Start(int playTimeTick);

	void PlayWav();
};