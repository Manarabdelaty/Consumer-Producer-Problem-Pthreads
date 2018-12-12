#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

typedef int buffer_item;
#define BUFFER_SIZE 5

/* Global variables shared between the three running threads: main, producer , consumer*/
size_t buffer_len = 0;                                  // lenght of the buffer
pthread_mutex_t  mutex;	           			// mutex variable to sync accessing the buufer          										
pthread_cond_t can_produce;                             // condition variable to signal that the producer can access the buffer
pthread_cond_t can_consume;				// condition variable to signal that the consumer can access the buffer

/* The buffer*/
buffer_item buffer[BUFFER_SIZE];                       // Buffer array of 5 elements

/* Producer function which runs in the producer thread*/
void* producer(void * param){
	buffer_item* buffer = (buffer_item*)param;
	buffer_item item;

	while(1){
   
	/*Sleep for a random period of time*/
	sleep(rand()%3);

	pthread_mutex_lock(&mutex);    // lock the mutex to prevent simulatenous access 
	
	while (buffer_len == BUFFER_SIZE){    // if the buffer is full
		pthread_cond_wait(&can_produce, &mutex);   // then wait for the consumer to cosume and signal that
	}

	item = rand();                     // a random item to insert
	printf("Producer produced %d\n", item);
	buffer[buffer_len] = item;     // insert item in the buffer
	++buffer_len;                  // increase the buffer lenght

	pthread_cond_signal(&can_consume);  // signal that the consumer can now access
	pthread_mutex_unlock(&mutex);       // unlock the mutex variable
	
	}

}
/* Consumer function which runs in the consumer thread*/
void *consumer(void * param){
	buffer_item * buffer = (buffer_item*)param;
	buffer_item item;                            
	
	while(1) {

	sleep(rand()%3);

	pthread_mutex_lock(&mutex);   // lock mutex variable to prevent simulatanrous access

	while (buffer_len ==0)            // if the buffer is empty
		pthread_cond_wait(&can_consume, &mutex); // then wait for the producer to produce and signal the can_consume condition variable
	
	--buffer_len;   // reduce the buffer length
	printf("Cosumer consumed %d\n", buffer[buffer_len]);
	pthread_cond_signal(&can_produce);               // signal that the producer can now access the buffer
	pthread_mutex_unlock(&mutex);                    // unlock the mutex variabel

	}
}

int main(int argc, char * argv[]){
	/* Buffer Lenght ins initialized to zero*/
	int i=0;
	int sleep_time =  *argv[0];
	int num_prod = *argv[1];           // Number of producers
	int num_cons = *argv[2];           // Number of consumers
	
	pthread_t * prod_ptr;              // Pointer to the array of prodcer threads
	pthread_t * cons_ptr;              // Pointer to the array of the consumer threads
	int *indx_prod , *indx_cons;

	/*Allocated dynamic memory for the cons and prod threads array*/
	indx_prod = calloc(num_prod, sizeof(int));
	indx_cons = calloc(num_cons, sizeof(int));
	
	for (i=0; i< num_prod; i++)
		indx_prod[i] = i;

	for (i=0;i<num_cons;i++)
		indx_cons[i] = i;
	
	/* Mutex and condition variables initialization*/
	buffer_len = 0;  
	mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	can_produce = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
	can_consume = (pthread_cond_t)PTHREAD_COND_INITIALIZER;	

	prod_ptr = malloc(sizeof(pthread_t)*num_prod);
	cons_ptr = malloc(sizeof(pthread_t)*num_cons); 
	
	/*Create Producer threads*/
	for (i=0; i< num_prod; i++)
		pthread_create(&prod_ptr[i], NULL, producer, (void*)&buffer);
	/*Create consumer threads*/
	for (i=0; i< num_cons; i++)
		pthread_create(&cons_ptr[i], NULL, consumer, (void*)&buffer);
	
	sleep(sleep_time);
	

	return 0;
}

