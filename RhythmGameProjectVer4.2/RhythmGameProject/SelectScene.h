#pragma once
#include <vector>
#include <string>

#include "Scene.h"

class Sprite;
class Font;

class SelectScene : public Scene
{
private:
	Sprite* _backgroundSprite;
	std::vector<std::string> _musicList;
	std::vector<Font*> _fontList;

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

	//Input
public:
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};