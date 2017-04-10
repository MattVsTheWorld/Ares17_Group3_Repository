#include "SoundManager.h"

// constructor initializes bass and the samples
SoundManager::SoundManager() {
	if (!BASS_Init(-1, 44100, 0, 0, NULL))
		std::cout << "Can't initialize device";

	/*
	Killers Kevin MacLeod (incompetech.com)
	Licensed under Creative Commons: By Attribution 3.0 License
	http://creativecommons.org/licenses/by/3.0/

	Corruption Kevin MacLeod (incompetech.com)
	Licensed under Creative Commons: By Attribution 3.0 License
	http://creativecommons.org/licenses/by/3.0/
	*/

	// pickup
	// https://www.freesound.org/people/juancamiloorjuela/sounds/204318/

	// enemy hit
	// --

	// load sound samples
	samples = new HSAMPLE[10];
	samples[BG] = loadSample("../Ares17_Group3/Sounds/corruption.wav");
	samples[BG_2] = loadSample("../Ares17_Group3/Sounds/killers.wav");
	samples[WILHELM] = loadSample("../Ares17_Group3/Sounds/wilhelm.wav");
	samples[GOOFY] = loadSample("../Ares17_Group3/Sounds/goofy.wav");
	samples[WINCH] = loadSample("../Ares17_Group3/Sounds/winch.wav");
	samples[LASER] = loadSample("../Ares17_Group3/Sounds/laser.wav");
	samples[BULLET] = loadSample("../Ares17_Group3/Sounds/bull.wav");
	samples[PICKUP_HEALTH] = loadSample("../Ares17_Group3/Sounds/pickup_h.wav");
	samples[PAINED_EN] = loadSample("../Ares17_Group3/Sounds/dspepain.wav");
	samples[PAINED_PL] = loadSample("../Ares17_Group3/Sounds/dsnoway.wav");
	
	bgMusicState = false; // set music to playing at first
}

// load sample function from the labs
HSAMPLE SoundManager::loadSample(char * filename) {
	HSAMPLE sam;
	if (sam = BASS_SampleLoad(FALSE, filename, 0, 0, 3, BASS_SAMPLE_OVER_POS))
		std::cout << "sample " << filename << " loaded!" << std::endl;
	else
	{
		std::cout << "Can't load sample";
		exit(0);
	}
	return sam;
}

// plays a sound at a certain volume, on a certain channel
void SoundManager::playSound(HSAMPLE sound, int volume, int channel) {
	ch[channel] = BASS_SampleGetChannel(sound, FALSE);
	BASS_ChannelSetAttribute(ch[channel], BASS_ATTRIB_FREQ, 0);
	BASS_ChannelSetAttribute(ch[channel], BASS_ATTRIB_VOL, volume);
	BASS_ChannelSetAttribute(ch[channel], BASS_ATTRIB_PAN, -1);
	if (!BASS_ChannelPlay(ch[channel], TRUE))
		std::cout << "Can't play sample" << std::endl;
}
// accessor for sound sample
HSAMPLE SoundManager::getSound(int i) {
	return samples[i];
}
// set background music state (on/off)
void SoundManager::setState(bool state) {
	bgMusicState = state;

}
// set volume for a certain channel (used for background volume)
void SoundManager::setVolume(float volume, int channel) {
	BASS_ChannelSetAttribute(ch[channel], BASS_ATTRIB_VOL, volume);
}
// returns state of background music, used to make it so it stop from repeating
bool SoundManager::getState() {
	return bgMusicState;
}