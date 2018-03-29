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

public:
	void SetPlayTimeTick(float sec);
	int GetPlayTimeTick();
	void SetTrackHeight(int height);
	int GetTrackHeight();
	int GetMusicVolume();
	void SetMusicVolume(int volume);

	//Initializer
private:
	GameSystem();
};