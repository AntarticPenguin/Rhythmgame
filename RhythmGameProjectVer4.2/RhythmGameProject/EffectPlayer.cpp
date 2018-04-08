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
	_oldEffect = _effectList->at(eJudge::PERFECT);

	int judgeDeltaLine = 100;

	_effectList->at(eJudge::MISS)->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() - judgeDeltaLine - 100);
	_effectList->at(eJudge::GREAT)->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() - judgeDeltaLine - 100);
	_effectList->at(eJudge::PERFECT)->SetPosition(GameSystem::GetInstance()->GetWindowWidth() / 2, GameSystem::GetInstance()->GetWindowHeight() - judgeDeltaLine - 100);

}

EffectPlayer* EffectPlayer::GetInstance()
{
	if (NULL == _instance)
		_instance = new EffectPlayer();
	return _instance;
}

void EffectPlayer::Update(int deltaTime)
{
	_effectList->at(eJudge::MISS)->Update(deltaTime);
	_effectList->at(eJudge::GREAT)->Update(deltaTime);
	_effectList->at(eJudge::PERFECT)->Update(deltaTime);
}

void EffectPlayer::Render()
{
	_effectList->at(eJudge::MISS)->Render();
	_effectList->at(eJudge::GREAT)->Render();
	_effectList->at(eJudge::PERFECT)->Render();
}

void EffectPlayer::Play(eJudge effect)
{
	_curEffect = _effectList->at(effect);
	if (_curEffect != _oldEffect)
		_oldEffect->Stop();
	_curEffect->Play();
	_oldEffect = _curEffect;
}

void EffectPlayer::Stop()
{
	_curEffect->Stop();
}