#include "SoundManager.h"

// constructor initializes bass and the samples
SoundManager::SoundManager() {
	if (!BASS_Init(-1, 44100, 0, 0, NULL))
		std::cout << "Can't initialize device";

	// load sound samples
	samples = new HSAMPLE[4];
	//samples[1] = loadSample("pickup.wav");
	//samples[2] = loadSample("Background.wav");
	bgMusicState = true; // set music to playing at first
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
	BASS_ChannelSetAttribute(ch[channel], BASS_ATTRIB_VOL, 1.0);
	BASS_ChannelSetAttribute(ch[channel], BASS_ATTRIB_PAN, -1);
	if (!BASS_ChannelPlay(ch[channel], FALSE))
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