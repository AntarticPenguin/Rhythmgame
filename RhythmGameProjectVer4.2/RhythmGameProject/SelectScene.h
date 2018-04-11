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
	std::vector<std::string> _musicList;
	std::vector<MusicInfoBox*> _viewList;

	//test
	Sprite* _selectBox;
	int _selectPosX;
	int _selectPosY;

	int _viewIndex;
	int _startViewIndex;
	int _endViewIndex;

public:
	SelectScene();
	~SelectScene();

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();

public:
	void InitMusicList();
	void InitViewList();

	//Input
public:
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};