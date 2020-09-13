//udp.c
#include "udp.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>			
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include "audioMixer.h"
#include "beat_pattern.h"

#define PORT 12345
#define PACKET_LEN 1024

//command list
#define HELP "help"
#define SOUND "sound"
#define MODE "mode"
#define VOLUME "volume"
#define TEMPO "tempo"
#define UPTIME "uptime"
#define UPTIME_PATH "/proc/uptime"

static pthread_t pid;
static void *udpListener(void *arg);

//create a new thread
 void initUdp()
 {
     pthread_create(&pid,NULL,&udpListener,NULL);
 }

 //wait for the thread specified by thread to terminate
 void cleanupUdp()
 {
     pthread_join(pid,NULL);
 }

 static int getIndex(char *string)
 {
     int x = -1;
     sscanf(string, "%*s%d",&x);
     return x;
 }

 FILE* openF(char* file_name,char* option)
 {
    FILE *file = fopen(file_name,option);
    if(!file){
        printf("cannot open file %s\n", file_name);
	exit(1);
    }
    return file;
 } 
 
 static char* readFromFile(char* file_name)
 {
    FILE *file = openF(file_name,"r");
    char* val = malloc(50);
    fscanf(file,"%s",val);
    fclose(file);
    return val;
 }

 static void *udpListener(void *arg)
 {
    //Buffer to hold packet data
    char recvBuffer[PACKET_LEN];
    char sendBuffer[PACKET_LEN];
    int isRunning = 1;
    
    //construct sock
    struct sockaddr_in sin;
    int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);
    if(socket_descriptor == -1)
    {
        printf("Unable to create socket..\n");
        exit(-1);
    }
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(PORT);
    socklen_t sin_len = sizeof(sin);
    
    if(bind (socket_descriptor, (struct sockaddr *) &sin, sizeof(sin)) == -1)
    {
		printf("Unable to bind socket\n");
		exit(-1);
	}
    
    

    while(isRunning)
    {
        int input_idx; 
        ssize_t recvBytes = recvfrom(socket_descriptor,recvBuffer,PACKET_LEN,0,(struct sockaddr *) &sin,&sin_len);
        if(recvBytes == -1)
        {
            printf("Network failed..\n");
            exit(-1);
        }
        //add terminator to end
        recvBuffer[recvBytes - 1] = '\0';
        if(strcmp(recvBuffer,HELP) == 0)
        {
            sprintf(sendBuffer, "Accepted command examples:\n");
			strcat(sendBuffer, "sound 1      -- display the sound 1.\n");
            strcat(sendBuffer, "mode 1       -- set the beat mode to 1.\n");
			strcat(sendBuffer, "volume 80    -- set the volume to 80.\n");
			strcat(sendBuffer, "tempo 120    -- set the tempo to 120.\n");
			sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
			
        }
	else if(strcmp(recvBuffer,UPTIME) == 0)
	{
	    
	    sprintf(sendBuffer, "time %s\n", readFromFile(UPTIME_PATH));
	    sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
	}
        else if(strncmp(recvBuffer,SOUND,strlen(SOUND)) == 0)
		{
	    
            input_idx = getIndex(recvBuffer);
			if (input_idx<0 || input_idx>2)
            {
                sprintf(sendBuffer,"Error: sound must be between 0 and 2.\n");
                sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
		//memset(&sin,0,sizeof(sin));
                
		    }
            else 
            {
        		if (input_idx==0){ play_sound(0);}
        		else if (input_idx==1){ play_sound(1);}
        		else if (input_idx==2){ play_sound(2);}
        		//else if (input_idx==3){ play_sound(3);}
         		//else if (input_idx==4){ play_sound(4);} 
                sprintf(sendBuffer,"sound %d\n",input_idx);
                sendto(socket_descriptor,sendBuffer,strnlen(sendBuffer,PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
                input_idx = 0;
            }
        }
		else if(strncmp(recvBuffer,MODE,strlen(MODE)) == 0)
		{
	  
            input_idx = getIndex(recvBuffer);
            if (input_idx<0 || input_idx>2)
            {
                sprintf(sendBuffer,"Error: mode must be between 0 and 2.\n");
                sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
		memset(&sin,0,sizeof(sin));
               
            }   
            else 
            {
		        set_pattern(input_idx);
                sprintf(sendBuffer,"mode %d\n",get_pattern());
                sendto(socket_descriptor,sendBuffer,strnlen(sendBuffer,PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
                input_idx = 0;
            }   
	   
		}
		else if(strncmp(recvBuffer,VOLUME,strlen(VOLUME)) == 0)
		{
	  
            input_idx = getIndex(recvBuffer);
            if (input_idx<0 || input_idx>100)
            {
                sprintf(sendBuffer,"Error: volume must be between 0 and 100. \n");
                sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
		memset(&sin,0,sizeof(sin));
                
            }   
            else 
            {
                AudioMixer_setVolume(input_idx);
                sprintf(sendBuffer,"volume %d\n",AudioMixer_getVolume());
                sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
		input_idx = 0;
            }   
		}
		else if(strncmp(recvBuffer,TEMPO,strlen(TEMPO)) == 0)
		{
	
            input_idx = getIndex(recvBuffer);
			if(input_idx < 40 || input_idx > 300)
			{
				sprintf(sendBuffer, "Error: tempo must be between 40 and 300. \n");
				sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
				memset(&sin,0,sizeof(sin));
                
			}
			else
			{
                AudioMixer_setBPM(input_idx);
				sprintf(sendBuffer, "tempo %d\n", AudioMixer_getBPM());
				sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
				input_idx = 0;
                
			}
		}
		else
		{
			sprintf(sendBuffer, "Unknown command. Please type ‘help’ for command list. \n");
			sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);

		}

	}
	close(socket_descriptor);
	return 0;	
}


