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
	fclose(fp);

	Sprite* trackSprite = new Sprite(trackbackgroundSprite, true);
	_bgSpriteList.push_back(trackSprite);

	Sprite* judgeSprite = new Sprite(judgelineSprite, true);
	_bgSpriteList.push_back(judgeSprite);

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
	std::list<Note*>::iterator itr;
	for(itr = _noteList.begin(); itr != _noteList.end();)
	{
		(*itr)->Deinit();
		_noteList.erase(itr++);
	}

	std::list<Sprite*>::iterator bgitr;
	for(bgitr = _bgSpriteList.begin(); bgitr != _bgSpriteList.end(); bgitr++)
	{
		if (NULL != (*bgitr))
			bgitr = _bgSpriteList.erase(bgitr);
	}

	if (NULL != _judgeEffectSprite)
	{
		delete _judgeEffectSprite;
		_judgeEffectSprite = NULL;
	}
}

void Track::Update(int deltaTime)
{
	std::list<Sprite*>::iterator bgitr;
	for (bgitr = _bgSpriteList.begin(); bgitr != _bgSpriteList.end(); bgitr++)
	{
		(*bgitr)->Update(deltaTime);
	}

	std::list<Note*>::iterator itr;
	for (itr = _noteList.begin(); itr != _noteList.end(); itr++)
	{
		// 4개마디씩 업데이트
		if (_curBarNum <= (*itr)->GetBarNum() && (*itr)->GetBarNum() <= _curBarNum + 3)
		{
			(*itr)->Start();
		}

		(*itr)->Update(deltaTime);

		//노트 판정: 판정선을 지났지만 아직 fail 체크 안된 노트
		if (_judgeEndTick < (*itr)->GetNoteTime() && false == (*itr)->IsPass())
		{
			(*itr)->Pass();

			EffectPlayer::GetInstance()->Play(eEffect::eMISS);
			DataManager::GetInstance()->ResetCombo();

			_judge = eJudge::MISS;
			_isPass = true;

			(*itr)->SetLive(false);
		}
	}

	_judgeEffectSprite->Update(deltaTime);
}

void Track::Render()
{
	std::list<Sprite*>::iterator bgitr;
	for(bgitr = _bgSpriteList.begin(); bgitr != _bgSpriteList.end(); bgitr++)
	{
		(*bgitr)->Render();
	}

	std::list<Note*>::iterator itr;
	for(itr = _noteList.begin(); itr != _noteList.end(); itr++)
	{
		(*itr)->Render();
	}

	_judgeEffectSprite->Render();
}

std::list<Note*>& Track::GetNoteList()
{
	return _noteList;
}

void Track::TrackPosition(int x, int y)
{
	_x = x;
	_y = y;

	std::list<Sprite*>::iterator bgitr = _bgSpriteList.begin();
	(*bgitr)->SetPosition(_x, _y / 2);					//트랙배경
	bgitr++;
	(*bgitr)->SetPosition(_x, _y - _judgeDeltaLine);	//판정선

	_judgeEffectSprite->SetPosition(_x, _y - _judgeDeltaLine);
}

bool Track::IsPass()
{
	return _isPass;
}

void Track::AddNoteToTrack(float sec, float duration, int judgeDeltaLine, int barNum)
{
	Note* note = new Note(sec, duration, judgeDeltaLine, barNum);
	note->SetXPosition(_x);
	note->Init();
	_noteList.push_front(note);
}

void Track::SetCurrentBar(int barNum)
{
	_curBarNum = barNum;
}

void Track::JudgeProcess(Note* note, eJudge judge)
{
	switch (_judge)
	{
	case eJudge::PERFECT:
		_judgeEffectSprite->Play();
		EffectPlayer::GetInstance()->Play(eEffect::ePERFECT);
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->ScorePerfect();
		note->SetLive(false);
		break;
	case eJudge::GREAT:
		_judgeEffectSprite->Play();
		EffectPlayer::GetInstance()->Play(eEffect::eGREAT);
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->ScoreGreat();
		note->SetLive(false);
		break;
	case eJudge::MISS:
		EffectPlayer::GetInstance()->Play(eEffect::eMISS);
		DataManager::GetInstance()->ResetCombo();
		note->SetLive(false);
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

	//노트 판정, 범위
	if (_keyType == eKeyType::HOLDING)
	{
		KeyHold();
		return;
	}

	std::list<Note*>::iterator itr;
	for(itr = _noteList.begin(); itr != _noteList.end(); itr++)
	{
		if ((*itr)->IsLive())
		{
			//노트가 판정 시작선 위에 있는가?
			if ((*itr)->GetNoteTime() < _judgeStartTick)
			{
				break;
			}

			//노트가 판정범위로 들어왔는가?
			if ((_judgeStartTick <= (*itr)->GetNoteTime() && (*itr)->GetNoteTime() < _judgeGreat_s) ||
				(_judgeGreat_e < (*itr)->GetNoteTime() && (*itr)->GetNoteTime() <= _judgeEndTick))
			{
				_judge = eJudge::MISS;
				break;
			}

			if (_judgePerfect_s <= (*itr)->GetNoteTime() && (*itr)->GetNoteTime() <= _judgePerfect_e)
			{
				if (0 < (*itr)->GetDuration())
				{
					_curJudgeNote = (*itr);
					_curJudgeNote->EnableReduceDuration();
					_judge = eJudge::JUDGE_START_PERFECT;
				}
				else
				{
					_judge = eJudge::PERFECT;
					break;
				}
			}

			if ((_judgeGreat_s <= (*itr)->GetNoteTime() && (*itr)->GetNoteTime() < _judgePerfect_s) ||
				(_judgePerfect_e < (*itr)->GetNoteTime() && (*itr)->GetNoteTime() <= _judgeGreat_e))
			{
				if (0 < (*itr)->GetDuration())
				{
					_curJudgeNote = (*itr);
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
	}

	JudgeProcess((*itr), _judge);
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