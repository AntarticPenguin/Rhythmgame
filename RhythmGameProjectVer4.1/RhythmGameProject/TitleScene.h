#pragma once
#include "Scene.h"

class Sprite;

class TitleScene : public Scene
{
private:
	Sprite* _backgroundSprite;

public:
	TitleScene();
	~TitleScene();

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();

public:
	void TrackDefaultKeyInit();
	void ChangeKey();
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};