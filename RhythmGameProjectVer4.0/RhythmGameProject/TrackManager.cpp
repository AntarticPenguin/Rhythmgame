#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "GameSystem.h"
#include "DataManager.h"
#include "EffectPlayer.h"
#include "TrackManager.h"
#include "Track.h"
#include "Note.h"
#include "Sprite.h"
#include "Font.h"

TrackManager::TrackManager()
{
	_trackList = NULL;
	_trackNoteList = NULL;
	_combofont = NULL;
	_scorefont = NULL;
}

TrackManager::~TrackManager()
{
	Deinit();
}

void TrackManager::Init()
{
	_combofont = new Font("arialbd.ttf", 40);
	_combofont->SetPosition(GameSystem::GetInstance()->GetWindowWidth() - 250, 150);

	_scorefont = new Font("arialbd.ttf", 32);
	_scorefont->SetPosition(0, 0);
	char text[50];
	sprintf(text, "SCORE %08d", DataManager::GetInstance()->GetScore());
	_scorefont->SetText(text);
	
	{
		Track* track1 = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2) - 204, GameSystem::GetInstance()->GetWindowHeight());
		Track* track2 = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2) - 102, GameSystem::GetInstance()->GetWindowHeight());
		Track* track3 = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2), GameSystem::GetInstance()->GetWindowHeight());
		Track* track4 = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2) + 102, GameSystem::GetInstance()->GetWindowHeight());
		Track* track5 = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2) + 204, GameSystem::GetInstance()->GetWindowHeight());

		_trackList = new Array<Track*>(5);
		_trackList->Set(eTrackNum::TRACK01, track1);
		_trackList->Set(eTrackNum::TRACK02, track2);
		_trackList->Set(eTrackNum::TRACK03, track3);
		_trackList->Set(eTrackNum::TRACK04, track4);
		_trackList->Set(eTrackNum::TRACK05, track5);
	}

	{
		_trackNoteList = new DLinkedList<sNoteLine*>[_trackList->GetSize()];
	}

	//BMS�Ľ� �� ��Ʈ ����
	memset(_longNoteKey, 0, sizeof(_longNoteKey));
	//ParsingBMS("BMS_Sample.bme");
	//ParsingBMS("BMS_Sample2.bms");
	ParsingBMS("Only you_HDMix.bms");

	for (int i = 0; i < _trackList->GetSize(); i++)
		_trackList->Get(i)->Init();

	//CreateGameNote();
	CreateGameNote2();
}

void TrackManager::Deinit()
{
	if (NULL != _trackList)
	{
		for (int i = 0; i < _trackList->GetSize(); i++)
		{
			delete _trackList->Get(i);
		}
		_trackList = NULL;
	}
	
	if (NULL != _combofont)
	{
		delete _combofont;
		_combofont = NULL;
	}

	if (NULL != _scorefont)
	{
		delete _scorefont;
		_scorefont = NULL;
	}
}

void TrackManager::Update(int deltaTime)
{

	for (int i = 0; i < _trackList->GetSize(); i++)
		_trackList->Get(i)->Update(deltaTime);

	EffectPlayer::GetInstance()->Update(deltaTime);

	{
		char text[50];
		sprintf(text, "COMBO %d", DataManager::GetInstance()->GetCombo());
		_combofont->SetText(text);
	}
}

void TrackManager::Render()
{
	for (int i = 0; i < _trackList->GetSize(); i++)
		_trackList->Get(i)->Render();


	EffectPlayer::GetInstance()->Render();
	_combofont->Render();
	_scorefont->Render();
}

void TrackManager::ParsingBMS(const char* fileName)
{
	char filePath[256];
	sprintf(filePath, "../../Resource/%s", fileName);
	FILE* fp = fopen(filePath, "r");
	if (NULL == fp)
	{
		printf("���Ͽ��� ����: %s\n", filePath);
	}

	char buffer[1024];
	char barInfo[256];
	char noteLine[1024];
	char* record = fgets(buffer, sizeof(buffer), fp);

	int fieldFlag = 1;		//HEADER = 1, MAIN DATA = 2

	int curBarNum = 0;
	_BPM = 0;
	_SecondPerBar = 0;

	int count = 0;
	
	while (true)
	{
		count++;
		record = fgets(buffer, sizeof(buffer), fp);
		if (NULL == record)
			break;

		char* token = NULL;

		if (1 == fieldFlag)
			token = strtok(record, " #\n");
		else if (2 == fieldFlag)
		{
			token = strtok(record, "#:\n");
		}

		//HEADER FEILD PARSING
		while(fieldFlag == 1)
		{
			if (NULL == token)
				break;

			if (!strcmp(token, "PLAYER"))
			{
				token = strtok(NULL, "\n");
			}
			else if (!strcmp(token, "BPM"))
			{
				token = strtok(NULL, "\n");
				_BPM = atoi(token);
				_SecondPerBar = (60.0f * 32.0f) / (8.0f * _BPM);	// BPM�� ���� 1����� �ʸ� ���Ѵ�.(1/32���� ����)
			}
			else if (!strcmp(token, "PLAYLEVEL"))
			{
				token = strtok(NULL, "\n");
			}
			else if (!strcmp(token, "LNOBJ"))
			{
				token = strtok(NULL, "\n");
				strncpy(_longNoteKey, token, strlen(token));
			}
			else if (!strcmp(token, "LNTYPE"))
			{
				token = strtok(NULL, "\n");
				strncpy(_longNoteKey, "LNTYPE", strlen(_longNoteKey));
			}
			else if (!strcmp(token, "MAIN"))
			{
				fieldFlag = 2;
				record = fgets(buffer, sizeof(buffer), fp);
				token = strtok(record, "#:\n");
				break;
			}
			token = strtok(NULL, " #\n");
		}

		//MAIN DATA PARSING
		while (fieldFlag == 2)
		{
			memset(barInfo, 0, sizeof(barInfo));
			memset(noteLine, 0, sizeof(noteLine));

			if (NULL == token)
				break;
			/*
				BMS ���� ����
				#XXXYY : XXX ���� ��ȣ / YY : Y(����: 0, �����÷���: 1, LNTYPE���� �ճ�Ʈ : 5, �׿�: �ϴ� ����) Y(Ʈ����ȣ)
			*/

			//1. ���������� �д´�
			strncpy(barInfo, token, strlen(token));
			printf("bar: %s, ", barInfo);
			token = strtok(NULL, "#:\n");

			//2. ��Ʈ������ �д´�.
			strncpy(noteLine, token, strlen(token));
			printf(" %s\n", noteLine);
			token = strtok(NULL, "#:\n");

			//���� ��ȣ�� ����
			sscanf(barInfo, "%3d", &curBarNum);

			int playerPlay = barInfo[3] - '0';		//�����÷�������
			int trackNum = atoi(&barInfo[4]);		//��Ʈ�� �� Ʈ�� �ѹ�

			if (1 == playerPlay || 5 == playerPlay)
			{
				sNoteLine* snoteLine = new sNoteLine;
				strncpy(snoteLine->line, noteLine, sizeof(noteLine));
				snoteLine->BarNum = curBarNum;
				snoteLine->isLongNote = playerPlay;

				switch (trackNum)
				{
				case 1:
					_trackNoteList[0].Append(snoteLine);
					break;
				case 2:
					_trackNoteList[1].Append(snoteLine);
					break;
				case 3:
					_trackNoteList[2].Append(snoteLine);
					break;
				case 4:
					_trackNoteList[3].Append(snoteLine);
					break;
				case 5:
					_trackNoteList[4].Append(snoteLine);
					break;
				}
			}
		}
	}
	fclose(fp);

	//�뷡 �ð�, Ʈ�� ���� ����
	int tempo = _BPM / 60;
	float playTimeSec = _SecondPerBar * (curBarNum + 1);
	int trackheight = GameSystem::GetInstance()->GetWindowHeight() * playTimeSec * tempo;
	trackheight /= 2;
	GameSystem::GetInstance()->SetTrackHeight(trackheight);
	GameSystem::GetInstance()->SetPlayTimeTick(playTimeSec);
}

void TrackManager::CreateGameNote()
{
	int judgeDeltaLine = 100;
	int noteTick = 0;
	int durationTick = 0;

	//BMSE Parsing Info
	float SecondPerBeat = 0;				// BPM�� ���� 1��Ʈ�� �ʸ� ���Ѵ�.	(1/4���� ����)

	for (int trackNum = 0; trackNum < _trackList->GetSize(); trackNum++)
	{
		DLinkedList<sNoteInfo> noteList;

		//���������� ��ȸ�ϸ鼭 �� ��Ʈ�� ���۽ð� ��ġ
		DLinkedListIterator<sNoteLine*> itr = _trackNoteList[trackNum].GetIterator();
		for (itr.Start(); itr.Valid(); itr.Forth())
		{
			sNoteLine* sNoteLine = itr.Item();

			sNoteInfo noteInfo;
			noteInfo.startTick = 0;
			noteInfo.durationTick = 0;
			noteInfo.isLongNote = sNoteLine->isLongNote;

			noteTick = _SecondPerBar * sNoteLine->BarNum * 1000;	//���� �����ϴ� �ð�(��)

			int beat = strlen(sNoteLine->line) / 2;	//����
			SecondPerBeat = (60.0f / _BPM) / ((float)beat / 4);

			char* ptr = &sNoteLine->line[0];
			for (int i = 0; i < beat; i++)
			{
				memset(noteInfo.note, 0, sizeof(noteInfo.note));
				strncpy(noteInfo.note, ptr, 2);
				noteInfo.note[2] = '\0';
				(ptr++);
				(ptr++);

				//00�� ��Ʈ�� �������� �ʰ� �н�
				if (!strcmp(noteInfo.note, "00"))
				{
					noteTick += SecondPerBeat * 1000;
				}
				else
				{
					noteInfo.startTick = noteTick;
					noteTick += SecondPerBeat * 1000;
					noteList.Append(noteInfo);
				}
			}
		}
		
		//���������� ��ȸ�ϸ鼭 �ճ�Ʈ ��� �� ��Ʈ�� Ʈ���� ��ġ
		DLinkedListIterator<sNoteInfo> it = noteList.GetIterator();
		for (it.End(); it.Valid(); it.Back())
		{
			sNoteInfo curNote = it.Item();

			float sec = 0;
			float duration = 0;

			if (!strcmp(curNote.note, _longNoteKey))
			{
				it.Back();
				sNoteInfo prevNote = it.Item();
				sec = (float)(prevNote.startTick) / 1000.0f;
				duration = (float)(curNote.startTick - prevNote.startTick) / 1000.0f;
			}
			else
			{
				sec = (float)(curNote.startTick) / 1000.0f;
				duration = (float)(curNote.durationTick) / 1000.0f;
			}
			_trackList->Get(trackNum)->AddNoteToTrack(sec, duration, judgeDeltaLine);
		}
	}
}

void TrackManager::CreateGameNote2()
{
	int judgeDeltaLine = 100;
	int noteTick = 0;
	int durationTick = 0;

	//BMSE Parsing Info
	float SecondPerBeat = 0;				// BPM�� ���� 1��Ʈ�� �ʸ� ���Ѵ�.	(1/4���� ����)

	for (int trackNum = 0; trackNum < _trackList->GetSize(); trackNum++)
	{
		DLinkedList<sNoteInfo> noteList;

		//���������� ��ȸ�ϸ鼭 �� ��Ʈ�� ���۽ð� ��ġ
		DLinkedListIterator<sNoteLine*> itr = _trackNoteList[trackNum].GetIterator();
		for (itr.Start(); itr.Valid(); itr.Forth())
		{
			sNoteLine* sNoteLine = itr.Item();

			sNoteInfo noteInfo;
			noteInfo.startTick = 0;
			noteInfo.durationTick = 0;
			noteInfo.isLongNote = sNoteLine->isLongNote;

			noteTick = _SecondPerBar * sNoteLine->BarNum * 1000;	//���� �����ϴ� �ð�(��)

			int beat = strlen(sNoteLine->line) / 2;	//����
			SecondPerBeat = (60.0f / _BPM) / ((float)beat / 4);

			char* ptr = &sNoteLine->line[0];
			for (int i = 0; i < beat; i++)
			{
				memset(noteInfo.note, 0, sizeof(noteInfo.note));
				strncpy(noteInfo.note, ptr, 2);
				noteInfo.note[2] = '\0';
				(ptr++);
				(ptr++);

				//00�� ��Ʈ�� �������� �ʰ� �н�
				if (!strcmp(noteInfo.note, "00"))
				{
					noteTick += SecondPerBeat * 1000;
				}
				else
				{
					noteInfo.startTick = noteTick;
					noteTick += SecondPerBeat * 1000;
					noteList.Append(noteInfo);
				}
			}
		}

		//���������� ��ȸ�ϸ鼭 �ճ�Ʈ ��� �� ��Ʈ�� Ʈ���� ��ġ
		DLinkedListIterator<sNoteInfo> it = noteList.GetIterator();
		for (it.End(); it.Valid(); it.Back())
		{
			sNoteInfo curNote = it.Item();

			float sec = 0;
			float duration = 0;

			if (5 == curNote.isLongNote)
			{
				it.Back();
				sNoteInfo prevNote = it.Item();
				sec = (float)(prevNote.startTick) / 1000.0f;
				duration = (float)(curNote.startTick - prevNote.startTick) / 1000.0f;
			}
			else
			{
				sec = (float)(curNote.startTick) / 1000.0f;
				duration = (float)(curNote.durationTick) / 1000.0f;
			}
			_trackList->Get(trackNum)->AddNoteToTrack(sec, duration, judgeDeltaLine);
		}
	}
}

void TrackManager::KeyDown(int keyCode)
{
	switch (keyCode)
	{
	case SDLK_d:
		_trackList->Get(eTrackNum::TRACK01)->KeyDown();
		break;
	case SDLK_f:
		_trackList->Get(eTrackNum::TRACK02)->KeyDown();
		break;
	case SDLK_SPACE:
		_trackList->Get(eTrackNum::TRACK03)->KeyDown();
		break;
	case SDLK_j:
		_trackList->Get(eTrackNum::TRACK04)->KeyDown();
		break;
	case SDLK_k:
		_trackList->Get(eTrackNum::TRACK05)->KeyDown();
		break;
	}

	{
		char text[50];
		sprintf(text, "COMBO %d", DataManager::GetInstance()->GetCombo());
		_combofont->SetText(text);
	}

	{
		char text[50];
		sprintf(text, "SCORE %08d", DataManager::GetInstance()->GetScore());
		_scorefont->SetText(text);
	}
}

void TrackManager::KeyUp(int keyCode)
{
	switch (keyCode)
	{
	case SDLK_d:
		_trackList->Get(eTrackNum::TRACK01)->KeyUp();
		break;
	case SDLK_f:
		_trackList->Get(eTrackNum::TRACK02)->KeyUp();
		break;
	case SDLK_SPACE:
		_trackList->Get(eTrackNum::TRACK03)->KeyUp();
		break;
	case SDLK_j:
		_trackList->Get(eTrackNum::TRACK04)->KeyUp();
		break;
	case SDLK_k:
		_trackList->Get(eTrackNum::TRACK05)->KeyUp();
		break;
	}

	{
		char text[50];
		sprintf(text, "COMBO %d", DataManager::GetInstance()->GetCombo());
		_combofont->SetText(text);
	}

	{
		char text[50];
		sprintf(text, "SCORE %08d", DataManager::GetInstance()->GetScore());
		_scorefont->SetText(text);
	}
}