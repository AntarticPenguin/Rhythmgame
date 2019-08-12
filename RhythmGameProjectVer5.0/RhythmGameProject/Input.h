#pragma once
#include <map>
#include <SDL_scancode.h>

enum class eKeyState
{
	DOWN,
	UP,
};

enum class eGameKey
{
	MENU = 0,
	BTN_A,
	BTN_B,
	BTN_SPACE,
	BTN_C,
	BTN_D,
	SPEED_UP,
	SPEED_DOWN,
	VOL_UP,
	VOL_DOWN,
	ARROW_UP,
	ARROW_DOWN,
	MAX,
};

union SDL_Event;

class Input
{
private:
	static Input* _instance;
	eKeyState* _gameKeyState;
	std::map<int, eGameKey> _gameKeyMap;

	const uint8_t* _keyboardState;

private:
	Input();

public:
	static Input& GetInstance();
	void Init();
	void InputEvent(SDL_Event& sdlEvent);

public:
	bool IsPressedAnyKey();
	bool IsPressed(eGameKey key);
	bool IsReleased(eGameKey key);
};