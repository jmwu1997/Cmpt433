#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <malloc.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "sorter.h"

int *arr = 0;
int *copy = 0;
int curr_size = 20;
int next_size = 20;
unsigned long long int arr_count = 0;
_Bool true_val=true;
static pthread_t sorter_id;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void Sorter_startSorting(void) {
	pthread_create(&sorter_id, NULL, &sorter, NULL);
}

void *sorter() {

	
	while(true_val){
		pthread_mutex_lock(&mutex);
		next_size = getarraysize();
		//printf("%d", next_size);
		arr = malloc(sizeof(int)*next_size);
		if (arr == NULL){printf("Malloc failed");}
		curr_size=next_size;
		generate_array(arr,curr_size);

		pthread_mutex_unlock(&mutex);
		//printf("\n");
		scramble(arr,curr_size);
		//Sorter_getArrayData(&curr_size);
		//print_array(arr,curr_size);
		//print_array(arr,curr_size);
		//printf("\n");
		bubbleSort(arr,curr_size);
		Sorter_getArrayData(&curr_size);
		//print_array(arr,curr_size);
		//print_array(arr,curr_size);
		//printf("\n");
		//pthread_mutex_unlock(&mutex);
		printf("Current array size: %d    ",curr_size);
		pthread_mutex_lock(&mutex);
		free(arr);
		curr_size=0;
		arr_count++;
		pthread_mutex_unlock(&mutex);
		printf("Array sorted count: %lld \n", arr_count);

	
	}
    free(copy);
    
	return NULL;
}

int getarraysize(){
	//sleep1s();
	int val = 10;
	int newSize=0;
	int a2d[10] = {0,500,1000,1500,2000,2500,3000,3500,4000,4100} ;
	int arrSize[10] ={1,20,60,120,250,300,500,800,1200,2100} ;
	int reading = getVoltage0Reading();
	for (int i=0; i<val; i++){
		if (reading < a2d[i]){
			newSize = arrSize[i];
			return newSize;
		}
		
	}

	return 0;
}





int* Sorter_getArrayData(int *length) {
	pthread_mutex_lock(&mutex);
	*length = curr_size;
	//copy init
	copy = malloc(sizeof(int)*next_size);
    //copy copies arr
	memcpy(copy, arr, sizeof(int)*next_size);
	pthread_mutex_unlock(&mutex);
	return copy;
}


void scramble(int arr[], int n){
	if (n > 1) 
    {
        int i;
        int j;
        int temp;

        for (i = 0; i < n - 1; i++) 
        {
          //in range from 1-max
          j = i + rand() / (RAND_MAX / (n - i) + 1);

          //swap with the random index
          temp = arr[j];
          arr[j] = arr[i];
          arr[i] = temp;
        }
    }
}

// An optimized version of Bubble Sort
 void bubbleSort(int arr[], int size)
   {
       int i = size;
       int j;
       int temp;
 
       while(i > 0)
       {
          for(j = 0; j < i - 1; j++)
          {
              if(arr[j] > arr[j + 1])
              {   
              	  //swap
              	  temp = arr[j];
                  arr[j] = arr[j + 1];
                  arr[j + 1] = temp;
              }
          }
          //count the total size exit loop when 0
          i--;
      }
      
 
  }

void Sorter_setArraySize(int newSize){
	next_size = newSize;
	//printf("set size = %d\n", newSize);
}

int Sorter_getArrayLength(void){
	//printf("length %d\n", curr_size);
	return next_size;
}

void Sorter_stopSorting(void) {
	true_val=false;
	pthread_join(sorter_id,NULL);
}

long long Sorter_getNumberArraysSorted(void){
	
	return arr_count;
}

//
void generate_array(int arr[], int n){
	for(int i = 0; i < n; i++) {
			arr[i] = i+1;
		}
}

void print_array(int arr[], int n){
	for(int i = 0; i < n; i++) {
			printf("%d ", arr[i]);
		}
}