#pragma once

class GameObject
{
public:
	GameObject();
	~GameObject();

public:
	virtual void Init() = 0;
	virtual void Deinit() = 0;
	virtual void Update(int deltaTime) = 0;
	virtual void Render() = 0;
};
