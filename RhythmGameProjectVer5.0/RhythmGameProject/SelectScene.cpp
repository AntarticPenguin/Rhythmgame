#include <io.h>

#include "SelectScene.h"

#include "SDL.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include "DataManager.h"
#include "Sprite.h"
#include "MusicInfoBox.h"

SelectScene::SelectScene()
{
	_backgroundSprite = NULL;
	_selectPosX = 515;
	_selectPosY = 100;

	_selectIndex = 0;
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
	
	if (6 < _musicList.size())
	{
		_endViewIndex = 6;
		_endPosY = 700;
	}
	else
	{
		_endViewIndex = _musicList.size() - 1;
		_endPosY = _musicList.size() * 100;
	}
		
}

void SelectScene::SelectUp()
{
	//SelectBox 위치
	_selectPosY -= 100;
	if (_selectPosY <= 100)
		_selectPosY = 100;

	_selectIndex--;
	if (_selectIndex < _startViewIndex)
	{
		_startViewIndex--;
		if(_startViewIndex < 0)
		{
			_startViewIndex = 0;
			_selectIndex = _startViewIndex;
			return;
		}
		_endViewIndex = _startViewIndex + 6;

		//위치 조정
		int index = 0;
		for (int i = _startViewIndex; i < _endViewIndex + 1; i++)
		{
			_viewList[i]->SetPosition(300, index * 100 + 75);
			index++;
		}
	}
}

void SelectScene::SelectDown()
{
	//SelectBox 위치
	_selectPosY += 100;
	if (_endPosY <= _selectPosY)
		_selectPosY = _endPosY;
	
	_selectIndex++;
	if (_endViewIndex < _selectIndex)
	{
		_endViewIndex++;
		if (_viewList.size() <= _endViewIndex)
		{
			_endViewIndex = _viewList.size() - 1;
			_selectIndex = _endViewIndex;
			return;
		}
		_startViewIndex = _endViewIndex - 6;

		//위치 조정
		int index = 0;
		for (int i = _startViewIndex; i < _viewList.size(); i++)
		{
			_viewList[i]->SetPosition(300, index * 100 + 75);
			index++;
		}
	}
}

void SelectScene::InputProcess()
{
	if (Input::GetInstance().IsPressed(eGameKey::BTN_SPACE))
	{
		DataManager::GetInstance()->SetMusicTitle(_musicList[_selectIndex]);
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_LOAD);
	}
	else if (Input::GetInstance().IsPressed(eGameKey::MENU))
	{
		SceneManager::GetInstance()->ChangeScene(eScene::SCENE_TITLE);
	}
	else if (Input::GetInstance().IsPressed(eGameKey::ARROW_UP))
	{
		SelectUp();
	}
	else if (Input::GetInstance().IsPressed(eGameKey::ARROW_DOWN))
	{
		SelectDown();
	}
}