#pragma once
#include <vector>
#include <list>
#include <map>

#include "GameObject.h"

class Track;
class Font;
enum eTrackButton;

enum eJudge
{
	NONE,
	PERFECT,
	GREAT,
	MISS,
	JUDGE_START_PERFECT,
	JUDGE_START_GREAT,
};

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
	int startTick;
	int durationTick;
	int isLongNote;
	int barNum;
}sNoteInfo;

class TrackManager : public GameObject
{
public:
	enum eTrackNum
	{
		TRACK01,
		TRACK02,
		TRACK03,
		TRACK04,
		TRACK05,
	};

private:
	std::vector<Track*>* _trackList;

	Font* _combofont;
	Font* _scorefont;

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

	int _BPM;				// BMSE ��ũ��Ʈ�� ���� BPM�� ���Ѵ�.
	float _SecondPerBar;	// BPM�� ���� 1����� �ʸ� ���Ѵ�.	(1/32���� ����)
	char _longNoteKey[3];	// BME���� ����ϴ� longNote Ű��
	eFileType _eFileType;		// BME, BMS ���� ���� ����

public:
	void ParsingBMS(const char* fileName);
	void AddNoteLine(int trackNum, sNoteLine* noteLine);
	void CreateGameNote();
	bool IsLongNote(eFileType _eFileType, sNoteInfo curNote);

	//input
private:
	std::map<int, eTrackButton> _trackButton;

public:
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};