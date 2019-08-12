#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL_mixer.h"
#include "GameSystem.h"
#include "DataManager.h"
#include "EffectPlayer.h"
#include "Track.h"
#include "Sprite.h"
#include "Note.h"

#include "Input.h"

Track::Track(int xPos, int yPos) : _x(xPos), _y(yPos)
{
	_trackEffectSprite = NULL;
	_judgeEffectSprite = NULL;

	_judge = eJudge::NONE;

	_bJudgeLongNote = false;
	_longComboTick = 0;

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
	_judgeStartTick = _judgeTick - 100;
	_judgeEndTick = _judgeTick + 100;

	_judgePerfect_s = _judgeTick - 60;
	_judgePerfect_e = _judgeTick + 60;
	_judgeGreat_s = _judgePerfect_s - 60;
	_judgeGreat_e = _judgePerfect_e + 60;

	_bHold = false;

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
	_judgeEffectSprite->Update(deltaTime);
	UpdateNoteList(deltaTime);
	UpdateInput(deltaTime);
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

void Track::TrackPosition(int x, int y)
{
	_x = x;
	_y = y;

	_trackEffectSprite->SetPosition(_x, _y - 465);	//트랙배경
	_judgeEffectSprite->SetPosition(_x, _y - _judgeDeltaLine);	//판정이펙트
}

void Track::SetGameKey(eGameKey gameKey)
{
	_gameKey = gameKey;
}

void Track::AddNoteToTrack(float sec, float duration, int judgeDeltaLine, int barNum, Mix_Chunk* wavFile)
{
	Note* note = new Note(sec, duration, judgeDeltaLine, barNum, wavFile);
	note->SetXPosition(_x);
	note->Init();
	_noteList.push_front(note);
}

void Track::SetPlayBarInfo(int barNum, int playTimeTick)
{
	_curBarNum = barNum;
	_playTimeTick = playTimeTick;
}

void Track::UpdateInput(int deltaTime)
{
	if (_curNote == _noteList.end())
		return;

	//판정선을 지난 노트
	if (_judgeEndTick < (*_curNote)->GetNoteTime() && false == (*_curNote)->IsPass())
	{
		(*_curNote)->Pass();
		_judge = eJudge::MISS;
		Judge(eJudge::MISS);

		SetNextNote();
	}

	if(false == _bHold && Input::GetInstance().IsPressed(_gameKey))
	{
		_bHold = true;
		_trackEffectSprite->Play();

		if ((*_curNote)->IsLive())
		{
			//노트가 판정 시작선 위에 있는가?
			if ((*_curNote)->GetNoteTime() < _judgeStartTick)
				return;

			//노트가 판정범위로 들어왔는가?
			_judge = CheckJudge((*_curNote)->GetNoteTime());
			Judge(_judge);

			//롱노트인가?
			if((*_curNote)->IsLongNote())
			{
				(*_curNote)->SetLive(true);
				(*_curNote)->EnableReduceDuration();
				(*_curNote)->AdjustmentLength();
				_bJudgeLongNote = true;
				return;
			}

			SetNextNote();
		}
	}
	else if (true == _bHold && Input::GetInstance().IsPressed(_gameKey))		//key hold checking
	{
		_trackEffectSprite->Play();
		
		//롱노트 판정
		if (_bJudgeLongNote)
		{
			/*	롱노트는 1비트당 1콤보, 즉 1비트의 시간을 구해 롱노트의 시간만큼 나눠주면 총 콤보가 나온다.
				60 BPM에서 5초짜리 롱노트 = 5콤보
				ex) 60 BPM	= 1분당 60비트
							= 1초당 1비트
							1비트는 1초
				ex) 240 BMP = 1분당 240비트
							= 1초당 4비트
							1비트는 0.25초
			*/

			if (false == (*_curNote)->IsLongNote())
			{
				_bJudgeLongNote = false;
				SetNextNote();
				return;
			}

			_longComboTick += deltaTime;
			if (_longComboTick >= DataManager::GetInstance()->GetTickPerBeat())
			{
				Judge(eJudge::PERFECT);
				_longComboTick = 0;
			}
		}
	}

	if (Input::GetInstance().IsReleased(_gameKey))
	{
		_trackEffectSprite->Stop();

		if (_bJudgeLongNote)		//롱노트 판정중이었을 때, 키를 릴리즈
		{
			Judge(eJudge::MISS);
			_bJudgeLongNote = false;
			_longComboTick = 0;

			SetNextNote();
		}
		_bHold = false;
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
		(*_curNote)->PlayWav();
		break;
	case eJudge::MISS:
		DataManager::GetInstance()->ResetCombo();
		break;
	case eJudge::NONE:
		return;
	}
	EffectPlayer::GetInstance()->Play(judge);
}

void Track::SetNextNote()
{
	(*_curNote)->SetLive(false);
	_curNote++;
}