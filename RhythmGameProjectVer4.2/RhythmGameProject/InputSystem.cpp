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
	for (int i = 0; i < 100; i++)
		_eKeyState[i] = eKeyState::KEY_UP;

	_trackButton[SDLK_d] = eTrackButton::TRACK1;
	_trackButton[SDLK_f] = eTrackButton::TRACK2;
	_trackButton[SDLK_SPACE] = eTrackButton::TRACK3;
	_trackButton[SDLK_j] = eTrackButton::TRACK4;
	_trackButton[SDLK_k] = eTrackButton::TRACK5;
}

void InputSystem::UpdateInput()
{
	const Uint8* keyStates = const_cast<Uint8*>(SDL_GetKeyboardState(NULL));

	SDL_Event sdlEvent;
	//이벤트 처리
	if (SDL_PollEvent(&sdlEvent))				//이벤트를 꺼내온다
	{
		if (SDL_KEYUP == sdlEvent.type)
		{
			//SceneManager::GetInstance()->KeyUp(sdlEvent.key.keysym.sym);
			IsKeyUp((int)SDL_GetScancodeFromKey(sdlEvent.key.keysym.sym));
			
		}

		if (SDL_KEYDOWN == sdlEvent.type)
		{
			//SceneManager::GetInstance()->KeyDown(sdlEvent.key.keysym.sym);
			IsKeyDown((int)SDL_GetScancodeFromKey(sdlEvent.key.keysym.sym));	//가상키값을 실제 물리키값으로 변환

			//Pause됬을 때, 정지된 시간을 저장
			if (SDLK_F4 == sdlEvent.key.keysym.sym)
				GameSystem::GetInstance()->SetPauseTime(SDL_GetTicks());
		}
	}
}

bool InputSystem::IsKeyDown(int keycode)
{
	_eKeyState[keycode] = eKeyState::KEY_DOWN;

	if (eKeyState::KEY_DOWN == _eKeyState[keycode])
		printf("keycode: %s state: DOWN\n", SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)keycode)));
	return (eKeyState::KEY_DOWN == _eKeyState[keycode]);
}

bool InputSystem::IsKeyUp(int keycode)
{
	_eKeyState[keycode] = eKeyState::KEY_UP;

	if (eKeyState::KEY_UP == _eKeyState[keycode])
		printf("keycode: %s state: UP\n", SDL_GetKeyName(SDL_GetKeyFromScancode((SDL_Scancode)keycode)));
	return (eKeyState::KEY_UP == _eKeyState[keycode]);
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