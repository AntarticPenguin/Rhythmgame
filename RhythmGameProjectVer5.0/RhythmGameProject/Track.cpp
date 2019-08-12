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
		printf("���Ͽ��� ����: %s\n", filePath);
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
		// 3�� ���� ������Ʈ
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

	_trackEffectSprite->SetPosition(_x, _y - 465);	//Ʈ�����
	_judgeEffectSprite->SetPosition(_x, _y - _judgeDeltaLine);	//��������Ʈ
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

	//�������� ���� ��Ʈ
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
			//��Ʈ�� ���� ���ۼ� ���� �ִ°�?
			if ((*_curNote)->GetNoteTime() < _judgeStartTick)
				return;

			//��Ʈ�� ���������� ���Դ°�?
			_judge = CheckJudge((*_curNote)->GetNoteTime());
			Judge(_judge);

			//�ճ�Ʈ�ΰ�?
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
		
		//�ճ�Ʈ ����
		if (_bJudgeLongNote)
		{
			/*	�ճ�Ʈ�� 1��Ʈ�� 1�޺�, �� 1��Ʈ�� �ð��� ���� �ճ�Ʈ�� �ð���ŭ �����ָ� �� �޺��� ���´�.
				60 BPM���� 5��¥�� �ճ�Ʈ = 5�޺�
				ex) 60 BPM	= 1�д� 60��Ʈ
							= 1�ʴ� 1��Ʈ
							1��Ʈ�� 1��
				ex) 240 BMP = 1�д� 240��Ʈ
							= 1�ʴ� 4��Ʈ
							1��Ʈ�� 0.25��
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

		if (_bJudgeLongNote)		//�ճ�Ʈ �������̾��� ��, Ű�� ������
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