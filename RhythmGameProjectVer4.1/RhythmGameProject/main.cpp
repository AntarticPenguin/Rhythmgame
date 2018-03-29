#include <stdio.h>
#include <stdlib.h>
#include <ctime>

#include "SDL.h"
#include "SDL_ttf.h"
#include "SDL_mixer.h"
#include "GameSystem.h"
#include "SceneManager.h"

int main(int argc, char* argv[])
{
	srand((unsigned)time(NULL));

	int windowWidth = GameSystem::GetInstance()->GetWindowWidth();
	int windowHeight = GameSystem::GetInstance()->GetWindowHeight();

	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_AUDIO);

	if (TTF_Init() < 0)
	{
		//에러처리
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

	SceneManager::GetInstance()->ChangeScene(eScene::SCENE_LOGO);

	SDL_SetRenderDrawColor(sdlRenderer, 0, 0, 0, 255);

	SDL_Event sdlEvent;
	int frame = 0;
	int oldFrame = 0;
	int curTick = SDL_GetTicks();
	int oldTick = curTick;

	int pauseTimeTick = 0;

	while (1)
	{
		curTick = SDL_GetTicks();
		frame = (SDL_GetTicks() % 1000) * 60;		//60프레임
		frame /= 1000;

		if (oldFrame != frame)
		{
			oldFrame = frame;

			if(0 != GameSystem::GetInstance()->GetPauseTime())
			{
				oldTick = GameSystem::GetInstance()->GetPauseTime();
				GameSystem::GetInstance()->SetPauseTime(0);
			}

			int deltaTime = curTick - oldTick;			//deltaTime 계산
			oldTick = curTick;
		
			//이벤트 처리
			if (SDL_PollEvent(&sdlEvent))				//이벤트를 꺼내온다
			{
				if ((SDL_QUIT == sdlEvent.type))
					break;

				if (SDL_KEYUP == sdlEvent.type)
				{
					SceneManager::GetInstance()->KeyUp(sdlEvent.key.keysym.sym);
				}

				if (SDL_KEYDOWN == sdlEvent.type)
				{
					SceneManager::GetInstance()->KeyDown(sdlEvent.key.keysym.sym);

					//Pause됬을 때, 정지된 시간을 저장
					if (SDLK_F4 == sdlEvent.key.keysym.sym)
						GameSystem::GetInstance()->SetPauseTime(SDL_GetTicks());
				}
			}
			SceneManager::GetInstance()->Update(deltaTime);

			//렌더링
			SDL_RenderClear(sdlRenderer);
			{
				SceneManager::GetInstance()->Render();
			}
			SDL_RenderPresent(sdlRenderer);
		}
	}

	return 0;
}