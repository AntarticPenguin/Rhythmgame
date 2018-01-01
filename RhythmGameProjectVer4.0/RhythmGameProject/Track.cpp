#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GameSystem.h"
#include "DataManager.h"
#include "EffectPlayer.h"
#include "TrackManager.h"
#include "Track.h"
#include "Sprite.h"
#include "Note.h"

Track::Track(int xPos, int yPos)
{
	_judgeEffectSprite = NULL;

	_judge = eJudge::NONE;
	_keyType = eKeyType::NEUTRAL;

	_x = xPos;
	_y = yPos;
	_isPass = false;

	_curJudgeNote = NULL;

	_oldDuration = 0;
	_holdDuration = 0;
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
		printf("���Ͽ��� ����: %s\n", filePath);
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
	fclose(fp);

	Sprite* trackSprite = new Sprite(trackbackgroundSprite, true);
	_bgSpriteList.Append(trackSprite);

	Sprite* judgeSprite = new Sprite(judgelineSprite, true);
	_bgSpriteList.Append(judgeSprite);

	_judgeEffectSprite = new Sprite(explosionSprite, false);

	//JudgeLine Init
	_judgeDeltaLine = 100;
	_judgeTick = GameSystem::GetInstance()->GetPlayTimeTick();
	_judgeStartTick = _judgeTick - 200;
	_judgeEndTick = _judgeTick + 200;

	_judgePerfect_s = _judgeTick - 50;
	_judgePerfect_e = _judgeTick + 50;
	_judgeGreat_s = _judgePerfect_s - 50;
	_judgeGreat_e = _judgePerfect_e + 50;

	TrackPosition(_x, _y);
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

	if (NULL != _judgeEffectSprite)
	{
		delete _judgeEffectSprite;
		_judgeEffectSprite = NULL;
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

		//��Ʈ ����, ����
		//�������� �������� ���� fail üũ �ȵ� ��Ʈ
		if (_judgeEndTick < itr.Item()->GetNoteTime() && false == itr.Item()->IsPass())
		{
			itr.Item()->Pass();

			EffectPlayer::GetInstance()->Play(eEffect::eMISS);
			DataManager::GetInstance()->ResetCombo();

			_judge = eJudge::MISS;
			_isPass = true;

			itr.Item()->SetLive(false);
		}
	}

	_judgeEffectSprite->Update(deltaTime);
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

	_judgeEffectSprite->Render();
}

DLinkedList<Note*>& Track::GetNoteList()
{
	return _noteList;
}

void Track::TrackPosition(int x, int y)
{
	_x = x;
	_y = y;

	DLinkedListIterator<Sprite*> bgitr = _bgSpriteList.GetIterator();
	bgitr.Start();
	bgitr.Item()->SetPosition(_x, _y / 2);					//Ʈ�����
	bgitr.Forth();
	bgitr.Item()->SetPosition(_x, _y - _judgeDeltaLine);	//������

	_judgeEffectSprite->SetPosition(_x, _y - _judgeDeltaLine);
}

bool Track::IsPass()
{
	return _isPass;
}

void Track::AddNoteToTrack(float sec, float duration, int judgeDeltaLine)
{
	Note* note = new Note(sec, duration, judgeDeltaLine);
	note->SetXPosition(_x);
	note->Init();
	_noteList.Prepend(note);
}

void Track::KeyDown()
{
	switch (_keyType)
	{
	case eKeyType::NEUTRAL:
		_keyType = eKeyType::PRESS;
		break;
	case eKeyType::PRESS:
		_keyType = eKeyType::HOLDING;
		break;
	case eKeyType::HOLDING:
		_keyType = eKeyType::HOLDING;
		break;
	}

	//��Ʈ ����, ����
	if (_keyType == eKeyType::HOLDING)
	{
		KeyHold();
		return;
	}

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
			break;
		}

		if (_judgePerfect_s <= itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() <= _judgePerfect_e)
		{
			if (0 < itr.Item()->GetDuration())
			{
				_curJudgeNote = itr.Item();
				_curJudgeNote->EnableReduceDuration();
				_judge = eJudge::JUDGE_START_PERFECT;
			}
			else
			{
				_judge = eJudge::PERFECT;
				break;
			}
		}

		if ((_judgeGreat_s <= itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() < _judgePerfect_s) ||
			(_judgePerfect_e < itr.Item()->GetNoteTime() && itr.Item()->GetNoteTime() <= _judgeGreat_e) )
		{
			if (0 < itr.Item()->GetDuration())
			{
				_curJudgeNote = itr.Item();
				_curJudgeNote->EnableReduceDuration();
				_judge = eJudge::JUDGE_START_GREAT;
			}
			else
			{
				_judge = eJudge::GREAT;
				break;
			}
		}
	}

	switch (_judge)
	{
	case eJudge::PERFECT:
		_judgeEffectSprite->Play();
		EffectPlayer::GetInstance()->Play(eEffect::ePERFECT);
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->ScorePerfect();
		delete itr.Item();
		_noteList.Remove(itr);
		break;
	case eJudge::GREAT:
		_judgeEffectSprite->Play();
		EffectPlayer::GetInstance()->Play(eEffect::eGREAT);
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->ScoreGreat();
		delete itr.Item();
		_noteList.Remove(itr);
		break;
	case eJudge::MISS:
		EffectPlayer::GetInstance()->Play(eEffect::eMISS);
		DataManager::GetInstance()->ResetCombo();
		delete itr.Item();
		_noteList.Remove(itr);
		break;
	case eJudge::JUDGE_START_PERFECT:
		_judgeEffectSprite->SetLoop(true);
		_judgeEffectSprite->Play();
		EffectPlayer::GetInstance()->Play(eEffect::ePERFECT);
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->ScorePerfect();
		break;
	case eJudge::JUDGE_START_GREAT:
		_judgeEffectSprite->SetLoop(true);
		_judgeEffectSprite->Play();
		EffectPlayer::GetInstance()->Play(eEffect::eGREAT);
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->ScoreGreat();
		break;
	}
}

void Track::KeyHold()
{
	if (NULL != _curJudgeNote)
	{
		_holdDuration += _oldDuration - _curJudgeNote->GetDuration();
		if (_holdDuration < 0)
			_holdDuration = 0;

		if (_curJudgeNote->GetDuration() <= 0)
		{
			if (NULL != _curJudgeNote)
				_curJudgeNote->SetLive(false);
			_curJudgeNote = NULL;

			EffectPlayer::GetInstance()->Play(eEffect::eMISS);
			DataManager::GetInstance()->ResetCombo();
			_judge = eJudge::MISS;
			return;
		}
		else if (100 <= _holdDuration)
		{
			EffectPlayer::GetInstance()->Play(eEffect::ePERFECT);
			DataManager::GetInstance()->IncreaseCombo();
			DataManager::GetInstance()->ScorePerfect();
			_holdDuration = 0;
		}
		_oldDuration = _curJudgeNote->GetDuration();
	}
}

void Track::KeyUp()
{
	_judgeEffectSprite->SetLoop(false);
	switch (_judge)
	{
	case eJudge::JUDGE_START_PERFECT:
	case eJudge::JUDGE_START_GREAT:
		_judgeEffectSprite->Stop();
		
		if (55 < _curJudgeNote->GetDuration())
		{
			DataManager::GetInstance()->ResetCombo();
			_judge = eJudge::MISS;
			EffectPlayer::GetInstance()->Play(eEffect::eMISS);
		}
		else if (1 <=_curJudgeNote->GetDuration() && _curJudgeNote->GetDuration() <= 32)
		{
			DataManager::GetInstance()->IncreaseCombo();
			DataManager::GetInstance()->ScorePerfect();
			_judge = eJudge::JUDGE_START_PERFECT;
			EffectPlayer::GetInstance()->Play(eEffect::ePERFECT);
		}
		else if (32 < _curJudgeNote->GetDuration() && _curJudgeNote->GetDuration() <= 54)
		{
			DataManager::GetInstance()->IncreaseCombo();
			DataManager::GetInstance()->ScoreGreat();
			_judge = eJudge::JUDGE_START_GREAT;
			EffectPlayer::GetInstance()->Play(eEffect::eGREAT);
		}

		if (NULL != _curJudgeNote)
			_curJudgeNote->SetLive(false);
		_curJudgeNote = NULL;
		break;
	}
	_judge = eJudge::NONE;
	_keyType = eKeyType::NEUTRAL;
}