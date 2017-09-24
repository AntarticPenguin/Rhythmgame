#include <stdio.h>
#include "EffectPlayer.h"
#include "GameSystem.h"
#include "Sprite.h"

EffectPlayer* EffectPlayer::_instance = NULL;

EffectPlayer::EffectPlayer()
{
	Sprite* missSprite = new Sprite("misseffectData.csv", false);
	Sprite* greatSprite = new Sprite("greateffectData.csv", false);
	Sprite* perfectSprite = new Sprite("perfecteffectData.csv", false);

	_effectList = new Array<Sprite*>(3);
	_effectList->Set(eEffect::eMISS, missSprite);
	_effectList->Set(eEffect::eGREAT, greatSprite);
	_effectList->Set(eEffect::ePERFECT, perfectSprite);

	_curEffect = NULL;
	_oldEffect = _effectList->Get(eEffect::ePERFECT);;

	int judgeDeltaLine = 100;

	_effectList->Get(eEffect::eMISS)->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() - judgeDeltaLine - 100);
	_effectList->Get(eEffect::eGREAT)->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() - judgeDeltaLine - 100);
	_effectList->Get(eEffect::ePERFECT)->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() - judgeDeltaLine - 100);
}

EffectPlayer* EffectPlayer::GetInstance()
{
	if (NULL == _instance)
		_instance = new EffectPlayer();
	return _instance;
}

void EffectPlayer::Update(int deltaTime)
{
	_effectList->Get(eEffect::eMISS)->Update(deltaTime);
	_effectList->Get(eEffect::eGREAT)->Update(deltaTime);
	_effectList->Get(eEffect::ePERFECT)->Update(deltaTime);
}

void EffectPlayer::Render()
{
	_effectList->Get(eEffect::eMISS)->Render();
	_effectList->Get(eEffect::eGREAT)->Render();
	_effectList->Get(eEffect::ePERFECT)->Render();
}

void EffectPlayer::Play(int code)
{
	switch (code)
	{
	case eEffect::ePERFECT:
		_curEffect = _effectList->Get(eEffect::ePERFECT);
		if (_curEffect != _oldEffect)
			_oldEffect->Stop();
		_curEffect->Play();
		_oldEffect = _curEffect;
		break;
	case eEffect::eGREAT:
		_curEffect = _effectList->Get(eEffect::eGREAT);
		if (_curEffect != _oldEffect)
			_oldEffect->Stop();
		_curEffect->Play();
		_oldEffect = _curEffect;
		break;
	case eEffect::eMISS:
		_curEffect = _effectList->Get(eEffect::eMISS);
		if (_curEffect != _oldEffect)
			_oldEffect->Stop();
		_curEffect->Play();
		_oldEffect = _curEffect;
		break;
	}
}