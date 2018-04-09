#pragma once
#include "Scene.h"

class Sprite;

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

	//Input
public:
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};