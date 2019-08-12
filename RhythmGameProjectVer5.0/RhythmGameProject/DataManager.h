#pragma once
#include <string>
#include <vector>
#include <map>

enum eJudge;
struct Mix_Chunk;
class Track;
class Wav;

class DataManager
{
	//Singleton
private:
	static DataManager* _instance;

public:
	static DataManager* GetInstance();

public:
	int _combo;
	int _maxCombo;
	int _score;

	int _missCount;
	int _greatCount;
	int _perfectCount;

	//SCORE & COMBO & JUDGE
public:
	int GetCombo();
	int GetMaxCombo();
	void IncreaseCombo();

	int GetScore();
	void SetScore(int score);
	void IncreaseScore(eJudge judge);

	void ResetCombo();
	void ResetScore();

	int GetMissCount();
	int GetGreatCount();
	int GetPerfectCount();

	//Music Title
private:
	std::string _musicTitle;

public:
	void SetMusicTitle(std::string title);
	std::string GetMusicTitle();

	//Play Data
private:
	std::vector<Track*> _trackList;
	std::map<std::string, Mix_Chunk*> _wavMap;
	std::vector<Wav*> _autoWavList;
	int _tickPerBeat;		//second per beat * 1000
	float _SecondPerBar;

public:
	void CreateTracks();
	std::vector<Track*>& GetTrackList();
	std::vector<Wav*>& GetAutoWavList();
	void SetTickPerBeat(int BPM);
	int GetTickPerBeat();
	float GetSecondPerBar();
	void SetSecondPerBar(float secondPerbar);

	void AddWavToMap(const char* filePath, std::string key);
	void AddNoteToTrack(int trackNum, float sec, float duration, int judgeDeltaLine, int barNum, std::string code);
	void AddAutoNote(float sec, char* code);

private:
	DataManager();
	~DataManager();
};