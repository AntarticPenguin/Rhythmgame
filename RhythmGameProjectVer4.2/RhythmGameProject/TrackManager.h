#pragma once
#include <vector>
#include <list>
#include <map>
#include <string>

#include "GameObject.h"

class Track;
class Sprite;
class Wav;

enum eFileType
{
	BME,
	BMS
};

typedef struct sNoteLine
{
	char line[1024];
	int BarNum;
	int isLongNote;
}sNoteLine;

typedef struct sNoteInfo
{
	char note[3];
	float startTick;
	int durationTick;
	int isLongNote;
	int barNum;
}sNoteInfo;

class TrackManager : public GameObject
{
private:
	std::vector<Track*>* _trackList;
	std::map<std::string, Mix_Chunk*> _wavMap;
	std::list<Wav*> _autoWavList;

	Sprite* _bgSprite;
	Sprite* _judgeLineSprite;

	int _playTimeTick;
	int _curBarNum;

public:
	TrackManager();
	~TrackManager();

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();

	//BMS Parsing & Create Note
private:
	std::list<sNoteLine*>* _trackNoteList;
	std::list<sNoteInfo> _noteList;
	std::list<sNoteLine*> _autoPlayNoteList;

	int _BPM;				// BMSE ��ũ��Ʈ�� ���� BPM�� ���Ѵ�.
	float _SecondPerBar;	// BPM�� ���� 1����� �ʸ� ���Ѵ�.	(1/32���� ����)
	char _longNoteKey[3];	// BME���� ����ϴ� longNote Ű��
	eFileType _eFileType;		// BME, BMS ���� ���� ����

public:
	void ParsingBMS(const char* fileName);
	void AddAutoNote(float sec, char* wavCode);
	void AddNoteLine(int trackNum, sNoteLine* noteLine);
	void PlaceNoteTime(std::list<sNoteLine*>& noteLine);
	void CreateGameNote();
	bool IsLongNote(eFileType _eFileType, sNoteInfo curNote);
};