#include <stdio.h>

#include "Wav.h"
#include "SDL_mixer.h"
#include "GameSystem.h"

Wav::Wav(float startTime, int barNum, Mix_Chunk* wavFile)
{
	_startTick = GameSystem::GetInstance()->GetPlayTimeTick() - (int)(startTime * 1000.0f);
	_barNum = barNum;
	_wavFile = wavFile;
}

Wav::~Wav()
{
}

void Wav::Init()
{
	_updateDuration = _startTick;
}

void Wav::Update(int deltaTime)
{
	_updateDuration += deltaTime;			//노트 업데이트 시간

	if (GameSystem::GetInstance()->GetPlayTimeTick() <= _updateDuration)
	{
		if (NULL != _wavFile)
		{
			printf("Play Wav!!\n");
			Mix_PlayChannel(-1, _wavFile, 0);
		}
	}
}