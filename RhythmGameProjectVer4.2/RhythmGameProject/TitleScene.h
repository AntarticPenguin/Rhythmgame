#pragma once
#include "Scene.h"

class Sprite;
class Font;

class TitleScene : public Scene
{
private:
	Sprite* _backgroundSprite;
	Font* _font;
	char _inputkey[10];

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
	void testPrintKey();
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};