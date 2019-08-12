#pragma once
#include <vector>
#include <string>

#include "Scene.h"

class Sprite;
class MusicInfoBox;

class SelectScene : public Scene
{
private:
	Sprite* _backgroundSprite;

public:
	SelectScene();
	~SelectScene();

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();

	//UI
private:
	std::vector<std::string> _musicList;
	std::vector<MusicInfoBox*> _viewList;

	Sprite* _selectBox;
	int _selectPosX;
	int _selectPosY;
	int _endPosY;

	int _selectIndex;
	int _startViewIndex;
	int _endViewIndex;

public:
	void InitMusicList();
	void InitViewList();
	void SelectUp();
	void SelectDown();

	//Input
public:
	void InputProcess() override;
};