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
	std::vector<Track*> _trackList;
	std::list<Wav*> _autoWavList;
	float _SecondPerBar;

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
};