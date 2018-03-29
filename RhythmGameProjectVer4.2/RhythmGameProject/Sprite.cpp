#include <stdio.h>
#include <string.h>
#include <list>

#include "Sprite.h"
#include "GameSystem.h"
#include "Texture.h"

Sprite::Sprite(const char* filename, bool isLoop)
{
	std::list<Texture*> texturelist;
	
	char filePath[256];
	sprintf(filePath, "../../Resource/%s", filename);
	FILE* fp = fopen(filePath, "r");
	if (NULL == fp)
	{
		printf("파일 열기 실패.\n");
	}
	
	char* textureName;
	float scaleWidth;
	float scaleHeight;

	char buffer[1024];
	char* record = fgets(buffer, sizeof(buffer), fp);

	while (true)
	{
		record = fgets(buffer, sizeof(buffer), fp);
		if (NULL == record)
			break;
		{
			char* token = strtok(record, ",");
			textureName = token;

			token = strtok(NULL, ",");
			scaleWidth = atof(token);

			token = strtok(NULL, ",");
			scaleHeight = atof(token);

			Texture* texture = new Texture(textureName);
			texture->SetScale(scaleWidth, scaleHeight);
			texturelist.push_back(texture);
		}
	}

	fclose(fp);

	_frameMaxCount = texturelist.size();
	_textureFrame = new std::vector<Texture*>();

	std::list<Texture*>::iterator itr;

	for (itr = texturelist.begin(); itr != texturelist.end(); itr++)
	{
		Texture* texture = (*itr);
		_textureFrame->push_back(texture);
	}

	_frame = 0;
	_frameDuration = 0;

	_pivotY = 0.5f;

	_aniSpeed = 1000 / _frameMaxCount;

	_isLoop = isLoop;
	if (_isLoop)
	{
		_isPlay = true;
	}
	else
	{
		_isPlay = false;
	}
}

Sprite::~Sprite()
{

}

void Sprite::Update(int deltaTime)
{
	if (false == _isPlay)
		return;

	_frameDuration += deltaTime;
	if (_aniSpeed < _frameDuration)
	{
		_frameDuration = 0;
		_frame = (_frame + 1) % _frameMaxCount;

		//이펙트를 지워주기 위한 작업
		if (false == _isLoop)
		{
			if (0 == _frame)
			{
				_isPlay = false;
			}
		}
	}

	Texture* texture = _textureFrame->at(_frame);
	texture->SetPosition(_x, _y);
}

void Sprite::Render()
{
	if (false == _isPlay)
		return;

	_textureFrame->at(_frame)->Render();
}

void Sprite::SetPosition(int x, int y)
{
	Texture* texture = _textureFrame->at(_frame);

	_x = x - (texture->GetRenderWidth() * 0.5f);
	_y = y - (texture->GetRenderHeight() * _pivotY);
	texture->SetPosition(_x, _y);
}

void Sprite::SetLength(int length)
{
	for (int i = 0; i < _textureFrame->size(); i++)
	{
		Texture* texture = _textureFrame->at(i);
		texture->SetLength(length);
	}
}

void Sprite::SetPivotY(float pivot)
{
	_pivotY = pivot;
}

void Sprite::Play()
{
	_frame = 0;				//이펙트가 또 터지면 다시 처음부터 재생
	_isPlay = true;
}

void Sprite::Stop()
{
	_frame = 0;
	_isPlay = false;
}

void Sprite::SetLoop(bool isLoop)
{
	_isLoop = isLoop;
}