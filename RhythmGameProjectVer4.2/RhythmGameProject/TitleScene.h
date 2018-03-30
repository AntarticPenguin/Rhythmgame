#pragma once
#include "Scene.h"

class Sprite;
class Font;

class TitleScene : public Scene
{
private:
	Sprite* _backgroundSprite;
	Font* _trackKeyFont;
	char _inputkeyTest[50];

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