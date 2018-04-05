#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "GameSystem.h"
#include "DataManager.h"
#include "EffectPlayer.h"
#include "TrackManager.h"
#include "InputKeyManager.h"
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
	_trackButton = InputKeyManager::GetInstance()->GetTrackButton();

	_combofont = new Font("arialbd.ttf", 40);
	_combofont->SetPosition(GameSystem::GetInstance()->GetWindowWidth() - 250, 150);

	_scorefont = new Font("arialbd.ttf", 32);
	_scorefont->SetPosition(0, 0);
	char text[50];
	sprintf(text, "SCORE %08d", DataManager::GetInstance()->GetScore());
	_scorefont->SetText(text);
	
	{
		_trackList = new std::vector<Track*>();

		int trackInterval = -204;
		for (int i = 0; i < 5; i++)
		{
			Track* track = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2) + trackInterval, GameSystem::GetInstance()->GetWindowHeight());
			_trackList->push_back(track);

			trackInterval += 102;
		}
	}

	{
		_trackNoteList = new std::list<sNoteLine*>[_trackList->size()];
	}

	//BMS�Ľ� �� ��Ʈ ����
	memset(_longNoteKey, 0, sizeof(_longNoteKey));
	ParsingBMS("BMS_Sample.bme");
	//ParsingBMS("Only you_HDMix.bms");

	for (int i = 0; i < _trackList->size(); i++)
		_trackList->at(i)->Init();

	CreateGameNote();

	_playTimeTick = 0;
	_curBarNum = 0;
}

void TrackManager::Deinit()
{
	if (NULL != _trackList)
	{
		for (int i = 0; i < _trackList->size(); i++)
		{
			delete _trackList->at(i);
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
	_playTimeTick += deltaTime;

	for (int i = 0; i < _trackList->size(); i++)
	{
		if ((_SecondPerBar * (_curBarNum + 1) * 1000) <= _playTimeTick)
		{
			_curBarNum++;
			printf("���� ����: %d\n", _curBarNum);
		}
		_trackList->at(i)->SetCurrentBar(_curBarNum);
	}

	for (int i = 0; i < _trackList->size(); i++)
		_trackList->at(i)->Update(deltaTime);

	EffectPlayer::GetInstance()->Update(deltaTime);
	{
		char text[50];
		sprintf(text, "COMBO %d", DataManager::GetInstance()->GetCombo());
		_combofont->SetText(text);
	}
}

void TrackManager::Render()
{
	for (int i = 0; i < _trackList->size(); i++)
		_trackList->at(i)->Render();

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
				_eFileType = eFileType::BME;
			}
			else if (!strcmp(token, "LNTYPE"))
			{
				token = strtok(NULL, "\n");
				strncpy(_longNoteKey, "LNTYPE", strlen(_longNoteKey));
				_eFileType = eFileType::BMS;
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
			token = strtok(NULL, "#:\n");

			//2. ��Ʈ������ �д´�.
			strncpy(noteLine, token, strlen(token));
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

				AddNoteLine(trackNum, snoteLine);
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

void TrackManager::AddNoteLine(int trackNum, sNoteLine* noteLine)
{
	//6�̻��� trackNum�� ����
	if((0 < trackNum) && (trackNum < 6))
		_trackNoteList[trackNum-1].push_back(noteLine);
}

void TrackManager::CreateGameNote()
{
	int judgeDeltaLine = 100;
	int noteTick = 0;
	int durationTick = 0;

	//BMSE Parsing Info
	float SecondPerBeat = 0;				// BPM�� ���� 1��Ʈ�� �ʸ� ���Ѵ�.	(1/4���� ����)

	for (int trackNum = 0; trackNum < _trackList->size(); trackNum++)
	{
		std::list<sNoteInfo> noteList;

		//���������� ��ȸ�ϸ鼭 �� ��Ʈ�� ���۽ð� ��ġ
		std::list<sNoteLine*>::iterator itr;
		for(itr = _trackNoteList[trackNum].begin(); itr != _trackNoteList[trackNum].end(); itr++)
		{
			sNoteLine* sNoteLine = (*itr);

			sNoteInfo noteInfo;
			noteInfo.startTick = 0;
			noteInfo.durationTick = 0;
			noteInfo.isLongNote = sNoteLine->isLongNote;

			{
				noteInfo.barNum = sNoteLine->BarNum;
			}
			

			//noteTick = _SecondPerBar * sNoteLine->BarNum * 1000;	//���� �����ϴ� �ð�(��)
			noteTick = (_SecondPerBar * sNoteLine->BarNum * 1000) + 8500;	//���� �����ϴ� �ð�(��)

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
					noteList.push_back(noteInfo);
				}
			}
		}

		//���������� ��ȸ�ϸ鼭 �ճ�Ʈ ��� �� ��Ʈ�� Ʈ���� ��ġ
		std::list<sNoteInfo>::iterator reverseItr;
		for(reverseItr = noteList.end(); reverseItr != noteList.begin();)
		{
			reverseItr--;
			sNoteInfo curNote = (*reverseItr);

			float sec = 0;
			float duration = 0;
			int barNum = 0;

			if(true == IsLongNote(_eFileType, curNote))
			{
				reverseItr--;
				sNoteInfo prevNote = (*reverseItr);
				sec = (float)(prevNote.startTick) / 1000.0f;
				duration = (float)(curNote.startTick - prevNote.startTick) / 1000.0f;
				barNum = prevNote.barNum;
			}
			else
			{
				sec = (float)(curNote.startTick) / 1000.0f;
				duration = (float)(curNote.durationTick) / 1000.0f;
				barNum = curNote.barNum;
			}
			//_trackList->at(trackNum)->AddNoteToTrack(sec, duration, judgeDeltaLine);
			_trackList->at(trackNum)->AddNoteToTrack(sec, duration, judgeDeltaLine, barNum);
		}
	}
}

bool TrackManager::IsLongNote(eFileType _eFileType, sNoteInfo curNote)
{
	switch (_eFileType)
	{
	case eFileType::BME:
		return (!strcmp(curNote.note, _longNoteKey));
	case eFileType::BMS:
		return (5 == curNote.isLongNote);
	}
	
	return false;
}

void TrackManager::KeyDown(int keyCode)
{
	switch (_trackButton[keyCode])
	{
	case eTrackButton::TRACK1:
		_trackList->at(eTrackNum::TRACK01)->KeyDown();
		break;
	case eTrackButton::TRACK2:
		_trackList->at(eTrackNum::TRACK02)->KeyDown();
		break;
	case eTrackButton::TRACK3:
		_trackList->at(eTrackNum::TRACK03)->KeyDown();
		break;
	case eTrackButton::TRACK4:
		_trackList->at(eTrackNum::TRACK04)->KeyDown();
		break;
	case eTrackButton::TRACK5:
		_trackList->at(eTrackNum::TRACK05)->KeyDown();
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
	switch (_trackButton[keyCode])
	{
	case eTrackButton::TRACK1:
		_trackList->at(eTrackNum::TRACK01)->KeyUp();
		break;
	case eTrackButton::TRACK2:
		_trackList->at(eTrackNum::TRACK02)->KeyUp();
		break;
	case eTrackButton::TRACK3:
		_trackList->at(eTrackNum::TRACK03)->KeyUp();
		break;
	case eTrackButton::TRACK4:
		_trackList->at(eTrackNum::TRACK04)->KeyUp();
		break;
	case eTrackButton::TRACK5:
		_trackList->at(eTrackNum::TRACK05)->KeyUp();
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