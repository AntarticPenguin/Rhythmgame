#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"
#include "GameSystem.h"
#include "DataManager.h"
#include "EffectPlayer.h"
#include "TrackManager.h"
#include "Track.h"
#include "Note.h"
#include "Sprite.h"
#include "Font.h"

TrackManager::TrackManager()
{
	_trackList = NULL;
	_combofont = NULL;
	_scorefont = NULL;
}

TrackManager::~TrackManager()
{
	Deinit();
}

void TrackManager::Init()
{
	_combofont = new Font("arialbd.ttf", 40);
	_combofont->SetPosition(GameSystem::GetInstance()->GetWindowWidth() - 250, 150);

	_scorefont = new Font("arialbd.ttf", 32);
	_scorefont->SetPosition(0, 0);
	char text[50];
	sprintf(text, "SCORE %08d", DataManager::GetInstance()->GetScore());
	_scorefont->SetText(text);

	int judgeDeltaLine = 100;

	{
		Track* track1 = new Track();
		track1->Init();
		Track* track2 = new Track();
		track2->Init();
		Track* track3 = new Track();
		track3->Init();
		Track* track4 = new Track();
		track4->Init();

		track1->SetPosition((GameSystem::GetInstance()->GetWindowWidth() / 2) - 155, GameSystem::GetInstance()->GetWindowHeight());
		track2->SetPosition((GameSystem::GetInstance()->GetWindowWidth() / 2) - 52, GameSystem::GetInstance()->GetWindowHeight());
		track3->SetPosition((GameSystem::GetInstance()->GetWindowWidth() / 2) + 52, GameSystem::GetInstance()->GetWindowHeight());
		track4->SetPosition((GameSystem::GetInstance()->GetWindowWidth() / 2) + 155, GameSystem::GetInstance()->GetWindowHeight());

		_trackList = new Array<Track*>(4);
		_trackList->Set(eTrackNum::TRACK01, track1);
		_trackList->Set(eTrackNum::TRACK02, track2);
		_trackList->Set(eTrackNum::TRACK03, track3);
		_trackList->Set(eTrackNum::TRACK04, track4);
	}

	int playTime = GameSystem::GetInstance()->GetPlayTimeTick();
	int deltaTick = 0;
	int durationTick = 0;
	for (int noteTick = 875; noteTick < playTime; )
	{
		int randValue = rand() % 4;
		switch (randValue)
		{
		case 0:
			deltaTick = 125;
			break;
		case 1:
			deltaTick = 250;
			break;
		case 2:
			deltaTick = 375;
			break;
		case 3:
			deltaTick = 500;
			break;
		}

		noteTick += deltaTick;

		randValue = rand() % 4;
		switch (randValue)
		{
		case 0:
		case 1:
			durationTick = 0;
			break;
		case 2:
			durationTick = 500;
			break;
		case 3:
			durationTick = 1000;
			break;
		default:
			durationTick = 0;
		}

		float sec = (float)noteTick / 1000.0f;
		float duration = (float)durationTick / 1000.0f;

		Note* note = new Note(sec, duration, judgeDeltaLine);
		
		{
			Track* pTrack = NULL;
			switch (randValue)
			{
			case 0:
				pTrack = _trackList->Get(eTrackNum::TRACK01);
				note->SetXPosition((GameSystem::GetInstance()->GetWindowWidth() / 2) - 155);
				break;
			case 1:
				pTrack = _trackList->Get(eTrackNum::TRACK02);
				note->SetXPosition((GameSystem::GetInstance()->GetWindowWidth() / 2) - 52);
				break;
			case 2:
				pTrack = _trackList->Get(eTrackNum::TRACK03);
				note->SetXPosition((GameSystem::GetInstance()->GetWindowWidth() / 2) + 52);
				break;
			case 3:
				pTrack = _trackList->Get(eTrackNum::TRACK04);
				note->SetXPosition((GameSystem::GetInstance()->GetWindowWidth() / 2) + 155);
				break;
			}
			note->Init();
			pTrack->GetNoteList().Append(note);
		}
		noteTick += durationTick;
	}
}

void TrackManager::Deinit()
{
	if (NULL != _trackList)
	{
		for (int i = 0; i < _trackList->GetSize(); i++)
		{
			delete _trackList->Get(i);
		}
		_trackList = NULL;
	}
	
	if (NULL != _combofont)
	{
		delete _combofont;
		_combofont = NULL;
	}

	if (NULL != _scorefont)
	{
		delete _scorefont;
		_scorefont = NULL;
	}
}

void TrackManager::Update(int deltaTime)
{

	for (int i = 0; i < _trackList->GetSize(); i++)
		_trackList->Get(i)->Update(deltaTime);

	EffectPlayer::GetInstance()->Update(deltaTime);
}

void TrackManager::Render()
{
	for (int i = 0; i < _trackList->GetSize(); i++)
		_trackList->Get(i)->Render();


	EffectPlayer::GetInstance()->Render();
	_combofont->Render();
	_scorefont->Render();
}

void TrackManager::KeyDown(int keyCode)
{
	switch (keyCode)
	{
	case SDLK_d:
		_trackList->Get(eTrackNum::TRACK01)->KeyDown();
		break;
	case SDLK_f:
		_trackList->Get(eTrackNum::TRACK02)->KeyDown();
		break;
	case SDLK_j:
		_trackList->Get(eTrackNum::TRACK03)->KeyDown();
		break;
	case SDLK_k:
		_trackList->Get(eTrackNum::TRACK04)->KeyDown();
		break;
	}

	{
		char text[50];
		sprintf(text, "COMBO %d", DataManager::GetInstance()->GetCombo());
		_combofont->SetText(text);
	}

	{
		char text[50];
		sprintf(text, "SCORE %08d", DataManager::GetInstance()->GetScore());
		_scorefont->SetText(text);
	}
}

void TrackManager::KeyUp(int keyCode)
{
	switch (keyCode)
	{
	case SDLK_d:
		_trackList->Get(eTrackNum::TRACK01)->KeyUp();
		break;
	case SDLK_f:
		_trackList->Get(eTrackNum::TRACK02)->KeyUp();
		break;
	case SDLK_j:
		_trackList->Get(eTrackNum::TRACK03)->KeyUp();
		break;
	case SDLK_k:
		_trackList->Get(eTrackNum::TRACK04)->KeyUp();
		break;
	}

	{
		char text[50];
		sprintf(text, "COMBO %d", DataManager::GetInstance()->GetCombo());
		_combofont->SetText(text);
	}

	{
		char text[50];
		sprintf(text, "SCORE %08d", DataManager::GetInstance()->GetScore());
		_scorefont->SetText(text);
	}
}