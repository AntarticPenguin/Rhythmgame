#include "LoadScene.h"

#include "SDL.h"
#include "SDL_mixer.h"
#include "DataManager.h"
#include "GameSystem.h"
#include "SceneManager.h"

LoadScene::LoadScene()
{
	_fp = NULL;
	_record = NULL;
	_fieldFlag = 1;

	_curBarNum = 0;
	_BPM = 0;
	_SecondPerBar = 0;

	_trackSize = 0;
}

LoadScene::~LoadScene()
{
}

void LoadScene::Init()
{
	char filePath[256];
	sprintf(filePath, "../../Resource/music/%s", DataManager::GetInstance()->GetMusicTitle().c_str());
	_fp = fopen(filePath, "r");
	if (NULL == _fp)
	{
		printf("파일열기 실패: %s\n", filePath);
	}

	DataManager::GetInstance()->CreateTracks();
	_trackSize = DataManager::GetInstance()->GetTrackList().size();
	_trackNoteList = new std::list<sNoteLine*>[_trackSize];

	Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 640);
	Mix_AllocateChannels(256);
}

void LoadScene::Deinit()
{

}

void LoadScene::Update(int deltaTime)
{
	switch (_loadingType)
	{
	case eLoadType::PARSING:
		ParsingBMS(deltaTime);
		break;
	case eLoadType::CREATE_NOTE:
		CreateGameNote();
		break;
	case eLoadType::FINISH:
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_GAME);
		return;
	}
}

void LoadScene::Render()
{

}

void LoadScene::ParsingBMS(int deltaTime)
{
	printf("BMS 파일 파싱...\n");

	int updateTick = 0;
	int startTick = SDL_GetTicks();
	int oldTick = 0;

	char buffer[1024];
	char barInfo[256];
	char noteLine[1024];

	while (true)
	{
		oldTick = SDL_GetTicks();
		updateTick += oldTick - startTick;

		if (deltaTime < updateTick)
			return;

		_record = fgets(buffer, sizeof(buffer), _fp);
		if (NULL == _record)
		{
			//노래 시간, 트랙 길이 세팅
			int tempo = _BPM / 60;
			float playTimeSec = _SecondPerBar * (_curBarNum + 1);
			int trackheight = GameSystem::GetInstance()->GetWindowHeight() * playTimeSec * tempo;
			trackheight /= 2;
			GameSystem::GetInstance()->SetTrackHeight(trackheight);
			GameSystem::GetInstance()->SetPlayTimeTick(playTimeSec);

			_loadingType = eLoadType::CREATE_NOTE;
			fclose(_fp);
			printf("FINISH!!!\n");
			return;
		}

		char* token = NULL;

		if (1 == _fieldFlag)
			token = strtok(_record, " #\n");
		else if (2 == _fieldFlag)
		{
			token = strtok(_record, "#:\n");
		}

		//HEADER FEILD PARSING
		while (_fieldFlag == 1)
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
				DataManager::GetInstance()->SetSecondPerBar(_SecondPerBar);
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

				char wavPath[256];
				std::string file = DataManager::GetInstance()->GetMusicTitle();
				std::string folderName = file.substr(0, file.length() - 4);
				sprintf(wavPath, "../../Resource/music/%s/%s", folderName.c_str(), wavName.c_str());

				/*Mix_Chunk* wavChunk = Mix_LoadWAV(wavPath);
				_wavMap[code] = wavChunk;*/
				DataManager::GetInstance()->AddWavToMap(wavPath, code);
			}
			else if (!strcmp(token, "MAIN"))
			{
				_fieldFlag = 2;
				_record = fgets(buffer, sizeof(buffer), _fp);
				token = strtok(_record, "#:\n");
				break;
			}
			token = strtok(NULL, " #\n");
		}

		//MAIN DATA PARSING
		while (_fieldFlag == 2)
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
			sscanf(barInfo, "%3d", &_curBarNum);

			int playerPlay = barInfo[3] - '0';		//오토플레이정보
			int trackNum = atoi(&barInfo[4]);		//노트가 들어갈 트랙 넘버

			if (0 == playerPlay && 1 == trackNum)
			{
				//오토 플레이관련
				sNoteLine* snoteLine = new sNoteLine;
				strncpy(snoteLine->line, noteLine, sizeof(noteLine));
				snoteLine->BarNum = _curBarNum;
				snoteLine->isLongNote = playerPlay;
				_autoPlayNoteList.push_back(snoteLine);
			}
			else if (1 == playerPlay || 5 == playerPlay)	//bms에서는 5일경우 롱노트
			{
				sNoteLine* snoteLine = new sNoteLine;
				strncpy(snoteLine->line, noteLine, sizeof(noteLine));
				snoteLine->BarNum = _curBarNum;
				snoteLine->isLongNote = playerPlay;

				AddNoteLine(trackNum, snoteLine);
			}
		}
	}
}

void LoadScene::AddNoteLine(int trackNum, sNoteLine* noteLine)
{
	//6이상의 trackNum은 무시
	if ((0 < trackNum) && (trackNum < 6))
		_trackNoteList[trackNum - 1].push_back(noteLine);
}

void LoadScene::CreateGameNote()
{
	printf("Make Game Note...\n");
	int judgeDeltaLine = 100;

	//BMSE Parsing Info
	float SecondPerBeat = 0;				// BPM에 따른 1비트당 초를 구한다.	(1/4박자 기준)

	//AutoPlay노트 시간 배치
	PlaceNoteTime(_autoPlayNoteList);

	for (int trackNum = 0; trackNum < _trackSize; trackNum++)
	{
		PlaceNoteTime(_trackNoteList[trackNum]);

		//역방향으로 순회하면서 롱노트 계산 및 노트를 트랙에 배치
		std::list<sNoteInfo>::iterator reverseItr;
		for (reverseItr = _noteList.end(); reverseItr != _noteList.begin();)
		{
			reverseItr--;
			sNoteInfo curNote = (*reverseItr);

			float sec = 0;
			float duration = 0;
			int barNum = 0;
			std::string code;

			if (true == IsLongNote(_eFileType, curNote))
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
			//_trackList[trackNum]->AddNoteToTrack(sec, duration, judgeDeltaLine, barNum, _wavMap[code]);
			DataManager::GetInstance()->AddNoteToTrack(trackNum, sec, duration, judgeDeltaLine, barNum, code);
		}
	}

	_loadingType = eLoadType::FINISH;
}

void LoadScene::PlaceNoteTime(std::list<sNoteLine*>& noteLine)
{
	int judgeDeltaLine = 100;
	float noteTick = 0.0f;
	int durationTick = 0;

	//BMSE Parsing Info
	float SecondPerBeat = 0;				// BPM에 따른 1비트당 초를 구한다.	(1/4박자 기준)

	_noteList.clear();

	//정방향으로 순회하면서 각 노트의 시작시간 배치
	std::list<sNoteLine*>::iterator itr;
	for (itr = noteLine.begin(); itr != noteLine.end(); itr++)
	{
		sNoteLine* sNoteLine = (*itr);

		sNoteInfo noteInfo;
		noteInfo.startTick = 0;
		noteInfo.durationTick = 0;
		noteInfo.isLongNote = sNoteLine->isLongNote;
		noteInfo.barNum = sNoteLine->BarNum;

		noteTick = _SecondPerBar * sNoteLine->BarNum;		//마디가 시작하는 시간(Tick)
		int beat = strlen(sNoteLine->line) / 2;				//박자
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
				noteTick += SecondPerBeat;
			}
			else
			{
				noteInfo.startTick = noteTick;

				if (0 == noteInfo.isLongNote)		// 오토플레이
				{
					int startTick = noteInfo.startTick * 1000;
					float sec = startTick / 1000.0f;
					//AddAutoNote(sec, noteInfo.note);
					DataManager::GetInstance()->AddAutoNote(sec, noteInfo.note);
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

bool LoadScene::IsLongNote(eFileType _eFileType, sNoteInfo curNote)
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