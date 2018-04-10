#include "GameSystem.h"
#include "Note.h"
#include "Sprite.h"

Note::Note(float startTime, float duration, int judgeDeltaLine, int barNum)
{
	_sprite = NULL;
	_longSprite = NULL;
	_startTick = GameSystem::GetInstance()->GetPlayTimeTick() - (int)(startTime * 1000.0f);
	_longDurTick = (int)(duration * 1000.0f);
	_barNum = barNum;

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
		float trackHeight = (float)GameSystem::GetInstance()->GetTrackHeight();
		float playTime = (float)GameSystem::GetInstance()->GetPlayTimeTick();
		float lengthRate = (float)_longDurTick / playTime;
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
		if (_longDurTick < 0)
		{
			_longDurTick = 0;
			_isReduceDuration = false;
		}

		float trackHeight = (float)GameSystem::GetInstance()->GetTrackHeight();
		float playTime = (float)GameSystem::GetInstance()->GetPlayTimeTick();
		float lengthRate = (float)_longDurTick / playTime;
		int lengthPerSec = (int)GameSystem::GetInstance()->GetTrackHeight() * lengthRate;
		_longSprite->SetLength(lengthPerSec);

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