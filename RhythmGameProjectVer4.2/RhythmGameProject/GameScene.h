#pragma once
#include "Scene.h"

class Sprite;
class TrackManager;

class GameScene : public Scene
{
private:
	//Sprite* _backgroundSprite;
	TrackManager* _trackManager;
	int _gameDuration;
	bool _isPause;

public:
	GameScene();
	~GameScene();

public:
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();
	void PauseGame();
	void ResumeGame();
	
	//Input
public:
	void KeyDown(int keyCode);
	void KeyUp(int keyCode);
};