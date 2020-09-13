#include <stdio.h>
#include <stdlib.h>
#include "sorter.h"
#include "potDriver.h"
#include "segDisplay.h"
#include "sorter.c"
#include "segDisplay.c"
#include "udpListener.h"
#include "udpListener.c"


int main (){


	Led_Init();
	Sorter_startSorting();
	udpListener_new();

	printf("Initilization complete, now start sorting\n");
	while (shutdownnow()==1){ 
		sleep1s();
	}

	updListener_clean();
	Sorter_stopSorting();
	Led_Clean();
	printf("Program now terminated\n");

return 0;



}