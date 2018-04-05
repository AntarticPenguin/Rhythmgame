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

	//BMS파싱 및 노트 생성
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
			printf("현재 마디: %d\n", _curBarNum);
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
				BMS 마디 형식
				#XXXYY : XXX 마디 번호 / YY : Y(오토: 0, 직접플레이: 1, LNTYPE에서 롱노트 : 5, 그외: 일단 무시) Y(트랙번호)
			*/

			//1. 마디정보를 읽는다
			strncpy(barInfo, token, strlen(token));
			token = strtok(NULL, "#:\n");

			//2. 노트정보를 읽는다.
			strncpy(noteLine, token, strlen(token));
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

				AddNoteLine(trackNum, snoteLine);
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

void TrackManager::AddNoteLine(int trackNum, sNoteLine* noteLine)
{
	//6이상의 trackNum은 무시
	if((0 < trackNum) && (trackNum < 6))
		_trackNoteList[trackNum-1].push_back(noteLine);
}

void TrackManager::CreateGameNote()
{
	int judgeDeltaLine = 100;
	int noteTick = 0;
	int durationTick = 0;

	//BMSE Parsing Info
	float SecondPerBeat = 0;				// BPM에 따른 1비트당 초를 구한다.	(1/4박자 기준)

	for (int trackNum = 0; trackNum < _trackList->size(); trackNum++)
	{
		std::list<sNoteInfo> noteList;

		//정방향으로 순회하면서 각 노트의 시작시간 배치
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
			

			//noteTick = _SecondPerBar * sNoteLine->BarNum * 1000;	//마디가 시작하는 시간(초)
			noteTick = (_SecondPerBar * sNoteLine->BarNum * 1000) + 8500;	//마디가 시작하는 시간(초)

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
					noteList.push_back(noteInfo);
				}
			}
		}

		//역방향으로 순회하면서 롱노트 계산 및 노트를 트랙에 배치
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