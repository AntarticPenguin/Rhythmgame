#include <stdio.h>

#include "Wav.h"
#include <stdio.h>
#include "SDL_mixer.h"
#include "GameSystem.h"

Wav::Wav(float startTime, Mix_Chunk* wavFile)
{
	_startTick = GameSystem::GetInstance()->GetPlayTimeTick() - (int)(startTime * 1000.0f);
	_wavFile = wavFile;

	_playingBarNum = 0;
	_playTimeTick = 0;

	_isPlayed = false;
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
	if (true == _isPlayed)
		return;

	_updateDuration += deltaTime;			//노트 업데이트 시간

	if (GameSystem::GetInstance()->GetPlayTimeTick() <= _updateDuration)
	{
		if (NULL != _wavFile)
		{
			Mix_VolumeChunk(_wavFile, GameSystem::GetInstance()->GetMusicVolume());
			Mix_PlayChannel(-1, _wavFile, 0);
			_isPlayed = true;
		}
	}
}