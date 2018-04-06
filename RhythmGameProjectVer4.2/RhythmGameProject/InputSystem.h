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
	KEY_DOWN,
	KEY_UP,
};

class InputSystem
{
private:
	static InputSystem* _instance;
	eKeyState _eKeyState[100];
	std::map<int, eTrackButton> _trackButton;

public:
	static InputSystem* GetInstance();
	void Init();
	void UpdateInput();

public:
	bool IsKeyDown(int keycode);
	bool IsKeyUp(int keycode);
	void ChangeKey();
	std::map<int, eTrackButton>& GetTrackButton();
	~InputSystem();

private:
	InputSystem();
};