#pragma once
#include <string>

class Font;

class MusicInfoBox
{
private:
	std::string _title;
	Font* _titleFont;

public:
	MusicInfoBox(std::string title);
	~MusicInfoBox();

public:
	void Init();
	void Render();
	void SetPosition(int x, int y);
};

