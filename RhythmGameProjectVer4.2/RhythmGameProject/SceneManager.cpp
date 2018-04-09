#include <stdio.h>

#include "SceneManager.h"
//#include "LogoScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "SelectScene.h"

SceneManager* SceneManager::_instance = NULL;

SceneManager::SceneManager()
{
	_scene = NULL;
}

SceneManager::~SceneManager()
{
	if (NULL != _scene)
	{
		_scene->Deinit();
		delete _scene;
	}
}

SceneManager* SceneManager::GetInstance()
{
	if (NULL == _instance)
		_instance = new SceneManager();
	return _instance;
}

void SceneManager::ChangeScene(eScene sceneName)
{
	if (NULL != _scene)
	{
		_scene->Deinit();
		delete _scene;
	}

	switch (sceneName)
	{
	//case eScene::SCENE_LOGO:
	//	_scene = new LogoScene();
	//	break;
	case eScene::SCENE_SELECT:
		_scene = new SelectScene();
		break;
	case eScene::SCENE_TITLE:
		_scene = new TitleScene();
		break;
	case eScene::SCENE_GAME:
		_scene = new GameScene();
		break;
	case eScene::SCENE_RESULT:
		_scene = NULL;
		break;
	}
	if (NULL != _scene)
		_scene->Init();
}

void SceneManager::Update(int deltaTime)
{
	_scene->Update(deltaTime);
}

void SceneManager::Render()
{
	_scene->Render();
}

void SceneManager::KeyUp(int keyCode)
{
	_scene->KeyUp(keyCode);
}

void SceneManager::KeyDown(int keyCode)
{
	_scene->KeyDown(keyCode);
}