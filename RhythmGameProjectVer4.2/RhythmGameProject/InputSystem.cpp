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