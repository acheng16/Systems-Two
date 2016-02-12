#include <pthread.h>
#include "buffer.h"
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

//initialize globals
int pcounter=0; //counter for producer
int ccounter=0; //counter for consumer
pthread_mutex_t mutex; //mutex
sem_t full, empty;  //full and empty semaphores
buffer_item buffer[BUFFER_SIZE]; //Buffer from buffer.h
pthread_attr_t attr; //attributes for pthread_create

//Init Producer & Consumer
void *producer(void *param);
void *consumer(void *param);

//insert item to Buffer (Circular Queue)
int insert_item(buffer_item item) {
	
	//if we have inserted BUFFER_SIZE items, next item will be inserted at the start again
	if (pcounter == BUFFER_SIZE) {
		pcounter = 0;
	}	
	buffer[pcounter] = item; //set buffer position equal to item we are inserting
	pcounter++; //inc counter
	/* Insert an item into buffer */
	printf ("--->Producer produce %d\n", item);
}

//remove item from buffer (Circular Queue)
int remove_item(buffer_item *item) {
	/*if we have remove BUFFER_SIZE items by definition the next item to remove is at the front of buffer*/
	if (ccounter == BUFFER_SIZE) {
		ccounter = 0;
	}
	*item = buffer[ccounter]; //removes item
	ccounter++; //inc counter
	printf("<---Consumer consume %d\n", *item);
	return 0;
}

// produces an item given params from pthread_create and calls on insert_item
void *producer(void *param) {
	buffer_item item;  //item we are inserting
	unsigned int seed;  //seed for rand_r
	int *id = (int*)param; //converting our parameter into proper int
	int int_id = *id;	

	do{
		/* sleep for a random period of time */
		seed = pthread_self()+time(NULL);  //seed using thread_id + system_time
		int rnum = (rand_r(&seed)%5)+1; //sleep time from (1-5) using the seed
		printf("producer %d seed is: %d\n", int_id, seed);
		printf("producer %d sleep for: %d\n", int_id, rnum);
		sleep(rnum); //put thread to sleep for rnum secs

		seed = pthread_self()+time(NULL);
		/* Generate a random number */
		item = rand_r(&seed);
	
		/* empty lock */
		sem_wait(&empty);
		
		/* mutex lock */
		pthread_mutex_lock(&mutex);
		
		/* insert item */
		insert_item(item);

		/* release mutex */
		pthread_mutex_unlock(&mutex);
		/* signal full */
		sem_post(&full);
	}while(1);
}

/* consume an item from the buffer by calling on remove_item */
void *consumer(void *param){
	buffer_item item; //item that is going to be removed
	unsigned int seed; //seed used for rand_r
	int *id = (int*)param;
	int int_id = *id;
	
	do{
		/* sleep for rand time */
		seed = pthread_self()+time(NULL); //seed using thread ID and system time
		int rnum = (rand_r(&seed)%5)+1; //sleep time
		printf("consumer %d seed is: %d\n", int_id, seed);
		printf("consumer %d sleep for: %d\n", int_id, rnum);
		sleep(rnum); //sleep for r_num
		

		/* full lock */
		sem_wait(&full);
		
		/* mutex lock */
		pthread_mutex_lock(&mutex);
	
		/* remove item */
		remove_item(&item);

		/* release mutex */
		pthread_mutex_unlock(&mutex);
		/* signal empty */
		sem_post(&empty);
	}while(1);
}

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
	}

	// Consumer threads in array
	pthread_t ctid[cNum];	
	pthread_attr_init(&attr);	

	// Create consumer threads
	for (i = 0; i< cNum; i++) {
		/* create consumer threads */
		pthread_create(&ctid[i], &attr, consumer, &i);
		printf("Hi, I am consumer: %d, my thread id = %d\n", i, ctid[i]);
	}

	sleep(sleepTime); // sleep time for main
	exit(0);
}
