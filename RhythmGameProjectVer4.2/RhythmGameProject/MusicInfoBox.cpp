#include <stdio.h>
#include "MusicInfoBox.h"
#include "Font.h"

MusicInfoBox::MusicInfoBox(std::string title) : _title(title)
{
	_titleFont = NULL;
}

MusicInfoBox::~MusicInfoBox()
{

}

void MusicInfoBox::Init()
{
	_titleFont = new Font("arialbd.ttf", 40);
	_titleFont->SetText(_title.c_str());
}

void MusicInfoBox::Render()
{
	_titleFont->Render();
}

void MusicInfoBox::SetPosition(int x, int y)
{
	_titleFont->SetPosition(x, y);
}