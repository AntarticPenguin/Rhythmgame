#pragma once

struct Mix_Chunk;

class Wav
{
private:
	int _startTick;
	Mix_Chunk* _wavFile;

	int _playingBarNum;
	int _playTimeTick;
	bool _isPlayed;

	int _updateDuration;

public:
	Wav(float startTime, Mix_Chunk* wavFile);
	~Wav();

public:
	void Init();
	void Update(int deltaTime);
};