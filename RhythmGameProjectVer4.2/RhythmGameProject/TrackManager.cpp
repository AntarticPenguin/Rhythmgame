#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL.h"
#include "SDL_mixer.h"
#include "GameSystem.h"
#include "DataManager.h"
#include "EffectPlayer.h"
#include "TrackManager.h"
#include "Track.h"
#include "Wav.h"
#include "Note.h"
#include "Sprite.h"
#include "Font.h"

TrackManager::TrackManager()
{
	_trackList = NULL;
	_trackNoteList = NULL;
	_judgeLineSprite = NULL;
	_combofont = NULL;
	_scorefont = NULL;
}

TrackManager::~TrackManager()
{
	Deinit();
}

void TrackManager::Init()
{
	_bgSprite = new Sprite("trackBackground.csv", true);
	_bgSprite->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2,
		GameSystem::GetInstance()->GetWindowHeight() / 2);

	_judgeLineSprite = new Sprite("judgeData.csv", true);
	_judgeLineSprite->SetPosition((GameSystem::GetInstance()->GetWindowWidth() / 2),
		GameSystem::GetInstance()->GetWindowHeight() - 100);

	_combofont = new Font("arialbd.ttf", 40);
	_combofont->SetPosition(GameSystem::GetInstance()->GetWindowWidth() - 250, 150);

	_scorefont = new Font("arialbd.ttf", 32);
	_scorefont->SetPosition(0, 0);
	char text[50];
	sprintf(text, "SCORE %08d", DataManager::GetInstance()->GetScore());
	_scorefont->SetText(text);
	
	{
		_trackList = new std::vector<Track*>();

		int trackInterval = -158;
		for (int i = 0; i < 5; i++)
		{
			Track* track = new Track((GameSystem::GetInstance()->GetWindowWidth() / 2) + trackInterval,
				GameSystem::GetInstance()->GetWindowHeight());
			_trackList->push_back(track);
			trackInterval += 79;
		}

		_trackNoteList = new std::list<sNoteLine*>[_trackList->size()];
	}

	//BMS�Ľ� �� ��Ʈ ����
	memset(_longNoteKey, 0, sizeof(_longNoteKey));
	ParsingBMS(DataManager::GetInstance()->GetMusicTitle().c_str());

	for (int i = 0; i < _trackList->size(); i++)
	{
		_trackList->at(i)->Init();
		_trackList->at(i)->SetTrackNumber(i);
	}

	CreateGameNote();
	
	for (int i = 0; i < _trackList->size(); i++)
	{
		_trackList->at(i)->LastInit();
	}

	_playTimeTick = 0;
	_curBarNum = 0;
}

void TrackManager::Deinit()
{
	_autoWavList.clear();
	_wavMap.clear();

	if (NULL != _trackList)
	{
		for (int i = 0; i < _trackList->size(); i++)
		{
			delete _trackList->at(i);
		}
		_trackList = NULL;
	}

	std::list<sNoteLine*>::iterator itr;
	for (itr = _trackNoteList->begin(); itr != _trackNoteList->end(); itr++)
	{
		delete (*itr);
	}
	_trackNoteList->clear();

	if (NULL != _bgSprite)
	{
		delete _bgSprite;
		_bgSprite = NULL;
	}
	
	if (NULL != _judgeLineSprite)
	{
		delete _judgeLineSprite;
		_judgeLineSprite = NULL;
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
	_bgSprite->Update(deltaTime);
	_judgeLineSprite->Update(deltaTime);
	EffectPlayer::GetInstance()->Update(deltaTime);

	_playTimeTick += deltaTime;

	//Track
	{
		for (int i = 0; i < _trackList->size(); i++)
		{
			_trackList->at(i)->SetPlayBarInfo(_curBarNum, _playTimeTick);
		}

		if ((_SecondPerBar * (_curBarNum) * 1000) <= _playTimeTick)
			_curBarNum++;

		for (int i = 0; i < _trackList->size(); i++)
			_trackList->at(i)->Update(deltaTime);
	}

	std::list<Wav*>::iterator itr;
	for (itr = _autoWavList.begin(); itr != _autoWavList.end(); itr++)
		(*itr)->Update(deltaTime);

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

void TrackManager::Render()
{
	_bgSprite->Render();
	_judgeLineSprite->Render();
	EffectPlayer::GetInstance()->Render();

	for (int i = 0; i < _trackList->size(); i++)
		_trackList->at(i)->Render();

	_combofont->Render();
	_scorefont->Render();
}

void TrackManager::ParsingBMS(const char* fileName)
{
	char filePath[256];
	sprintf(filePath, "../../Resource/music/%s", fileName);
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
			else if (NULL != strstr(token, "WAV"))
			{
				std::string str = token;
				std::string code = str.substr(3, 4);

				token = strtok(NULL, "\n");
				std::string wavName = token;
				
				Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
				Mix_AllocateChannels(256);

				char wavPath[256];
				std::string file = fileName;
				std::string folderName = file.substr(0, file.length() - 4);
				sprintf(wavPath, "../../Resource/music/%s/%s", folderName.c_str(), wavName.c_str());

				Mix_Chunk* wavChunk = Mix_LoadWAV(wavPath);
				_wavMap[code] = wavChunk;
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

			if (0 == playerPlay && 1 == trackNum)
			{
				//���� �÷��̰���
				sNoteLine* snoteLine = new sNoteLine;
				strncpy(snoteLine->line, noteLine, sizeof(noteLine));
				snoteLine->BarNum = curBarNum;
				snoteLine->isLongNote = playerPlay;
				_autoPlayNoteList.push_back(snoteLine);
			}
			else if (1 == playerPlay || 5 == playerPlay)	//bms������ 5�ϰ�� �ճ�Ʈ
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

void TrackManager::AddAutoNote(float sec, int barNum, char* wavCode)
{
	Wav* wav = new Wav(sec, barNum, _wavMap[wavCode]);
	wav->Init();
	_autoWavList.push_back(wav);
}

void TrackManager::AddNoteLine(int trackNum, sNoteLine* noteLine)
{
	//6�̻��� trackNum�� ����
	if((0 < trackNum) && (trackNum < 6))
		_trackNoteList[trackNum-1].push_back(noteLine);
}

void TrackManager::PlaceNoteTime(std::list<sNoteLine*>& noteLine)
{
	int judgeDeltaLine = 100;
	float noteTick = 0.0f;
	int durationTick = 0;

	//BMSE Parsing Info
	float SecondPerBeat = 0;				// BPM�� ���� 1��Ʈ�� �ʸ� ���Ѵ�.	(1/4���� ����)

	_noteList.clear();

	//���������� ��ȸ�ϸ鼭 �� ��Ʈ�� ���۽ð� ��ġ
	std::list<sNoteLine*>::iterator itr;
	for (itr = noteLine.begin(); itr != noteLine.end(); itr++)
	{
		sNoteLine* sNoteLine = (*itr);

		sNoteInfo noteInfo;
		noteInfo.startTick = 0;
		noteInfo.durationTick = 0;
		noteInfo.isLongNote = sNoteLine->isLongNote;
		noteInfo.barNum = sNoteLine->BarNum;

		noteTick = _SecondPerBar * sNoteLine->BarNum;		//���� �����ϴ� �ð�(Tick)
		int beat = strlen(sNoteLine->line) / 2;				//����
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
				noteTick += SecondPerBeat;
			}
			else
			{
				noteInfo.startTick = noteTick;

				if (0 == noteInfo.isLongNote)		// �����÷���
				{
					int startTick = noteInfo.startTick * 1000;
					float sec = startTick / 1000.0f;
					int barNum = noteInfo.barNum;
					AddAutoNote(sec, barNum, noteInfo.note);
				}
				else if (1 == noteInfo.isLongNote || 5 == noteInfo.isLongNote)
				{
					_noteList.push_back(noteInfo);
				}
				
				noteTick += SecondPerBeat;
			}
		}
	}
}

void TrackManager::CreateGameNote()
{
	int judgeDeltaLine = 100;

	//BMSE Parsing Info
	float SecondPerBeat = 0;				// BPM�� ���� 1��Ʈ�� �ʸ� ���Ѵ�.	(1/4���� ����)

	//AutoPlay��Ʈ �ð� ��ġ
	PlaceNoteTime(_autoPlayNoteList);

	for (int trackNum = 0; trackNum < _trackList->size(); trackNum++)
	{
		PlaceNoteTime(_trackNoteList[trackNum]);

		//���������� ��ȸ�ϸ鼭 �ճ�Ʈ ��� �� ��Ʈ�� Ʈ���� ��ġ
		std::list<sNoteInfo>::iterator reverseItr;
		for(reverseItr = _noteList.end(); reverseItr != _noteList.begin();)
		{
			reverseItr--;
			sNoteInfo curNote = (*reverseItr);

			float sec = 0;
			float duration = 0;
			int barNum = 0;
			std::string code;

			if(true == IsLongNote(_eFileType, curNote))
			{
				reverseItr--;
				sNoteInfo prevNote = (*reverseItr);
				int prevStartTick = prevNote.startTick * 1000;
				int curStartTick = curNote.startTick * 1000;

				sec = (float)(prevStartTick) / 1000.0f;
				duration = (float)(curStartTick - prevStartTick) / 1000.0f;
				barNum = prevNote.barNum;
				code = prevNote.note;
			}
			else
			{
				int curStartTick = curNote.startTick * 1000;
				sec = (float)(curStartTick) / 1000.0f;
				duration = (float)(curNote.durationTick) / 1000.0f;
				barNum = curNote.barNum;
				code = curNote.note;
			}
			_trackList->at(trackNum)->AddNoteToTrack(sec, duration, judgeDeltaLine, barNum, code);
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