#pragma once
#include "GameObject.h"

class Scene : public GameObject
{
public:
	Scene();
	~Scene();

public:
	//가상함수
	void Init();
	void Deinit();
	void Update(int deltaTime);
	void Render();

	//Input
public:
	virtual void KeyDown(int keyCode);
	virtual void KeyUp(int keyCode);
};