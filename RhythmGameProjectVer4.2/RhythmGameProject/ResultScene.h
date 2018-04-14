#pragma once
#include <vector>
#include "Scene.h"

class Font;

class ResultScene : public Scene
{
private:
	std::vector<Font*> _fontList;

public:
	ResultScene();
	~ResultScene();

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