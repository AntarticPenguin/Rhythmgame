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

	//BMS파싱 및 노트 생성
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
		printf("파일열기 실패: %s\n", filePath);
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
				_SecondPerBar = (60.0f * 32.0f) / (8.0f * _BPM);	// BPM에 따른 1마디당 초를 구한다.(1/32박자 기준)
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
				BMS 마디 형식
				#XXXYY : XXX 마디 번호 / YY : Y(오토: 0, 직접플레이: 1, LNTYPE에서 롱노트 : 5, 그외: 일단 무시) Y(트랙번호)
			*/

			//1. 마디정보를 읽는다
			strncpy(barInfo, token, strlen(token));
			printf("bar: %s, ", barInfo);
			token = strtok(NULL, "#:\n");

			//2. 노트정보를 읽는다.
			strncpy(noteLine, token, strlen(token));
			printf(" %s\n", noteLine);
			token = strtok(NULL, "#:\n");

			//마디 번호를 저장
			sscanf(barInfo, "%3d", &curBarNum);

			int playerPlay = barInfo[3] - '0';		//오토플레이정보
			int trackNum = atoi(&barInfo[4]);		//노트가 들어갈 트랙 넘버

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

	//노래 시간, 트랙 길이 세팅
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
	float SecondPerBeat = 0;				// BPM에 따른 1비트당 초를 구한다.	(1/4박자 기준)

	for (int trackNum = 0; trackNum < _trackList->GetSize(); trackNum++)
	{
		DLinkedList<sNoteInfo> noteList;

		//정방향으로 순회하면서 각 노트의 시작시간 배치
		DLinkedListIterator<sNoteLine*> itr = _trackNoteList[trackNum].GetIterator();
		for (itr.Start(); itr.Valid(); itr.Forth())
		{
			sNoteLine* sNoteLine = itr.Item();

			sNoteInfo noteInfo;
			noteInfo.startTick = 0;
			noteInfo.durationTick = 0;
			noteInfo.isLongNote = sNoteLine->isLongNote;

			noteTick = _SecondPerBar * sNoteLine->BarNum * 1000;	//마디가 시작하는 시간(초)

			int beat = strlen(sNoteLine->line) / 2;	//박자
			SecondPerBeat = (60.0f / _BPM) / ((float)beat / 4);

			char* ptr = &sNoteLine->line[0];
			for (int i = 0; i < beat; i++)
			{
				memset(noteInfo.note, 0, sizeof(noteInfo.note));
				strncpy(noteInfo.note, ptr, 2);
				noteInfo.note[2] = '\0';
				(ptr++);
				(ptr++);

				//00은 노트를 삽입하지 않고 패스
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
		
		//역방향으로 순회하면서 롱노트 계산 및 노트를 트랙에 배치
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
	float SecondPerBeat = 0;				// BPM에 따른 1비트당 초를 구한다.	(1/4박자 기준)

	for (int trackNum = 0; trackNum < _trackList->GetSize(); trackNum++)
	{
		DLinkedList<sNoteInfo> noteList;

		//정방향으로 순회하면서 각 노트의 시작시간 배치
		DLinkedListIterator<sNoteLine*> itr = _trackNoteList[trackNum].GetIterator();
		for (itr.Start(); itr.Valid(); itr.Forth())
		{
			sNoteLine* sNoteLine = itr.Item();

			sNoteInfo noteInfo;
			noteInfo.startTick = 0;
			noteInfo.durationTick = 0;
			noteInfo.isLongNote = sNoteLine->isLongNote;

			noteTick = _SecondPerBar * sNoteLine->BarNum * 1000;	//마디가 시작하는 시간(초)

			int beat = strlen(sNoteLine->line) / 2;	//박자
			SecondPerBeat = (60.0f / _BPM) / ((float)beat / 4);

			char* ptr = &sNoteLine->line[0];
			for (int i = 0; i < beat; i++)
			{
				memset(noteInfo.note, 0, sizeof(noteInfo.note));
				strncpy(noteInfo.note, ptr, 2);
				noteInfo.note[2] = '\0';
				(ptr++);
				(ptr++);

				//00은 노트를 삽입하지 않고 패스
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

		//역방향으로 순회하면서 롱노트 계산 및 노트를 트랙에 배치
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