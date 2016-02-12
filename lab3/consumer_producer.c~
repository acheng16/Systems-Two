#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include "buffer.h"

//initialize globals
int pcounter=0; //counter for producer
int ccounter=0; //counter for consumer
extern pthread_mutex_t mutex; //mutex
extern sem_t full, empty;  //full and empty semaphores
buffer_item buffer[BUFFER_SIZE]; //Buffer from buffer.h
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
