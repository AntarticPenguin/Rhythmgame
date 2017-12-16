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
		track1->Init();
		Track* track2 = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2) - 102, GameSystem::GetInstance()->GetWindowHeight());
		track2->Init();
		Track* track3 = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2), GameSystem::GetInstance()->GetWindowHeight());
		track3->Init();
		Track* track4 = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2) + 102, GameSystem::GetInstance()->GetWindowHeight());
		track4->Init();
		Track* track5 = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2) + 204, GameSystem::GetInstance()->GetWindowHeight());
		track5->Init();

		_trackList = new Array<Track*>(5);
		_trackList->Set(eTrackNum::TRACK01, track1);
		_trackList->Set(eTrackNum::TRACK02, track2);
		_trackList->Set(eTrackNum::TRACK03, track3);
		_trackList->Set(eTrackNum::TRACK04, track4);
		_trackList->Set(eTrackNum::TRACK05, track5);
	}

	//BMS파싱 및 노트 생성
	CreateGameNote("BMS_Sample.bme");
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

void TrackManager::CreateGameNote(const char* fileName)
{
	char filePath[256];
	sprintf(filePath, "../../Resource/%s", fileName);
	FILE* fp = fopen(filePath, "r");
	if (NULL == fp)
	{
		printf("파일열기 실패: %s\n", filePath);
	}

	char buffer[256];
	char* barInfo = new char[50];
	char* noteInfo = new char[1024];
	char* record = fgets(buffer, sizeof(buffer), fp);

	int fieldFlag = 1;		//HEADER = 1, MAIN DATA = 2
	
	int judgeDeltaLine = 100;
	int noteTick = 0;
	int durationTick = 0;

	//BMSE Parsing Info
	int BPM = 0;					// BMSE 스크립트를 통해 BPM을 구한다.
	float SecondPerBeat = 0;		// BPM에 따른 1비트당 초를 구한다.	(1/4박자 기준)
	float SecondPerBar = 0;			// BPM에 따른 1마디당 초를 구한다.	(1/32박자 기준)
	float curBarTick = 0;
	int curBarNum = 0;

	while (true)
	{
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
				printf("PLAYER :%s\n", token);
			}
			else if (!strcmp(token, "BPM"))
			{
				token = strtok(NULL, "\n");
				BPM = atoi(token);
				SecondPerBar = (60.0f * 32.0f) / (8.0f * BPM);	// BPM에 따른 1마디당 초를 구한다.(1/32박자 기준)
				printf("BPM: %d\n", BPM);
			}
			else if (!strcmp(token, "PLAYLEVEL"))
			{
				token = strtok(NULL, "\n");
				printf("PLAYLEVEL :%s\n", token);
			}
			else if (!strcmp(token, "LNOBJ"))
			{
				
			}
			else if (!strcmp(token, "LNTYPE"))
			{

			}
			else if (!strcmp(token, "MAIN"))
			{
				fieldFlag = 2;
				printf("MAIN DATA FIELD\n");
				record = fgets(buffer, sizeof(buffer), fp);
				token = strtok(record, "#:\n");
				break;
			}

			token = strtok(NULL, " #\n");
		}

		//MAIN DATA PARSING
		while (fieldFlag == 2)
		{
			memset(barInfo, 0, sizeof(char) * 50);
			memset(noteInfo, 0, sizeof(char) * 120);
			durationTick = 0;

			if (NULL == token)
				break;
			/*
				BMS 마디 형식
				#XXXYY : XXX 마디 번호 / YY : Y(오토: 0, 직접플레이: 1, 그외: 일단 무시) Y(트랙번호)
			*/

			//1. 마디정보를 읽는다
			strncpy(barInfo, token, strlen(token));
			token = strtok(NULL, "#:\n");

			//2. 노트정보를 읽는다.
			strncpy(noteInfo, token, strlen(token));
			token = strtok(NULL, "#:\n");

			//마디 번호를 저장
			sscanf(barInfo, "%3d", &curBarNum);

			int playerPlay = barInfo[3] - '0';		//오토플레이정보
			int trackNum = atoi(&barInfo[4]);		//노트가 들어갈 트랙 넘버

			if (1 == playerPlay)
			{
				noteTick = SecondPerBar * (curBarNum-1) * 1000;	//마디가 시작하는 시간(초)

				int beat = strlen(noteInfo) / 2;	//박자
				if (1 == beat)
				{
					durationTick = SecondPerBar * 1000;
				}
				else
				{
					SecondPerBeat = (60.0f / BPM) / (beat / 4);
				}
				
				char* ptr = &noteInfo[0];
				for (int i = 0; i < beat; i++)
				{
					char note[3] = { 0, };
					strncpy(note, ptr, 2);
					//printf("%s ", note);
					note[2] = '\0';
					(ptr++);
					(ptr++);

					//00은 노트를 삽입하지 않고 패스
					if (!strcmp(note, "00"))
					{
						noteTick += SecondPerBeat * 1000;
					}
					else
					{
						//노트배치
						float sec = (float)noteTick / 1000.0f;
						float duration = (float)durationTick / 1000.0f;
						switch (trackNum)
						{
						case 1:
							_trackList->Get(eTrackNum::TRACK01)->AddNoteToTrack(sec, duration, judgeDeltaLine);
							break;
						case 2:
							_trackList->Get(eTrackNum::TRACK02)->AddNoteToTrack(sec, duration, judgeDeltaLine);
							break;
						case 3:
							_trackList->Get(eTrackNum::TRACK03)->AddNoteToTrack(sec, duration, judgeDeltaLine);
							break;
						case 4:
							_trackList->Get(eTrackNum::TRACK04)->AddNoteToTrack(sec, duration, judgeDeltaLine);
							break;
						case 5:
							_trackList->Get(eTrackNum::TRACK05)->AddNoteToTrack(sec, duration, judgeDeltaLine);
							break;
						}
						noteTick += SecondPerBeat * 1000;
					}
				}
			}
		}
	}
	fclose(fp);
	delete[] barInfo;
	delete[] noteInfo;
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