#ifndef SOUNDMANAGER
#define SOUNDMANAGER
#include "bass.h"
#include <iostream>
#include <vector>
// sound manager class
// simple impelementations of lab usage of bass into a class
// more details in .cpp file

enum {
	BG,
	WILHELM,
	GOOFY,
	WINCH,
	LASER,
	BULLET,
	BG_2,
	PICKUP_HEALTH,
	PAINED_EN,
	PAINED_PL
};

class SoundManager
{
public:
	SoundManager(); // constructor initializes bass and loads sound files
	HSAMPLE loadSample(char * filename); // function used to load the samples
	void playSound(HSAMPLE sound, int volume, int channel); // function used to play sounds
															// accessor for sound sample
	HSAMPLE getSound(int i);
	// set background music state (on/off)
	void setState(bool state);
	// set volume for a certain channel (used for background volume)
	void setVolume(float volume, int channel);
	// returns state of background music, used to make it so it stop from repeating
	bool getState();

	// Delete current BG so you can add a new one
	void stopBG() {
		BASS_SampleStop(samples[BG]);
		BASS_SampleStop(samples[BG_2]);
	//	BASS_SampleStop(samples[BG_3]);
	}
private:
	HSAMPLE *samples = NULL;
	bool bgMusicState;
	HCHANNEL ch[4];
};

#endif