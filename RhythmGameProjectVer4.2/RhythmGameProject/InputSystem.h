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

enum eKeyState
{
	KEY_NEUTRAL,
	KEY_DOWN,
	KEY_HOLD,
	KEY_UP,
};

class InputSystem
{
private:
	static InputSystem* _instance;
	eKeyState _eKeyState[5];
	std::map<int, eTrackButton> _trackButton;

public:
	static InputSystem* GetInstance();
	void Init();

public:
	void ChangeKey();
	std::map<int, eTrackButton>& GetTrackButton();
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
	bool IsKeyDown(int trackNum);
	bool IsKeyHold(int trackNum);
	bool IsKeyUp(int trackNum);
	~InputSystem();

private:
	InputSystem();
};