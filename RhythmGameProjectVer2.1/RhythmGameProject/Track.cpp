#include <stdio.h>
#include <stdlib.h>


#include "GameSystem.h"
#include "TrackManager.h"
#include "Track.h"
#include "Sprite.h"
#include "Note.h"

Track::Track()
{
	_effectSprite = NULL;

	_isKeyDown = false;
	_judge = NONE;

	_judgeDeltaLine = 100;
	_judgeTick = GameSystem::GetInstance()->GetPlayTimeTick();
	_judgeStartTick = _judgeTick - 200;
	_judgeEndTick = _judgeTick + 200;

	_judgePerfect_s = _judgeTick - 50;
	_judgePerfect_e = _judgeTick + 50;
	_judgeGreat_s = _judgePerfect_s - 50;
	_judgeGreat_e = _judgePerfect_e + 50;

	_x = 0;
	_y = 0;
	_isPass = false;
}

Track::~Track()
{
	Deinit();
}

void Track::Init()
{
	Sprite* trackSprite = new Sprite("trackData.csv", true);
	_bgSpriteList.Append(trackSprite);

	Sprite* judgeSprite = new Sprite("judgeData.csv", true);
	_bgSpriteList.Append(judgeSprite);

	_effectSprite = new Sprite("judgeeffectData.csv", false);
}

void Track::Deinit()
{

	DLinkedListIterator<Note*> itr = _noteList.GetIterator();
	for (itr.Start(); itr.Valid(); itr.Forth())
	{
		itr.Item()->Deinit();
		delete itr.Item();
		_noteList.Remove(itr);
	}

	DLinkedListIterator<Sprite*> bgitr = _bgSpriteList.GetIterator();
	for (bgitr.Start(); bgitr.Valid(); bgitr.Forth())
	{
		if (NULL != bgitr.Item())
		{
			delete bgitr.Item();
		}
		_bgSpriteList.Remove(bgitr);
	}

	if (NULL != _effectSprite)
	{
		delete _effectSprite;
		_effectSprite = NULL;
	}
}

void Track::Update(int deltaTime)
{
	DLinkedListIterator<Sprite*> bgitr = _bgSpriteList.GetIterator();
	for (bgitr.Start(); bgitr.Valid(); bgitr.Forth())
	{
		bgitr.Item()->Update(deltaTime);
	}

	DLinkedListIterator<Note*> itr = _noteList.GetIterator();
	for (itr.Start(); itr.Valid(); itr.Forth())
	{
		if (itr.Item()->IsLive())
		{
			itr.Item()->Update(deltaTime);
		}
		else
		{
			delete itr.Item();
			_noteList.Remove(itr);
		}

		//��Ʈ ����, ����
		//�������� �������� ���� fail üũ �ȵ� ��Ʈ
		if (_judgeEndTick < itr.Item()->GetNoteTime() && false == itr.Item()->IsPass())
		{
			itr.Item()->Pass();
			_judge = eJudge::MISS;
			_isPass = true;

			delete itr.Item();
			_noteList.Remove(itr);
		}
	}

	_effectSprite->Update(deltaTime);
}

void Track::Render()
{
	DLinkedListIterator<Sprite*> bgitr = _bgSpriteList.GetIterator();
	for (bgitr.Start(); bgitr.Valid(); bgitr.Forth())
	{
		bgitr.Item()->Render();
	}

	DLinkedListIterator<Note*> itr = _noteList.GetIterator();
	for (itr.Start(); itr.Valid(); itr.Forth())
	{
		itr.Item()->Render();
	}

	_effectSprite->Render();
}

DLinkedList<Note*>& Track::GetNoteList()
{
	return _noteList;
}

void Track::SetPosition(int x, int y)
{
	_x = x;
	_y = y;

	DLinkedListIterator<Sprite*> bgitr = _bgSpriteList.GetIterator();
	bgitr.Start();
	bgitr.Item()->SetPosition(_x, _y / 2);					//Ʈ�����
	bgitr.Forth();
	bgitr.Item()->SetPosition(_x, _y - _judgeDeltaLine);	//������

	_effectSprite->SetPosition(_x, _y - _judgeDeltaLine);
}

eJudge Track::GetResultJudge()
{
	return _resultJudge;
}

void Track::ResetResultJudge()
{
	_resultJudge = eJudge::NONE;
}

bool Track::IsPass()
{
	return _isPass;
}

void Track::resetPass()
{
	_isPass = false;
}


void Track::KeyDown()
{
	if (_isKeyDown)
		return;
	_isKeyDown = true;

	//��Ʈ ����, ����
	DLinkedListIterator<Note*> itr = _noteList.GetIterator();
	for (itr.Start(); itr.Valid(); itr.Forth())
	{
		//��Ʈ�� ���� ���ۼ� ���� �ִ°�?
		if (itr.Item()->GetNoteTime() < _judgeStartTick)
		{
			break;
		}

		//��Ʈ�� ���������� ���Դ°�?
		if ( (_judgeStartTick <= itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() < _judgeGreat_s) ||
			(_judgeGreat_e < itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() <= _judgeEndTick) )
		{
			_judge = eJudge::MISS;
			_resultJudge = _judge;
			break;
		}

		if (_judgePerfect_s <= itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() <= _judgePerfect_e)
		{
			_judge = eJudge::PERFECT;
			_resultJudge = _judge;
			break;
		}

		if ((_judgeGreat_s <= itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() < _judgePerfect_s) ||
			(_judgePerfect_e < itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() <= _judgeGreat_e) )
		{
			_judge = eJudge::GREAT;
			_resultJudge = _judge;
			break;
		}
	}

	switch (_judge)
	{
	case eJudge::PERFECT:
	case eJudge::GREAT:
		_effectSprite->Play();
	case eJudge::MISS:
		delete itr.Item();
		_noteList.Remove(itr);
		break;
	}

	_judge = NONE;
}

void Track::KeyUp()
{
	_isKeyDown = false;
}