#pragma once
#include "Scene.h"

class Sprite;

class LogoScene : public Scene
{
private:
	Sprite* _backgroundSprite;

public:
	LogoScene();
	~LogoScene();

	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();

	//Input
public:
	virtual void KeyDown(int keyCode);
	virtual void KeyUp(int keyCode);
};