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
	int _BPM;				// BMSE ��ũ��Ʈ�� ���� BPM�� ���Ѵ�.
	float _SecondPerBar;	// BPM�� ���� 1����� �ʸ� ���Ѵ�.	(1/32���� ����)
	char _longNoteKey[3];	// BME���� ����ϴ� longNote Ű��
	eFileType _eFileType;		// BME, BMS ���� ���� ����
	int _trackSize;

public:
	void ParsingBMS(int deltaTime);
	void AddNoteLine(int trackNum, sNoteLine* noteLine);
	void CreateGameNote();
	void PlaceNoteTime(std::list<sNoteLine*>& noteLine);
	bool IsLongNote(eFileType _eFileType, sNoteInfo curNote);
};