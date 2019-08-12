#pragma once
#include <map>

enum eTrackButton
{
	TRACK1,
	TRACK2,
	TRACK3,
	TRACK4,
	TRACK5,
};

class InputKeyManager
{
private:
	static InputKeyManager* _instance;
	std::map<int, eTrackButton> _trackButton;

public:
	static InputKeyManager* GetInstance();
	void Init();
	void ChangeKey();
	std::map<int, eTrackButton>& GetTrackButton();
	~InputKeyManager();

private:
	InputKeyManager();
};