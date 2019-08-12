#include "ResultScene.h"
#include "SceneManager.h"
#include "DataManager.h"
#include "Font.h"


ResultScene::ResultScene()
{
}

ResultScene::~ResultScene()
{
}

void ResultScene::Init()
{
	{
		Font* font = new Font("arialbd.ttf", 40);
		char text[128];
		sprintf(text, "MISS: %d", DataManager::GetInstance()->GetMissCount());
		font->SetText(text);
		_fontList.push_back(font);
	}
	{
		Font* font = new Font("arialbd.ttf", 40);
		char text[128];
		sprintf(text, "GREAT: %d", DataManager::GetInstance()->GetGreatCount());
		font->SetText(text);
		_fontList.push_back(font);
	}
	{
		Font* font = new Font("arialbd.ttf", 40);
		char text[128];
		sprintf(text, "PERFECT: %d", DataManager::GetInstance()->GetPerfectCount());
		font->SetText(text);
		_fontList.push_back(font);
	}
	{
		Font* font = new Font("arialbd.ttf", 40);
		char text[128];
		sprintf(text, "Max Combo: %d", DataManager::GetInstance()->GetMaxCombo());
		font->SetText(text);
		_fontList.push_back(font);
	}
	{
		Font* font = new Font("arialbd.ttf", 40);
		char text[128];
		sprintf(text, "Result Score: %d", DataManager::GetInstance()->GetScore());
		font->SetText(text);
		_fontList.push_back(font);
	}

	for (int i = 0; i < _fontList.size(); i++)
		_fontList[i]->SetPosition(100, (i + 1) * 100);
}

void ResultScene::Deinit()
{

}

void ResultScene::Update(int deltaTime)
{
	InputProcess();
}

void ResultScene::Render()
{
	for (int i = 0; i < _fontList.size(); i++)
		_fontList[i]->Render();
}

void ResultScene::InputProcess()
{
	/*if(Input::GetInstance().GetKeyDown(SDLK_SPACE))
	{
		SceneManager::GetInstance()->ChangeScene(SCENE_TITLE);
	}*/
}