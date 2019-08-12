#include "GameSystem.h"
#include "SDL_mixer.h"
#include "Note.h"
#include "Sprite.h"

Note::Note(float startTime, float duration, int judgeDeltaLine, int barNum, Mix_Chunk* wavFile)
{
	_sprite = NULL;
	_longSprite = NULL;
	_startTick = GameSystem::GetInstance()->GetPlayTimeTick() - (int)(startTime * 1000.0f);
	_longDurTick = (int)(duration * 1000.0f);
	_barNum = barNum;
	_wavFile = wavFile;

	_isLive = true;
	_judgeDeltaLine = judgeDeltaLine;
	_isPass = false;
	_isReduceDuration = false;

	_isStart = false;
}

Note::~Note()
{
	Deinit();
}

void Note::Init()
{
	_sprite = new Sprite("noteSprite.csv", true);
	_longSprite = new Sprite("longNoteSprite.csv", true);
	
	_updateDuration = _startTick;

	if (0 < _longDurTick)		// 0이면 단노트, 0이상이면 롱노트
	{
		float lengthRate = (float)_longDurTick / (float)GameSystem::GetInstance()->GetPlayTimeTick();
		int lengthPerSec = (int)GameSystem::GetInstance()->GetTrackHeight() * lengthRate;
		_longSprite->SetLength(lengthPerSec);
		_longSprite->SetPivotY(1.0f);
	}

	UpdatePosition(0);
}

void Note::Deinit()
{
	if (NULL != _sprite)
	{
		delete _sprite;
		_sprite = NULL;
	}

	if (NULL != _longSprite)
	{
		delete _longSprite;
		_longSprite = NULL;
	}
}

void Note::Update(int deltaTime)
{
	if (false == _isLive || false == _isStart)
		return;

	if (0 < _longDurTick)
		_longSprite->Update(deltaTime);
	_sprite->Update(deltaTime);
	UpdatePosition(deltaTime);
}

void Note::Render()
{
	if (false == _isLive || false == _isStart)
		return;

	if (0 < _longDurTick)
		_longSprite->Render();
	_sprite->Render();
}

void Note::UpdatePosition(int deltaTime)
{
	_updateDuration += deltaTime;			//노트 업데이트 시간

	if (true == _isReduceDuration)
	{
		_longDurTick -= deltaTime;
		if (_longDurTick < -90)
		{
			_longDurTick = 0;
			_isReduceDuration = false;
		}

		UpdateLongnoteLength();
		_updateDuration = GameSystem::GetInstance()->GetPlayTimeTick();
	}

	if (_updateDuration <= GameSystem::GetInstance()->GetPlayTimeTick() + 200)
	{
		float positionRate = (float)_updateDuration / (float)GameSystem::GetInstance()->GetPlayTimeTick();	//위치 비율
		float positionInSec = GameSystem::GetInstance()->GetTrackHeight() * positionRate;					//트랙길이에 비율을 곱한 위치
		_y = positionInSec - (GameSystem::GetInstance()->GetTrackHeight() - GameSystem::GetInstance()->GetWindowHeight() + _judgeDeltaLine);

		_sprite->SetPosition(_x, _y);
		_longSprite->SetPosition(_x, _y);
	}
	else
	{
		_isLive = false;
	}
}

void Note::UpdateLongnoteLength()
{
	float lengthRate = (float)(_longDurTick) / (float)GameSystem::GetInstance()->GetPlayTimeTick();
	int lengthPerSec = (int)GameSystem::GetInstance()->GetTrackHeight() * lengthRate;
	_longSprite->SetLength(lengthPerSec);
}

void Note::AdjustmentLength()
{
	/*	
		롱노트를 눌렀을때에 따라 길이가 갑자기 길어지거나 짧아지는
		미묘한 차이가 있는데 그것을 보정
		1. gap을 구함
		2. gap이 음수 : 판정선보다 밑에서 롱노트를 누름(MISS판정이 아님)
			_longDurTick -= (gap*-1);		절대값을 빼줌
		3. gap이 양수 : 판정선보다 위에서 롱노트를 누름(MISS판정이 아님)
			_longDurTick += gap;
			식을 같게 해주기 위해
			_longDurTick -= (gap*-1);
	*/

	int gap = (float)GameSystem::GetInstance()->GetPlayTimeTick() - _updateDuration;
	gap *= -1;
	_longDurTick -= gap;
	float lengthRate = (float)(_longDurTick) / (float)GameSystem::GetInstance()->GetPlayTimeTick();
	int lengthPerSec = (int)GameSystem::GetInstance()->GetTrackHeight() * lengthRate;
	_longSprite->SetLength(lengthPerSec);
}

bool Note::IsLive()
{
	return _isLive;
}

int Note::GetNoteTime()
{
	return _updateDuration;
}

int Note::GetDuration()
{
	return _longDurTick;
}

void Note::EnableReduceDuration()
{
	_isReduceDuration = true;
}

//Check longNote or tick is available
bool Note::IsLongNote()
{
	return (_longDurTick > 0);
}

bool Note::IsPass()
{
	return _isPass;
}

void Note::Pass()
{
	_isPass = true;
}

void Note::SetLive(bool isLive)
{
	_isLive = isLive;
}

void Note::SetXPosition(int x)
{
	_x = x;
}

int Note::GetBarNum()
{
	return _barNum;
}

void Note::Start(int playTimeTick)
{
	if (false == _isStart)
	{
		_updateDuration += playTimeTick;
		_isStart = true;
	}
}

void Note::PlayWav()
{
	if (NULL != _wavFile)
	{
		Mix_VolumeChunk(_wavFile, GameSystem::GetInstance()->GetMusicVolume() + 10);
		Mix_PlayChannel(-1, _wavFile, 0);
	}
}