///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Playback sounds in real time, allowing multiple simultaneous wave files
// to be mixed together and played without jitter.
#ifndef AUDIO_MIXER_H
#define AUDIO_MIXER_H

#define BASE_DRUM "beatbox-wav-files/100051__menegass__gui-drum-bd-hard.wav"
#define HI_BAT "beatbox-wav-files/100053__menegass__gui-drum-cc.wav"
#define SNARE "beatbox-wav-files/100059__menegass__gui-drum-snare-soft.wav"
#define SPLASH "beatbox-wav-files/100060__menegass__gui-drum-splash-hard.wav"
#define CO "beatbox-wav-files/100055__menegass__gui-drum-co.wav"
#define TOM "beatbox-wav-files/100062__menegass__gui-drum-tom-hi-hard.wav"
#define AUDIOMIXER_MAX_VOLUME 100
#define MAX_BPM 300
#define DEFAULT_BPM 120
#define MIN_BPM 40
#define MAX_VOLUME 100
#define DEFAULT_VOLUME 80
#define MIN_VOLUME 0

typedef struct {
	int numSamples;
	short *pData;
} wavedata_t;

wavedata_t playlist[6];



// init() must be called before any other functions,
// cleanup() must be called last to stop playback threads and free memory.
void AudioMixer_init(void);
void AudioMixer_cleanup(void);

// Read the contents of a wave file into the pSound structure. Note that
// the pData pointer in this structure will be dynamically allocated in
// readWaveFileIntoMemory(), and is freed by calling freeWaveFileData().
void AudioMixer_readWaveFileIntoMemory(char *fileName, wavedata_t *pSound);
void AudioMixer_freeWaveFileData(wavedata_t *pSound);

// Queue up another sound bite to play as soon as possible.
void AudioMixer_queueSound(wavedata_t *pSound);

// Get/set the volume.
// setVolume() function posted by StackOverflow user "trenki" at:
// http://stackoverflow.com/questions/6787318/set-alsa-master-volume-from-c-code
int  AudioMixer_getVolume();
void AudioMixer_setVolume(int newVolume);

void Sound_init(void);
void bpmsleep();
void AudioMixer_setBPM(int newBPM);
int AudioMixer_getBPM();
void cycle_pattern();
int get_pattern();
void set_pattern(int number);
void play_sound(int number);
void Sound_clean(void);
#endif
