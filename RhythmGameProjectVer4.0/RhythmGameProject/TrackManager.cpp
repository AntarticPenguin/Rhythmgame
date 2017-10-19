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
	char* noteInfo = new char[120];
	char* record = fgets(buffer, sizeof(buffer), fp);

	int fieldFlag = 1;		//HEADER = 1, MAIN DATA = 2

	int judgeDeltaLine = 100;
	int noteTick = 0;
	int deltaTick = 300;
	int durationTick = 0;

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
				printf("BPM :%s\n", token);
			}
			else if (!strcmp(token, "PLAYLEVEL"))
			{
				token = strtok(NULL, "\n");
				printf("PLAYLEVEL :%s\n", token);
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
			if (NULL == token)
				break;
			/*
				BPM에 따른 노트 시간 계산

				1. BPM = 1분 당 1/4박자의 개수
				예) 60BPM = 1분에 1/4박자가 60개 / 1/8박자 120개  / 1/16박자 240개  / 1/32박자 480개

				2. 초당 비트
					1/4박자 1초에 1비트  / 1/8박자 0.5초에 1비트  / 1/16박자 0.25초에 1비트
				
				3. 비트 하나당 간격
				130BPM =>  130bpm / 60s = 2.16666667(s)
				1/16박자 => 0.25 / 2.16666667 = 0.1153846초마다 나옴

				BMS 마디 형식
				#XXXYY : XXX 마디 번호 / YY : Y(오토: 0, 직접플레이: 1, 그외: 일단 무시) Y(트랙번호)
			*/

			//1. 마디정보를 읽는다
			strncpy(barInfo, token, strlen(token));
			token = strtok(NULL, "#:\n");

			//2. 노트정보를 읽는다.
			strncpy(noteInfo, token, strlen(token));
			token = strtok(NULL, "#:\n");

			printf("bar info: %s\n", barInfo);
			printf("note info: %s\n\n", noteInfo);

			int trackNum = atoi(&barInfo[4]);
			printf("trackNumber is %d\n", trackNum);

			noteTick += deltaTick;

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
		}
	}
	fclose(fp);
	//노트 생성
	/*int judgeDeltaLine = 100;
	int playTime = GameSystem::GetInstance()->GetPlayTimeTick();
	int deltaTick = 0;
	int durationTick = 0;
	for (int noteTick = 875; noteTick < playTime; )
	{
		int randValue = rand() % 4;
		switch (randValue)
		{
		case 0:
			deltaTick = 450;
			break;
		case 1:
			deltaTick = 600;
			break;
		case 2:
			deltaTick = 750;
			break;
		case 3:
			deltaTick = 1000;
			break;
		}

		noteTick += deltaTick;

		randValue = rand() % 5;
		switch (randValue)
		{
		case 0:
		case 1:
		case 2:
			durationTick = 0;
			break;
		case 3:
			durationTick = 1000;
			break;
		default:
			durationTick = 0;
		}

		float sec = (float)noteTick / 1000.0f;
		float duration = (float)durationTick / 1000.0f;

		switch(randValue)
		{
		case 0:
			_trackList->Get(eTrackNum::TRACK01)->AddNoteToTrack(sec, duration, judgeDeltaLine);
			break;
		case 1:
			_trackList->Get(eTrackNum::TRACK02)->AddNoteToTrack(sec, duration, judgeDeltaLine);
			break;
		case 2:
			_trackList->Get(eTrackNum::TRACK03)->AddNoteToTrack(sec, duration, judgeDeltaLine);
			break;
		case 3:
			_trackList->Get(eTrackNum::TRACK04)->AddNoteToTrack(sec, duration, judgeDeltaLine);
			break;
		case 4:
			_trackList->Get(eTrackNum::TRACK05)->AddNoteToTrack(sec, duration, judgeDeltaLine);
			break;
		}
		noteTick += durationTick;
	}*/
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