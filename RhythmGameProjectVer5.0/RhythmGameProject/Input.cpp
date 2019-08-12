#include "Input.h"
#include "SDL.h"

Input* Input::_instance = 0;

Input& Input::GetInstance()
{
	if (0 == _instance)
		_instance = new Input();
	return *_instance;
}

Input::Input()
{

}

void Input::Init()
{
	_keyboardState = SDL_GetKeyboardState(NULL);

	_gameKeyMap[SDL_SCANCODE_Q] = eGameKey::MENU;
	_gameKeyMap[SDL_SCANCODE_D] = eGameKey::BTN_A;
	_gameKeyMap[SDL_SCANCODE_F] = eGameKey::BTN_B;
	_gameKeyMap[SDL_SCANCODE_SPACE] = eGameKey::BTN_SPACE;
	_gameKeyMap[SDL_SCANCODE_J] = eGameKey::BTN_C;
	_gameKeyMap[SDL_SCANCODE_K] = eGameKey::BTN_D;
	_gameKeyMap[SDL_SCANCODE_F3] = eGameKey::SPEED_UP;
	_gameKeyMap[SDL_SCANCODE_F4] = eGameKey::SPEED_DOWN;
	_gameKeyMap[SDL_SCANCODE_F7] = eGameKey::VOL_UP;
	_gameKeyMap[SDL_SCANCODE_F8] = eGameKey::VOL_DOWN;
	_gameKeyMap[SDL_SCANCODE_UP] = eGameKey::ARROW_UP;
	_gameKeyMap[SDL_SCANCODE_DOWN] = eGameKey::ARROW_DOWN;

	_gameKeyState = new eKeyState[(int)eGameKey::MAX];
	for (int key = 0; key < (int)eGameKey::MAX; key++)
		_gameKeyState[key] = eKeyState::UP;
}

void Input::InputEvent(SDL_Event& sdlEvent)
{
	auto str = SDL_GetKeyName(sdlEvent.key.keysym.sym);
	auto scanCode = SDL_GetScancodeFromKey(sdlEvent.key.keysym.sym);
	if (_gameKeyMap.find(scanCode) != _gameKeyMap.end())
	{
		eGameKey btn = _gameKeyMap[scanCode];
		if (_keyboardState[scanCode])
		{
			_gameKeyState[(int)btn] = eKeyState::DOWN;
			printf("%s is DOWN\n", str);
		}
		else
		{
			_gameKeyState[(int)btn] = eKeyState::UP;
			printf("%s is UP\n", str);
		}
	}
}

bool Input::IsPressedAnyKey()
{
	for (int btn = 0; btn < (int)eGameKey::MAX; btn++)
	{
		if (eKeyState::DOWN == _gameKeyState[btn])
			return true;
	}
	return false;
}

/*
	Check down or hold
*/
bool Input::IsPressed(eGameKey key)
{
	return (eKeyState::DOWN == _gameKeyState[(int)key]);
}

bool Input::IsReleased(eGameKey key)
{
	return (eKeyState::UP == _gameKeyState[(int)key]);
}