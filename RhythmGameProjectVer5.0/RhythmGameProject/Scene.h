#pragma once
#include "GameObject.h"
#include "Input.h"

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
	virtual void InputProcess() = 0;
};