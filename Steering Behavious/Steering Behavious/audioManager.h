#include <fmod.hpp>
#pragma once
class audioManager
{
public:
	audioManager();
	~audioManager();
	bool AudioInit();
	void Release();
	FMOD::System* audioSystem;
	FMOD::Sound* fxJump;
	FMOD::Sound* trackBackground;
	FMOD_RESULT result;
	bool Muted;
	float MuteTimer;	//Short delay until you can mute/unmute
};

