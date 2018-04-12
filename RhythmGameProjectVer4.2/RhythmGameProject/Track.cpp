#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "GameSystem.h"
#include "DataManager.h"
#include "EffectPlayer.h"
#include "Track.h"
#include "Sprite.h"
#include "Note.h"

#include "InputSystem.h"

Track::Track(int xPos, int yPos) : _x(xPos), _y(yPos)
{
	_trackEffectSprite = NULL;
	_judgeEffectSprite = NULL;

	_judge = eJudge::NONE;

	_isJudging = false;

	_curBarNum = 0;
	_playTimeTick = 0;
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

	char trackEffectSprite[256];
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
			if (!strcmp(token, "TrackEffect"))
			{
				token = strtok(NULL, ",\n");
				strcpy(trackEffectSprite, token);
			}
			else if (!strcmp(token, "Explosion"))
			{
				token = strtok(NULL, ",\n");
				strcpy(explosionSprite, token);
			}
		}
	}
	fclose(fp);

	_trackEffectSprite = new Sprite(trackEffectSprite, false);
	_judgeEffectSprite = new Sprite(explosionSprite, false);

	//JudgeLine Init
	_judgeDeltaLine = 100;
	_judgeTick = GameSystem::GetInstance()->GetPlayTimeTick();
	_judgeStartTick = _judgeTick - 200;
	_judgeEndTick = _judgeTick + 200;

	_judgePerfect_s = _judgeTick - 60;
	_judgePerfect_e = _judgeTick + 60;
	_judgeGreat_s = _judgePerfect_s - 60;
	_judgeGreat_e = _judgePerfect_e + 60;

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

	if (NULL != _trackEffectSprite)
	{
		delete _trackEffectSprite;
		_trackEffectSprite = NULL;
	}

	if (NULL != _judgeEffectSprite)
	{
		delete _judgeEffectSprite;
		_judgeEffectSprite = NULL;
	}
}

void Track::Update(int deltaTime)
{
	_trackEffectSprite->Update(deltaTime);
	UpdateNoteList(deltaTime);
	_judgeEffectSprite->Update(deltaTime);

	UpdateInput();
}

void Track::Render()
{
	_trackEffectSprite->Render();

	std::list<Note*>::iterator itr;
	for(itr = _noteList.begin(); itr != _noteList.end(); itr++)
	{
		(*itr)->Render();
	}

	_judgeEffectSprite->Render();
}

void Track::LastInit()
{
	_curNote = _noteList.begin();
}

void Track::UpdateNoteList(int deltaTime)
{
	std::list<Note*>::iterator itr;
	for (itr = _noteList.begin(); itr != _noteList.end(); itr++)
	{
		// 3개 마디씩 업데이트
		if (_curBarNum <= (*itr)->GetBarNum() && (*itr)->GetBarNum() <= _curBarNum + 2)
		{
			(*itr)->Start(_playTimeTick);
		}

		(*itr)->Update(deltaTime);
	}
}

std::list<Note*>& Track::GetNoteList()
{
	return _noteList;
}

void Track::TrackPosition(int x, int y)
{
	_x = x;
	_y = y;

	_trackEffectSprite->SetPosition(_x, _y - 465);	//트랙배경
	_judgeEffectSprite->SetPosition(_x, _y - _judgeDeltaLine);	//판정이펙트
}

void Track::SetTrackNumber(int trackNumber)
{
	_trackNumber = trackNumber;
}

void Track::AddNoteToTrack(float sec, float duration, int judgeDeltaLine, int barNum)
{
	Note* note = new Note(sec, duration, judgeDeltaLine, barNum);
	note->SetXPosition(_x);
	note->Init();
	_noteList.push_front(note);
}

void Track::SetPlayBarInfo(int barNum, int playTimeTick)
{
	_curBarNum = barNum;
	_playTimeTick = playTimeTick;
}

void Track::UpdateInput()
{
	if (InputSystem::GetInstance()->IsKeyDown(_trackNumber))
	{
		_trackEffectSprite->Play();

		if (_curNote == _noteList.end())
			return;

		if ((*_curNote)->IsLive())
		{
			//노트가 판정 시작선 위에 있는가?
			if ((*_curNote)->GetNoteTime() < _judgeStartTick)
				return;

			//노트가 판정범위로 들어왔는가?
			_judge = CheckJudge((*_curNote)->GetNoteTime());
			Judge(_judge);

			//롱노트인가?
			if (0 < (*_curNote)->GetDuration())
			{
				(*_curNote)->SetLive(true);
				(*_curNote)->EnableReduceDuration();
				(*_curNote)->AdjustmentLength();
				_isJudging = true;
				return;
			}

			(*_curNote)->SetLive(false);
			_curNote++;
		}
	}
	else if (InputSystem::GetInstance()->IsKeyHold(_trackNumber))
	{
		_trackEffectSprite->Play();

		if (_curNote == _noteList.end())
			return;

		if (_isJudging)
		{
			if ((*_curNote)->GetDuration() <= -80)
			{
				_judge = eJudge::MISS;
				Judge(_judge);
				_isJudging = false;

				(*_curNote)->SetLive(false);
				_curNote++;
				return;
			}
			else
			{
				Judge(eJudge::PERFECT);
			}
		}
	}
	else if (InputSystem::GetInstance()->IsKeyUp(_trackNumber))
	{
		_trackEffectSprite->Stop();

		if (_curNote == _noteList.end())
			return;

		if (_isJudging)
		{
			if (109 < (*_curNote)->GetDuration())
			{
				_judge = eJudge::MISS;
				Judge(_judge);
			}
			else if (50 < (*_curNote)->GetDuration() && (*_curNote)->GetDuration() <= 109)
			{
				_judge = eJudge::GREAT;
				Judge(_judge);
			}
			else if (1 <= (*_curNote)->GetDuration() && (*_curNote)->GetDuration() <= 50)
			{
				_judge = eJudge::PERFECT;
				Judge(_judge);
			}
			_isJudging = false;
			(*_curNote)->SetLive(false);
			_curNote++;
		}
	}
	else
	{
		if (_curNote == _noteList.end())
			return;

		//판정선을 지난 노트
		if (_judgeEndTick < (*_curNote)->GetNoteTime() && false == (*_curNote)->IsPass())
		{
			(*_curNote)->Pass();
			_judge = eJudge::MISS;
			Judge(eJudge::MISS);

			(*_curNote)->SetLive(false);
			_curNote++;
		}
	}
}

eJudge Track::CheckJudge(int noteTime)
{
	if ((_judgeStartTick <= noteTime && noteTime < _judgeGreat_s) ||
		(_judgeGreat_e < noteTime && noteTime <= _judgeEndTick))
		return eJudge::MISS;

	if (_judgePerfect_s <= noteTime && noteTime <= _judgePerfect_e)
	{
		_judgeEffectSprite->Play();
		return eJudge::PERFECT;
	}

	if ((_judgeGreat_s <= noteTime && noteTime < _judgePerfect_s) ||
		(_judgePerfect_e < noteTime && noteTime <= _judgeGreat_e))
	{
		_judgeEffectSprite->Play();
		return eJudge::GREAT;
	}
	
	return eJudge::NONE;
}

void Track::Judge(eJudge judge)
{
	switch (judge)
	{
	case eJudge::PERFECT:
	case eJudge::GREAT:
		DataManager::GetInstance()->IncreaseCombo();
		DataManager::GetInstance()->IncreaseScore(judge);
		break;
	case eJudge::MISS:
		DataManager::GetInstance()->ResetCombo();
		break;
	case eJudge::NONE:
		return;
	}
	EffectPlayer::GetInstance()->Play(judge);
}