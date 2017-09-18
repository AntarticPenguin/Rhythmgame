#include "GameSystem.h"
#include "Note.h"
#include "Sprite.h"

Note::Note(float startTime, int judgeDeltaLine)
{
	_sprite = NULL;
	_startTick = GameSystem::GetInstance()->GetPlayTimeTick() - (int)(startTime * 1000.0f);
	_isLive = true;
	_judgeDeltaLine = judgeDeltaLine;
	_isPass = false;
}

Note::~Note()
{
	Deinit();
}

void Note::Init()
{
	_sprite = new Sprite("spriteData.csv", true);
	
	_updateDuration = _startTick;
	UpdatePosition(0);
	_sprite->SetPosition(_x, _y);
}

void Note::Deinit()
{
	if (NULL != _sprite)
	{
		delete _sprite;
		_sprite = NULL;
	}
}

void Note::Update(int deltaTime)
{
	//시간 기반
	_sprite->Update(deltaTime);
	UpdatePosition(deltaTime);
	_sprite->SetPosition(_x, _y);
}

void Note::Render()
{
	_sprite->Render();
}

void Note::UpdatePosition(int deltaTime)
{
	_updateDuration += deltaTime;					//노트 업데이트 시간
	if (_updateDuration <= GameSystem::GetInstance()->GetPlayTimeTick() + 200)
	{
		float positionRate = (float)_updateDuration / (float)GameSystem::GetInstance()->GetPlayTimeTick();;	//위치 비율
		float positionInSec = GameSystem::GetInstance()->GetTrackHeight() * positionRate;					//트랙길이에 비율을 곱한 위치
		_y = positionInSec - (GameSystem::GetInstance()->GetTrackHeight() - GameSystem::GetInstance()->GetWindowHeight() + _judgeDeltaLine);
	}
	else
	{
		_isLive = false;
	}
}

bool Note::IsLive()
{
	return _isLive;
}

int Note::GetNoteTime()
{
	return _updateDuration;
}

bool Note::IsPass()
{
	return _isPass;
}

void Note::Pass()
{
	_isPass = true;
}

void Note::SetXPosition(int x)
{
	_x = x;
}