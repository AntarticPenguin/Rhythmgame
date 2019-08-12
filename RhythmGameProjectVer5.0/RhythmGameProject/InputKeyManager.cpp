#include "InputKeyManager.h"
#include "SDL.h"

InputKeyManager* InputKeyManager::_instance = 0;

InputKeyManager* InputKeyManager::GetInstance()
{
	if (NULL == _instance)
		_instance = new InputKeyManager();
	return _instance;
}

InputKeyManager::InputKeyManager()
{

}

InputKeyManager::~InputKeyManager()
{
}

void InputKeyManager::Init()
{
	_trackButton[SDLK_d] = eTrackButton::TRACK1;
	_trackButton[SDLK_f] = eTrackButton::TRACK2;
	_trackButton[SDLK_SPACE] = eTrackButton::TRACK3;
	_trackButton[SDLK_j] = eTrackButton::TRACK4;
	_trackButton[SDLK_k] = eTrackButton::TRACK5;
}

void InputKeyManager::ChangeKey()
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

std::map<int, eTrackButton>& InputKeyManager::GetTrackButton()
{
	return _trackButton;
}