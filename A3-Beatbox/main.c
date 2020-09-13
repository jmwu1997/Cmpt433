#include "audioMixer.h"
#include "audioMixer.c"
#include "beat_pattern.h"
#include "beat_pattern.c"
#include "joystickInput.c"
#include "joystickInput.h"
#include "accelerometer.c"
#include "udp.c"
#include "udp.h"



int main()
{
	AudioMixer_init();
	initJoystick();
	Accel_init();

	printf("Audio and Joystick initialized\n");
	
    pthread_t playbackThreadId;
	pthread_t play_patternID;
	pthread_t playjoystickID;
	pthread_t udpID;
	printf("Treads initialized\n");
	
	pthread_create(&udpID,NULL,&udpListener,NULL);
	pthread_create(&play_patternID, NULL,playpatternThread,NULL);
	pthread_create(&playbackThreadId, NULL, playbackThread, NULL);
	pthread_create(&playjoystickID, NULL, joystick_t, NULL);
	printf("Threads created\n");
	
	
    pthread_join(play_patternID, NULL);
	pthread_join(playbackThreadId, NULL);
	pthread_join(playjoystickID, NULL);
	pthread_join(udpID,NULL);
	printf("Threads closed\n");

	Accel_clean();
	AudioMixer_cleanup();
	cleanupJoystick();
	return 0;
}