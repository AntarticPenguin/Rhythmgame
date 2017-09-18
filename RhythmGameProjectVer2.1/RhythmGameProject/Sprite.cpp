#include <stdio.h>
#include <string.h>

#include "Sprite.h"
#include "GameSystem.h"
#include "Texture.h"
#include "DLinkedList.h"

Sprite::Sprite(const char* filename, bool isLoop)
{
	DLinkedList<Texture*> linkedlist;
	
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
			linkedlist.Append(texture);
		}
	}

	fclose(fp);

	_frameMaxCount = linkedlist.Count();
	_textureFrame = new Array<Texture*>(_frameMaxCount);

	int index = 0;
	DLinkedListIterator<Texture*> itr = linkedlist.GetIterator();
	for (itr.Start(); itr.Valid(); itr.Forth())
	{
		Texture* texture = itr.Item();
		_textureFrame->Set(index, texture);
		index++;
	}

	_frame = 0;
	_frameDuration = 0;
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

void Sprite::Render()
{
	if (false == _isPlay)
		return;

	_textureFrame->Get(_frame)->Render();
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

	Texture* texture = _textureFrame->Get(_frame);
	texture->SetPosition(_x, _y);
}

void Sprite::SetPosition(int x, int y)
{
	_x = x;
	_y = y;

	Texture* texture = _textureFrame->Get(_frame);
	texture->SetPosition(_x, _y);
}

void Sprite::Play()
{
	_frame = 0;				//이펙트가 또 터지면 다시 처음부터 재생
	_isPlay = true;
}