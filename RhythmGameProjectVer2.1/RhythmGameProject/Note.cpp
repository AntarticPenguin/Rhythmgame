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
	//�ð� ���
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
	_updateDuration += deltaTime;					//��Ʈ ������Ʈ �ð�
	if (_updateDuration <= GameSystem::GetInstance()->GetPlayTimeTick() + 200)
	{
		float positionRate = (float)_updateDuration / (float)GameSystem::GetInstance()->GetPlayTimeTick();;	//��ġ ����
		float positionInSec = GameSystem::GetInstance()->GetTrackHeight() * positionRate;					//Ʈ�����̿� ������ ���� ��ġ
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