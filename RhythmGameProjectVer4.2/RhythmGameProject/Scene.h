#pragma once
#include <map>

#include "GameObject.h"

enum eTrackButton
{
	TRACK1,
	TRACK2,
	TRACK3,
	TRACK4,
	TRACK5,
};

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
protected:
	std::map<int, eTrackButton> _trackButton;

public:
	virtual void KeyDown(int keyCode);
	virtual void KeyUp(int keyCode);
};