#include "beat_pattern.h"
#include "audioMixer.h"
#include "accelerometer.h"
void pattern1()
{
	AudioMixer_queueSound(&playlist[0]);
	AudioMixer_queueSound(&playlist[1]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	AudioMixer_queueSound(&playlist[2]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	AudioMixer_queueSound(&playlist[1]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	AudioMixer_queueSound(&playlist[2]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	bpmsleep();
}

void pattern2(){
	AudioMixer_queueSound(&playlist[0]);
	AudioMixer_queueSound(&playlist[1]);
	AudioMixer_queueSound(&playlist[2]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[1]);
	AudioMixer_queueSound(&playlist[2]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	AudioMixer_queueSound(&playlist[1]);
	AudioMixer_queueSound(&playlist[2]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[0]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[1]);
	AudioMixer_queueSound(&playlist[2]);
	bpmsleep();

	AudioMixer_queueSound(&playlist[4]);
	bpmsleep();
}

void pattern0(){
}

void play_sound(int number){
	if(number==0){
        AudioMixer_queueSound(&playlist[0]);
        bpmsleep();
    }
    else if(number==1){
        AudioMixer_queueSound(&playlist[1]);
        bpmsleep();
    }
    else if(number==2){
        AudioMixer_queueSound(&playlist[2]);
        bpmsleep();
    }
 
}



void* playpatternThread(void* arg)
{
	Sound_init();
	while(!stopping){
		
		int *p = get_accel();
		//x 0 stay left -1 right 1
		//y 0 stay in -1 out 1
		//z 1 stay side 0 down -1 
		//x
		if( *(p) == -1 || *(p) == 1 ){
			play_sound(0);
		}
		//y
		else if( *(p+1) == -1 || *(p+1) == 1 ){
			play_sound(1);
		}
		//z
		else if( *(p+2) == 0 || *(p+2) == -1 ){
			play_sound(2);	
		}
		else if(get_pattern()==0){
			pattern0();
		}
		else if(get_pattern()==1){
			pattern1();
		}
		else if(get_pattern()==2){
			pattern2();
		}

		



	}

	pthread_exit(0);
}