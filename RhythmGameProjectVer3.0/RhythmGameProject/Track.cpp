#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "GameSystem.h"
#include "DataManager.h"
#include "TrackManager.h"
#include "Track.h"
#include "Sprite.h"
#include "Note.h"

Track::Track()
{
	_effectSprite = NULL;

	_isKeyDown = false;
	_judge = NONE;

	_x = 0;
	_y = 0;
	_isPass = false;
	_isHold = false;

	_curJudgeNote = NULL;

	_judgeDeltaLine = 100;
	_judgeTick = GameSystem::GetInstance()->GetPlayTimeTick();
	_judgeStartTick = _judgeTick - 200;
	_judgeEndTick = _judgeTick + 200;

	_judgePerfect_s = _judgeTick - 50;
	_judgePerfect_e = _judgeTick + 50;
	_judgeGreat_s = _judgePerfect_s - 50;
	_judgeGreat_e = _judgePerfect_e + 50;
}

Track::~Track()
{
	Deinit();
}

void Track::Init()
{
	char filePath[256];
	sprintf(filePath, "../../Resource/%s", "trackSetting4K.csv");
	FILE* fp = fopen(filePath, "r");
	if (NULL == fp)
	{
		printf("파일열기 실패: %s\n", filePath);
	}

	char trackbackgroundSprite[256];
	char judgelineSprite[256];
	char explosionSprite[256];

	char buffer[256];
	char* record = fgets(buffer, sizeof(buffer), fp);

	while (true)
	{
		record = fgets(buffer, sizeof(buffer), fp);
		if (NULL == record)
			break;

		{
			char* token = strtok(record, ",\n");
			if (!strcmp(token, "Background"))
			{
				token = strtok(NULL, ",\n");
				strcpy(trackbackgroundSprite, token);
			}
			else if (!strcmp(token, "Judgeline"))
			{
				token = strtok(NULL, ",\n");
				strcpy(judgelineSprite, token);
			}
			else if (!strcmp(token, "Explosion"))
			{
				token = strtok(NULL, ",\n");
				strcpy(explosionSprite, token);
			}
		}
	}

	Sprite* trackSprite = new Sprite(trackbackgroundSprite, true);
	_bgSpriteList.Append(trackSprite);

	Sprite* judgeSprite = new Sprite(judgelineSprite, true);
	_bgSpriteList.Append(judgeSprite);

	_effectSprite = new Sprite(explosionSprite, false);
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
			if (_curJudgeNote == itr.Item())
				_curJudgeNote = NULL;

			delete itr.Item();
			_noteList.Remove(itr);
			break;
		}

		//노트 판정, 범위
		//판정선을 지났지만 아직 fail 체크 안된 노트
		if (_judgeEndTick < itr.Item()->GetNoteTime() && false == itr.Item()->IsPass())
		{
			itr.Item()->Pass();
			_judge = eJudge::MISS;
			_isPass = true;

			itr.Item()->SetLive(false);
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
	bgitr.Item()->SetPosition(_x, _y / 2);					//트랙배경
	bgitr.Forth();
	bgitr.Item()->SetPosition(_x, _y - _judgeDeltaLine);	//판정선

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

bool Track::isHolding()
{
	return _isHold;
}


void Track::KeyDown()
{
	if (_isKeyDown)
		return;
	_isKeyDown = true;

	//노트 판정, 범위
	DLinkedListIterator<Note*> itr = _noteList.GetIterator();
	for (itr.Start(); itr.Valid(); itr.Forth())
	{
		//노트가 판정 시작선 위에 있는가?
		if (itr.Item()->GetNoteTime() < _judgeStartTick)
		{
			break;
		}

		//노트가 판정범위로 들어왔는가?
		if ( (_judgeStartTick <= itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() < _judgeGreat_s) ||
			(_judgeGreat_e < itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() <= _judgeEndTick) )
		{
			_judge = eJudge::MISS;
			_resultJudge = _judge;
			break;
		}

		if (_judgePerfect_s <= itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() <= _judgePerfect_e)
		{
			if (0 < itr.Item()->GetDuration())
			{
				_isHold = true;
				_curJudgeNote = itr.Item();
				_curJudgeNote->EnableReduceDuration();
				_judge = eJudge::JUDGE_START_PERFECT;
				_resultJudge = _judge;
			}
			else
			{
				_judge = eJudge::PERFECT;
				_resultJudge = _judge;
				break;
			}
		}

		if ((_judgeGreat_s <= itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() < _judgePerfect_s) ||
			(_judgePerfect_e < itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() <= _judgeGreat_e) )
		{
			if (0 < itr.Item()->GetDuration())
			{
				_isHold = true;
				_curJudgeNote = itr.Item();
				_curJudgeNote->EnableReduceDuration();
				_judge = eJudge::JUDGE_START_GREAT;
				_resultJudge = _judge;
			}
			else
			{
				_judge = eJudge::GREAT;
				_resultJudge = _judge;
				break;
			}
		}
	}

	switch (_judge)
	{
	case eJudge::PERFECT:
		_effectSprite->Play();
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->ScorePerfect();
		delete itr.Item();
		_noteList.Remove(itr);
		break;
	case eJudge::GREAT:
		_effectSprite->Play();
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->ScoreGreat();
		delete itr.Item();
		_noteList.Remove(itr);
		break;
	case eJudge::MISS:
		DataManager::GetInstance()->ResetCombo();
		delete itr.Item();
		_noteList.Remove(itr);
		break;
	case eJudge::JUDGE_START_PERFECT:
		_effectSprite->SetLoop(true);
		_effectSprite->Play();
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->ScorePerfect();
		break;
	case eJudge::JUDGE_START_GREAT:
		_effectSprite->SetLoop(true);
		_effectSprite->Play();
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->ScoreGreat();
		break;
	}
}

void Track::KeyUp()
{
	_effectSprite->SetLoop(false);
	switch (_judge)
	{
	case eJudge::JUDGE_START_PERFECT:
	case eJudge::JUDGE_START_GREAT:
		_effectSprite->Stop();

		if (55 < _curJudgeNote->GetDuration())
		{
			DataManager::GetInstance()->ResetCombo();
			_judge = eJudge::MISS;
			_resultJudge = _judge;
		}
		else if (1 <=_curJudgeNote->GetDuration() && _curJudgeNote->GetDuration() <= 25)
		{
			DataManager::GetInstance()->IncreaseCombo();
			DataManager::GetInstance()->ScorePerfect();
			_judge = eJudge::JUDGE_START_PERFECT;
			_resultJudge = _judge;
		}
		else if (25 <= _curJudgeNote->GetDuration() && _curJudgeNote->GetDuration() <= 54)
		{
			DataManager::GetInstance()->IncreaseCombo();
			DataManager::GetInstance()->ScoreGreat();
			_judge = eJudge::JUDGE_START_GREAT;
			_resultJudge = _judge;
		}

		if (NULL != _curJudgeNote)
			_curJudgeNote->SetLive(false);
		_curJudgeNote = NULL;
		break;
	}
	_judge = eJudge::NONE;
	_isKeyDown = false;
	_isHold = false;
}