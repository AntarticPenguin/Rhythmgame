#include "InputSystem.h"
#include "SDL.h"
#include "GameSystem.h"
#include "SceneManager.h"

InputSystem* InputSystem::_instance = 0;

InputSystem* InputSystem::GetInstance()
{
	if (NULL == _instance)
		_instance = new InputSystem();
	return _instance;
}

InputSystem::InputSystem()
{

}

InputSystem::~InputSystem()
{
}

void InputSystem::Init()
{
	for (int i = 0; i < 5; i++)
		_eKeyState[i] = eKeyState::KEY_NEUTRAL;

	_trackButton[SDLK_d] = eTrackButton::TRACK1;
	_trackButton[SDLK_f] = eTrackButton::TRACK2;
	_trackButton[SDLK_SPACE] = eTrackButton::TRACK3;
	_trackButton[SDLK_j] = eTrackButton::TRACK4;
	_trackButton[SDLK_k] = eTrackButton::TRACK5;
}

void InputSystem::ChangeKey()
{
	SDL_Event sdlEvent;
	int trackNumber = eTrackButton::TRACK1;
	std::map<int, eTrackButton>::iterator itr;

	while (true)
	{
		if (SDL_PollEvent(&sdlEvent))
		{
			if (SDL_KEYDOWN == sdlEvent.type)
			{
				int keycode = sdlEvent.key.keysym.sym;
				for (itr = _trackButton.begin(); itr != _trackButton.end();)
				{
					if (itr->second == (eTrackButton)trackNumber)
					{
						_trackButton.erase(itr++);
						_trackButton[keycode] = (eTrackButton)trackNumber;
						trackNumber++;
						break;
					}
					else
						itr++;
				}
			}
		}

		if (5 == trackNumber)
			break;
	}
}

std::map<int, eTrackButton>& InputSystem::GetTrackButton()
{
	return _trackButton;
}

void InputSystem::KeyDown(int keyCode)
{
	int trackNum = (int)_trackButton[keyCode];
	if (eKeyState::KEY_NEUTRAL == _eKeyState[trackNum])
	{
		_eKeyState[trackNum] = eKeyState::KEY_DOWN;
		//printf("DOWN: %s\n", SDL_GetKeyName(keyCode));
	}
}

void InputSystem::KeyUp(int keyCode)
{
	int trackNum = (int)_trackButton[keyCode];
	if (eKeyState::KEY_HOLD == _eKeyState[trackNum])
	{
		_eKeyState[trackNum] = eKeyState::KEY_UP;
		//printf("UP: %s\n", SDL_GetKeyName(keyCode));
	}
}

bool InputSystem::IsKeyDown(int trackNum)
{
	if (eKeyState::KEY_DOWN == _eKeyState[trackNum])
	{
		_eKeyState[trackNum] = eKeyState::KEY_HOLD;
		return true;
	}
	return false;
}

bool InputSystem::IsKeyHold(int trackNum)
{
	if (eKeyState::KEY_HOLD == _eKeyState[trackNum])
		return true;
	return false;
}

bool InputSystem::IsKeyUp(int trackNum)
{
	if (eKeyState::KEY_UP == _eKeyState[trackNum])
	{
		_eKeyState[trackNum] = eKeyState::KEY_NEUTRAL;
		return true;
	}
	return false;
}