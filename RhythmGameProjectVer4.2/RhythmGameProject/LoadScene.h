#pragma once
#include <list>
#include "Scene.h"

enum class eLoadType
{
	PARSING,
	CREATE_NOTE,
	FINISH,
};

enum class eFileType
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

class LoadScene : public Scene
{
public:
	LoadScene();
	~LoadScene();

public:
	void Init() override;
	void Deinit() override;
	void Update(int deltaTime) override;
	void Render() override;

	//BMS Parsing & Create Note
private:
	FILE* _fp;
	char* _record;
	int _fieldFlag;		//HEADER = 1, MAIN DATA = 2
	eLoadType _loadingType;

	std::list<sNoteLine*>* _trackNoteList;
	std::list<sNoteLine*> _autoPlayNoteList;
	std::list<sNoteInfo> _noteList;

	int _curBarNum;
	int _BPM;				// BMSE 스크립트를 통해 BPM을 구한다.
	float _SecondPerBar;	// BPM에 따른 1마디당 초를 구한다.	(1/32박자 기준)
	char _longNoteKey[3];	// BME에서 사용하는 longNote 키값
	eFileType _eFileType;		// BME, BMS 파일 형식 구분
	int _trackSize;

public:
	void ParsingBMS(int deltaTime);
	void AddNoteLine(int trackNum, sNoteLine* noteLine);
	void CreateGameNote();
	void PlaceNoteTime(std::list<sNoteLine*>& noteLine);
	bool IsLongNote(eFileType _eFileType, sNoteInfo curNote);
};