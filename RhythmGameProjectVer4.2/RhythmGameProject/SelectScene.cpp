#include "SelectScene.h"

#include "SDL.h"
#include "GameSystem.h"
#include "SceneManager.h"
#include "Sprite.h"

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

	//뮤직 리스트 등록
	std::vector<MusicContainer*> _musicList;
	_musicList = GameSystem::GetInstance()->GetMusicList();

	for (int i = 0; i < _musicList.size(); i++)
	{
		_musicList.SetPosition();
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