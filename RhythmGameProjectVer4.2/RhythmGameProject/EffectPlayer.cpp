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

	_effectList = new std::vector<Sprite*>();
	_effectList->push_back(missSprite);
	_effectList->push_back(greatSprite);
	_effectList->push_back(perfectSprite);

	_curEffect = NULL;
	_oldEffect = _effectList->at(eEffect::ePERFECT);

	int judgeDeltaLine = 100;

	_effectList->at(eEffect::eMISS)->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() - judgeDeltaLine - 100);
	_effectList->at(eEffect::eGREAT)->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() - judgeDeltaLine - 100);
	_effectList->at(eEffect::ePERFECT)->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() - judgeDeltaLine - 100);

}

EffectPlayer* EffectPlayer::GetInstance()
{
	if (NULL == _instance)
		_instance = new EffectPlayer();
	return _instance;
}

void EffectPlayer::Update(int deltaTime)
{
	_effectList->at(eEffect::eMISS)->Update(deltaTime);
	_effectList->at(eEffect::eGREAT)->Update(deltaTime);
	_effectList->at(eEffect::ePERFECT)->Update(deltaTime);
}

void EffectPlayer::Render()
{
	_effectList->at(eEffect::eMISS)->Render();
	_effectList->at(eEffect::eGREAT)->Render();
	_effectList->at(eEffect::ePERFECT)->Render();
}

void EffectPlayer::Play(eEffect effect)
{
	_curEffect = _effectList->at(effect);
	if (_curEffect != _oldEffect)
		_oldEffect->Stop();
	_curEffect->Play();
	_oldEffect = _curEffect;
}