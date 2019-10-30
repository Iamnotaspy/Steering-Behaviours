#include "audioManager.h"



audioManager::audioManager()
{
}


audioManager::~audioManager()
{
}

void audioManager::Release()
{
	fxJump->release();
	trackBackground->release();
	audioSystem->release();
	return;
}

bool audioManager::AudioInit()
{
	result = FMOD::System_Create(&audioSystem);
	if (result != FMOD_OK)
	{
		return(false);
	}

	result = audioSystem->init(100, FMOD_INIT_NORMAL | FMOD_INIT_3D_RIGHTHANDED, 0);
	if (result != FMOD_OK)
	{
		return(false);
	}
	//Create/initialise sounds
	
	result = audioSystem->createSound(
		"Resources/Audio/Jump.mp3",
		FMOD_DEFAULT,
		0,
		&fxJump);

	result = audioSystem->createSound(
		"Resources/Audio/Background.mp3",
		FMOD_LOOP_NORMAL,
		0,
		&trackBackground);

	return(true);
}