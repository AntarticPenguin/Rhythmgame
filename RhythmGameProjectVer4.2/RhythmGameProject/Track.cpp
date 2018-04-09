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
	_bgSprite = NULL;
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

	char trackbackgroundSprite[256];
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
			else if (!strcmp(token, "Explosion"))
			{
				token = strtok(NULL, ",\n");
				strcpy(explosionSprite, token);
			}
		}
	}
	fclose(fp);

	_bgSprite = new Sprite(trackbackgroundSprite, true);
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

	if (NULL != _bgSprite)
	{
		delete _bgSprite;
		_bgSprite = NULL;
	}

	if (NULL != _judgeEffectSprite)
	{
		delete _judgeEffectSprite;
		_judgeEffectSprite = NULL;
	}
}

void Track::Update(int deltaTime)
{
	_bgSprite->Update(deltaTime);
	UpdateNoteList(deltaTime);
	_judgeEffectSprite->Update(deltaTime);

	UpdateInput();
}

void Track::Render()
{
	_bgSprite->Render();

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
		// 2개 마디씩 업데이트
		if (_curBarNum <= (*itr)->GetBarNum() && (*itr)->GetBarNum() <= _curBarNum + 1)
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

	_bgSprite->SetPosition(_x, _y / 2);	//트랙배경
	//(*bgitr)->SetPosition(_x, _y - _judgeDeltaLine);	//판정선

	_judgeEffectSprite->SetPosition(_x, _y - _judgeDeltaLine);
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
				_isJudging = true;
				return;
			}

			(*_curNote)->SetLive(false);
			_curNote++;
		}
	}
	else if (InputSystem::GetInstance()->IsKeyHold(_trackNumber))
	{
		if (_isJudging)
		{
			if ((*_curNote)->GetDuration() <= 0)
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
		if (_isJudging)
		{
			if (100 < (*_curNote)->GetDuration())
			{
				_judge = eJudge::MISS;
				Judge(_judge);
			}
			else if (50 < (*_curNote)->GetDuration() && (*_curNote)->GetDuration() <= 99)
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