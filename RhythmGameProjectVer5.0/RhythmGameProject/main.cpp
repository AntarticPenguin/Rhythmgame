#include <stdio.h>
#include <stdlib.h>
#include <ctime>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include "Input.h"

int main(int argc, char* argv[])
{
	srand((unsigned)time(NULL));

	int windowWidth = GameSystem::GetInstance()->GetWindowWidth();
	int windowHeight = GameSystem::GetInstance()->GetWindowHeight();

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);

	if (TTF_Init() < 0)
	{
		//����ó��
		printf("Error Init SDL Font\n");
	}
	atexit(TTF_Quit);
	
	SDL_Window* sdlWindow = SDL_CreateWindow(
		"SDL TEST",
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED, 
		windowWidth,
		windowHeight,
		SDL_WINDOW_OPENGL
	);

	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(sdlWindow, 0, 0);
	GameSystem::GetInstance()->SetRenderer(sdlRenderer);
	Input::GetInstance().Init();

	SceneManager::GetInstance()->ChangeScene(eScene::SCENE_TITLE);

	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);

	SDL_Event sdlEvent;
	int frame = 0;
	int oldFrame = 0;
	int curTick = SDL_GetTicks();
	int oldTick = curTick;

	bool bQuit = false;

	int pauseTimeTick = 0;

	while (1)
	{
		if (bQuit)
			break;

		curTick = SDL_GetTicks();
		frame = (SDL_GetTicks() % 1000) * 120;		// * ������
		frame /= 1000;
		
		if (oldFrame != frame)
		{
			oldFrame = frame;

			if(0 != GameSystem::GetInstance()->GetPauseTime())
			{
				oldTick = GameSystem::GetInstance()->GetPauseTime();
				GameSystem::GetInstance()->SetPauseTime(0);
			}

			int deltaTime = curTick - oldTick;			//deltaTime ���
			oldTick = curTick;
			
			//�̺�Ʈ ó��
			if (SDL_PollEvent(&sdlEvent))				//�̺�Ʈ�� �����´�
			//while(SDL_PollEvent(&sdlEvent))				//Ű�Է��̺�Ʈ�� ��Ƽ� ó��(����Ű �Է�)
			{
				auto str = SDL_GetKeyName(sdlEvent.key.keysym.sym);
				switch (sdlEvent.type)
				{
				case SDL_QUIT:
					bQuit = true;
					break;
				case SDL_KEYDOWN:
					Input::GetInstance().InputEvent(sdlEvent);
					SceneManager::GetInstance()->InputProcess();
					break;
				case SDL_KEYUP:
					Input::GetInstance().InputEvent(sdlEvent);
					/*if (SDLK_F4 == sdlEvent.key.keysym.sym)
						GameSystem::GetInstance()->SetPauseTime(SDL_GetTicks());*/
					break;
				default:
					break;
				}
			}
			
			SceneManager::GetInstance()->Update(deltaTime);
			/*if (Input::GetInstance().IsPressedAnyKey())
				SceneManager::GetInstance()->InputProcess();*/
				

			//������
			SDL_RenderClear(sdlRenderer);
			{
				SceneManager::GetInstance()->Render();
			}
			SDL_RenderPresent(sdlRenderer);
		}
	}

	return 0;
}