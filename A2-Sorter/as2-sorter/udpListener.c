#include "udpListener.h"
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>			
#include <unistd.h>			
#include <pthread.h>
#include <stdbool.h>
#include <sys/socket.h>
#include "sorter.h"

#define PORT 12345
#define PACKET_LEN 1024

//command list
#define HELP "help"
#define COUNT "count"
#define GET_VALUE "get "
#define GET_LENGTH "get length"
#define GET_ARRAY "get array"
#define STOP "stop"

pthread_t pid;
int ret = 1;
void val();
int shutdownnow();
void* udpListener(void* arg);
void program_termination();

// create a new thread
void udpListener_new()
{
	pthread_create(&pid, NULL, &udpListener, NULL);
}

// wait for the thread specified by thread to terminate
void updListener_clean()
{
	pthread_join(pid,NULL);
}

// main function
void* udpListener(void* arg)
{
	//init
	int isRunning = 1;
	char sendBuffer[PACKET_LEN];
	char recvBuffer[PACKET_LEN];
	int recvFlag = 0;
	//construct sock
	struct sockaddr_in sin;
	int socket_descriptor = socket(PF_INET, SOCK_DGRAM, 0);
	if(socket_descriptor == -1){
		printf("Unable to create socket\n");
		exit(-1);
	}
	
	sin.sin_family = AF_INET;                
	sin.sin_addr.s_addr = htonl(INADDR_ANY); 
	sin.sin_port = htons(PORT);
	socklen_t sin_len = sizeof(sin);

	if(bind (socket_descriptor, (struct sockaddr*) &sin, sizeof(sin)) == -1){
		printf("Unable to bind socket\n");
		exit(-1);
	}

	while(isRunning)
	{	

		ssize_t recvBytes = recvfrom(socket_descriptor, recvBuffer, PACKET_LEN, recvFlag, (struct sockaddr *) &sin, &sin_len);
		if(recvBytes == -1){
			printf("Network Failed\n");
			exit(-1);
		}
		//add terminator to end
		recvBuffer[recvBytes - 1] = '\0';
		//printf("bytes recved: %d, detail: %s\n",recvBytes, recvBuffer);
		if(strcmp(recvBuffer, HELP) == 0)
		{
			sprintf(sendBuffer, "Accepted command examples:\n");
			strcat(sendBuffer, "count        -- display number arrays sorted.\n");
			strcat(sendBuffer, "get length   -- display length of array currently being sorted.\n");
			strcat(sendBuffer, "get array    -- display the full array being sorted.\n");
			strcat(sendBuffer, "get 10       -- display the tenth element of array currently being sorted.\n");
			strcat(sendBuffer, "stop         -- cause the server program to end.\n");
			sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
		}
		else if(strcmp(recvBuffer, COUNT) == 0)
		{
			sprintf(sendBuffer, "Number of arrays sorted = %lld. \n",Sorter_getNumberArraysSorted());
			sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
		}
		else if(strcmp(recvBuffer, GET_LENGTH) == 0)
		{

			sprintf(sendBuffer, "Current array length = %d. \n",Sorter_getArrayLength());
			sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
		}
		else if(strcmp(recvBuffer, GET_ARRAY) == 0)
		{
			int length = Sorter_getArrayLength();
			int* data = Sorter_getArrayData(&length); //makes pointer from integer without a cast [-Werror=int-conversion]
			char* array_serialized = malloc(length*sizeof(int));
			memset(array_serialized,0,length*sizeof(int));
			for (int i = 0; i<length; i++)
			{
				sprintf(array_serialized,"%d",data[i]);
				if( i != length - 1){
					strcat(array_serialized,",");
					if( (i + 1) % 10 == 0){
						strcat(array_serialized,"\n");
					}
				}
				else{
					strcat(array_serialized,"\n");
				}
				sendto(socket_descriptor,array_serialized, strnlen(array_serialized, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
			}
			// free the data & output array
			free(data);
			free(array_serialized);
		}
		else if(strncmp(recvBuffer, GET_VALUE,4) == 0)
		{
			char *pch;
			pch = strstr(recvBuffer," ");
			int idx = atoi(pch);
			if(idx < 1)
			{
				sprintf(sendBuffer, "Error: the range of value that are acceptable is between 1 and %d. \n", Sorter_getArrayLength());
				sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
			}
			else if(idx>Sorter_getArrayLength())
			{
				sprintf(sendBuffer, "Error: the range of value that are acceptable is between 1 and %d. \n", Sorter_getArrayLength());
				sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
			}
	
			else
			{
				sprintf(sendBuffer, "Value %d = %d \n", idx, *Sorter_getArrayData(&idx));
				sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
			}
		}
		else if(strncmp(recvBuffer, STOP, strlen(STOP)) == 0)
		{
			sprintf(sendBuffer, "Program terminating. \n");
			sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
			isRunning = 0;
			val();
			
			//close(socket_descriptor);
			
			//exit(0);
		}
		else
		{
			sprintf(sendBuffer, "Unknown command. Please type ‘help’ for command list. \n");
			sendto(socket_descriptor,sendBuffer, strnlen(sendBuffer, PACKET_LEN),0,(struct sockaddr *) &sin,sin_len);
		}

	}
	return 0;	
}

void val(){
	ret = 0;
}
int shutdownnow(){
	return ret;
	
}
