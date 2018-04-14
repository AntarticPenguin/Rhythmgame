#pragma once

struct Mix_Chunk;

class Wav
{
private:
	int _startTick;
	int _barNum;
	Mix_Chunk* _wavFile;

	int _updateDuration;


public:
	Wav(float startTime, int barNum, Mix_Chunk* wavFile);
	~Wav();

public:
	void Init();
	void Update(int deltaTime);
};