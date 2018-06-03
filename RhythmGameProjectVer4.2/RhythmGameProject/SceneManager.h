#pragma once

enum eScene
{
	//SCENE_LOGO,
	SCENE_TITLE,
	SCENE_SELECT,
	SCENE_LOAD,
	SCENE_GAME,
	SCENE_RESULT,
};

class Scene;

class SceneManager
{
	//Singleton
private:
	static SceneManager* _instance;

public:
	static SceneManager* GetInstance();

public:
	~SceneManager();

private:
	Scene* _scene;

public:
	void ChangeScene(eScene sceneName);
	void Update(int deltaTime);
	void Render();

	//Input
public:
	void KeyUp(int keyCode);
	void KeyDown(int keyCode);

	//Initializer
private:
	SceneManager();
};