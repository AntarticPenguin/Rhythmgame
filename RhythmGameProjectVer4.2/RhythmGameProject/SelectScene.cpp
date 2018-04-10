#include <io.h>

#include "SelectScene.h"

#include "SDL.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include "Sprite.h"
#include "Font.h"

SelectScene::SelectScene()
{
	_backgroundSprite = NULL;
}

SelectScene::~SelectScene()
{
	
}

void SelectScene::Init()
{
	_backgroundSprite = new Sprite("SelectBackground.csv", true);
	_backgroundSprite->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2,
		GameSystem::GetInstance()->GetWindowHeight() / 2);
	_backgroundSprite->SetAlpha(100);

	InitMusicList();
	for (int i = 0; i < _musicList.size(); i++)
	{
		Font* font = new Font("arialbd.ttf", 40);
		font->SetText(_musicList[i].c_str());
		font->SetPosition(300, i * 50);
		_fontList.push_back(font);
	}
}

void SelectScene::Deinit()
{
	if (NULL != _backgroundSprite)
	{
		delete _backgroundSprite;
		_backgroundSprite = NULL;
	}
}

void SelectScene::Update(int deltaTime)
{
	_backgroundSprite->Update(deltaTime);
}

void SelectScene::Render()
{
	_backgroundSprite->Render();

	for (int i = 0; i < _fontList.size(); i++)
		_fontList[i]->Render();
}

void SelectScene::InitMusicList()
{
	char filePath[256];
	sprintf(filePath, "../../Resource/music/*.bms");

	_finddata_t fd;
	long handle;
	int result = 1;

	int count = 0;		//bms, bme 두번 루프

	while(2 != count)
	{
		handle = _findfirst(filePath, &fd);
		if (-1 == handle)
		{
			printf("BMS, BME 파일이 없습니다.");
			return;
		}

		while (-1 != result)
		{
			_musicList.push_back(fd.name);
			result = _findnext(handle, &fd);
		}
		
		sprintf(filePath, "../../Resource/music/*.bme");
		result = 1;
		count++;
	}

	_findclose(handle);

	for (int i = 0; i < _musicList.size(); i++)
	{
		printf("File List: %s\n", _musicList[i].c_str());
	}
}

void SelectScene::KeyDown(int keyCode)
{
	switch (keyCode)
	{
	case SDLK_SPACE:
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_GAME);
		break;
	case SDLK_ESCAPE:
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_TITLE);
		break;
	}
}

void SelectScene::KeyUp(int keyCode)
{

}