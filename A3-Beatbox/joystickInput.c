#include "joystickInput.h"
#include "audioMixer.h"
#include "beat_pattern.h"


#define EXPORT_PATH "/sys/class/gpio/export"
#define JOYSTICK_UP 26
#define JOYSTICK_DOWN 46
#define JOYSTICK_LEFT 65
#define JOYSTICK_RIGHT 47
#define JOYSTICK_IN 27
#define GPIO_JOYSTICK_UP_PATH "/sys/class/gpio/gpio26/value"
#define GPIO_JOYSTICK_DOWN_PATH "/sys/class/gpio/gpio46/value"
#define GPIO_JOYSTICK_LEFT_PATH "/sys/class/gpio/gpio65/value"
#define GPIO_JOYSTICK_RIGHT_PATH "/sys/class/gpio/gpio47/value"
#define GPIO_JOYSTICK_IN_PATH "/sys/class/gpio/gpio27/value"


pthread_t joystick_tid;
static void joystickInput(void);
static void writeValToFile(char* file_name, int val);
static int readValFromFile(char* file_name);
static int volume;
static int tempo;
static void* joystick_t(void *arg);



int mySleep(int msec)
{
	struct timespec ts;
	int res = -1;
	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;
	do{
		res = nanosleep(&ts,&ts);
	}while(res);

	return res;
}

//Joystick, 0 pressed, 1 unpressed
void initJoystick()
{
	//[up, down, left, right,in]
	int direction_val[5] = {JOYSTICK_UP,JOYSTICK_DOWN,JOYSTICK_LEFT,JOYSTICK_RIGHT,JOYSTICK_IN};
	for(int i = 0; i < 5; i++)
	{
		writeValToFile(EXPORT_PATH,direction_val[i]);
		mySleep(330);
	}
	
	char *direction_path[5] = {GPIO_JOYSTICK_UP_PATH,GPIO_JOYSTICK_DOWN_PATH,GPIO_JOYSTICK_LEFT_PATH,GPIO_JOYSTICK_RIGHT_PATH,GPIO_JOYSTICK_IN_PATH};
	for(int j = 0; j<5; j++)
	{
		writeValToFile(direction_path[j],1);
		mySleep(330);
	}

}

void cleanupJoystick()
{
	pthread_join(joystick_tid,NULL);
}

static void* joystick_t(void* arg)
{
	while(1)
	{
		joystickInput();	
	}
	return NULL;
}



FILE* openFile(char* file_name,char* option)
{
	FILE *file = fopen(file_name,option);
	if(!file){
		printf("cannot open file %s\n", file_name);
		exit(1);	
	}
	return file;
}


static void writeValToFile(char* file_name, int val)
{
	FILE *file = openFile(file_name,"w");
	fprintf(file, "%d", val);
	fclose(file);
}

static int readValFromFile(char* file_name)
{
	FILE *file = openFile(file_name,"r");
	int val;
	fscanf(file,"%d",&val);
	fclose(file);
	return val;
}

static void joystickInput()
{
	if(readValFromFile(GPIO_JOYSTICK_UP_PATH)==0)
	{
		while(1)
		{
			if(readValFromFile(GPIO_JOYSTICK_UP_PATH)==1)
			{
				volume = AudioMixer_getVolume()+5;
				AudioMixer_setVolume(volume);
				mySleep(100);
				break;
			}
		}
	}
	else if(readValFromFile(GPIO_JOYSTICK_DOWN_PATH)==0)
	{
		while(1)
		{
			if(readValFromFile(GPIO_JOYSTICK_DOWN_PATH)==1)
			{
				volume = AudioMixer_getVolume()-5;
				AudioMixer_setVolume(volume);
				mySleep(100);
				break;
			}
		}
	}
	else if(readValFromFile(GPIO_JOYSTICK_LEFT_PATH)==0)
	{
		while(1)
		{
			if(readValFromFile(GPIO_JOYSTICK_LEFT_PATH)==1)
			{
				tempo = AudioMixer_getBPM()-5;
				AudioMixer_setBPM(tempo);
				mySleep(100);
				break;
			}
		}
	}
	else if(readValFromFile(GPIO_JOYSTICK_RIGHT_PATH)==0)
	{
		while(1)
		{
			if(readValFromFile(GPIO_JOYSTICK_RIGHT_PATH)==1)
			{
				tempo = AudioMixer_getBPM()+5;
				AudioMixer_setBPM(tempo);
				mySleep(100);
				break;
			}
		}
	}
	else if(readValFromFile(GPIO_JOYSTICK_IN_PATH)==0)
	{
		while(1)
		{
			if(readValFromFile(GPIO_JOYSTICK_IN_PATH)==1)
			{
				cycle_pattern();
				mySleep(100);
				break;
			}
		}
	}
}


