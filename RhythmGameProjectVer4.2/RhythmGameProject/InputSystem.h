#pragma once
#include <map>

enum eTrackButton
{
	TRACK1,
	TRACK2,
	TRACK3,
	TRACK4,
	TRACK5,
};

class InputSystem
{
private:
	static InputSystem* _instance;
	std::map<int, eTrackButton> _trackButton;

public:
	static InputSystem* GetInstance();
	void Init();
	void Update(int deltaTime);

public:
	void ChangeKey();
	std::map<int, eTrackButton>& GetTrackButton();
	~InputSystem();

private:
	InputSystem();
};