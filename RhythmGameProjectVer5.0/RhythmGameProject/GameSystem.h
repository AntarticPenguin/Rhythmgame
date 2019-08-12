#pragma once

struct SDL_Renderer;

class GameSystem
{
	//Singleton
private:
	static GameSystem* _instance;
	SDL_Renderer* _sdlRenderer;

	//Window Info
public:
	static GameSystem* GetInstance();
	int GetWindowWidth();
	int GetWindowHeight();
	void Quit();

	//SDL
public:
	void SetRenderer(SDL_Renderer* sdlRenderer);
	SDL_Renderer* GetRenderer();

	//GameInfo
private:
	int _trackHeight;
	int _playTimeTick;
	int _musicVolume;
	int _pauseTime;

public:
	void SetPlayTimeTick(float sec);
	int GetPlayTimeTick();

	void SetTrackHeight(int height);
	int GetTrackHeight();
	void DecreaseTrackHeight();
	void IncreaseTrackHeight();

	int GetMusicVolume();
	void DecreaseVolume();
	void IncreaseVolume();

	int GetPauseTime();
	void SetPauseTime(int pauseTime);

	//Initializer
private:
	GameSystem();
};