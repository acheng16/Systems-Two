#include <pthread.h>
#include "buffer.h"
#include "consumer_producer.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

//initialize globals
pthread_mutex_t mutex; //mutex
sem_t full, empty;  //full and empty semaphores
pthread_attr_t attr; //attributes for pthread_create

//Init Producer & Consumer
void *producer(void *param);
void *consumer(void *param);

int main(int argc, char *argv[]){
	int i;

	/* expect four args or end program */
	if (argc!= 4) {
		fprintf(stderr, "Incorrect number of arguements are passed in, correct format <executable> <main_sleep_time> <number of producer threads> <number of consumer threads>");
		exit(EXIT_FAILURE);
	}
	int sleepTime = atoi(argv[1]);  // sleep time for main
	int pNum = atoi(argv[2]);  // Number of Producer Threads
	int cNum = atoi(argv[3]); // Number of Consumer Threads
	
	pthread_t ptid[pNum]; // array of producer threads
	

	pthread_mutex_init(&mutex, NULL); //initalize mutex
	pthread_attr_init(&attr);  //initialize thread attributes to NULL
	sem_init(&empty, 0, BUFFER_SIZE); // Initialize the empty sempaphore
	
	sem_init(&full, 0, 0);  // initialize the full semaphore

	// Create producer threads
	for (i = 0; i< pNum; i++) {
		/* create producer threads */
		pthread_create(&ptid[i], &attr, producer, &i);
		printf("Hi, I am producer: %d, my thread id = %d\n", i, ptid[i]);
		sleep(1);
	}

	// Consumer threads in array
	pthread_t ctid[cNum];	
	pthread_attr_init(&attr);	

	// Create consumer threads
	for (i = 0; i< cNum; i++) {
		/* create consumer threads */
		pthread_create(&ctid[i], &attr, consumer, &i);
		printf("Hi, I am consumer: %d, my thread id = %d\n", i, ctid[i]);
		sleep(1);
	}

	sleep(sleepTime); // sleep time for main
	exit(0);
}
