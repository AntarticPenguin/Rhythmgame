#include <io.h>

#include "SelectScene.h"

#include "SDL.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include "Sprite.h"
#include "MusicInfoBox.h"

SelectScene::SelectScene()
{
	_backgroundSprite = NULL;
	_selectPosX = 515;
	_selectPosY = 100;

	_viewIndex = 0;
	_startViewIndex = 0;
	_endViewIndex = 6;
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
	InitViewList();		// view리스트 위치값 초기화

	_selectBox = new Sprite("SelectSprite.csv", true);
	_selectBox->SetAlpha(100);
	_selectBox->SetPosition(_selectPosX, _selectPosY);
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
	_selectBox->Update(deltaTime);
	_selectBox->SetPosition(_selectPosX, _selectPosY);
}

void SelectScene::Render()
{
	_backgroundSprite->Render();
	_selectBox->Render();
	
	for (int i = _startViewIndex; i <= _endViewIndex; i++)
		_viewList[i]->Render();
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
}

void SelectScene::InitViewList()
{
	for (int i = 0; i < _musicList.size(); i++)
	{
		MusicInfoBox* infoBox = new MusicInfoBox(_musicList[i]);
		infoBox->Init();
		infoBox->SetPosition(300, i * 100 + 75);
		_viewList.push_back(infoBox);
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
	case SDLK_UP:
		_viewIndex--;
		if (_viewIndex < _startViewIndex)
			_viewIndex = _startViewIndex;
		printf("index: %d\n", _viewIndex);

		//SelectBox 위치
		_selectPosY -= 100;
		if (_selectPosY <= 100)
			_selectPosY = 100;
		break;
	case SDLK_DOWN:
		_viewIndex++;

		//밑에 더 리스트가 있는지 체크
		//있으면 startIndex, endIndex 증가
		if (NULL != _viewList[_viewIndex])
		{
			_startViewIndex++;
			_endViewIndex++;
		}

		if (_endViewIndex <= _viewIndex)
			_viewIndex = _endViewIndex;
		printf("index: %d\n", _viewIndex);

		//SelectBox 위치
		_selectPosY += 100;
		if (700 <= _selectPosY)
			_selectPosY = 700;
		break;
	}
}

void SelectScene::KeyUp(int keyCode)
{

}