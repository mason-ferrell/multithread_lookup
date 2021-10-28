#include "array.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <time.h>

#define NUM_THREADS 20
//for this test program, we are using the same number of producer and consumer strings, we are mainly testing no race conditions or deadlock occur

int total_pushed = 0;
int total_popped = 0;
sem_t test_mutex;
//the above global variables are to test that the producers/consumers access the array the same number of times

void *producer(void* args);
void *consumer(void* args);

int main(){
    shared_array test_array;
    push_pop_args args = {.s = &test_array, 
			  .element = "test_string",
			  .consumerstring = malloc(MAX_NAME_LENGTH*sizeof(char))};
    sem_init(&test_mutex, 0, 1);
    sem_init(&args.mutex, 0, 1);
    sem_init(&args.full, 0, 0);
    sem_init(&args.empty, 0, MAX_NAME_LENGTH);

    if(shared_array_init(&test_array) < 0) exit(-1);

    pthread_t producers[NUM_THREADS];
    pthread_t consumers[NUM_THREADS];
    pthread_attr_t attr;

    clock_t begin = clock();

    pthread_attr_init(&attr);
    for(int i=0; i<NUM_THREADS; i++){
	pthread_create(&producers[i], &attr, producer, &args);
	pthread_create(&consumers[i], &attr, consumer, &args);
    }

    for(int i=0; i<NUM_THREADS; i++){
	pthread_join(producers[i], NULL);
	pthread_join(consumers[i], NULL);
    }
    shared_array_free(&test_array);
    free(args.consumerstring);

    clock_t end = clock();
    double elapsed = (double) (end-begin)/CLOCKS_PER_SEC;

    printf("total pushed: %d\n", total_pushed);
    printf("total popped: %d\n", total_popped);
    printf("time of execution: %f\n", elapsed);
    return 0;
}

//producer and consumer threads each push or pop 1000 elements from the array
void *producer(void* args){
    for(int i=0; i<1000; i++){
        while(shared_array_push((push_pop_args*) args));
        sem_wait(&test_mutex);
        total_pushed++;
        sem_post(&test_mutex);
    }
    pthread_exit(0);
}

void *consumer(void* args){
    for(int i=0; i<1000; i++){
        while(shared_array_pop((push_pop_args*) args));
        sem_wait(&test_mutex);
        total_popped++;
        sem_post(&test_mutex);
    }
    pthread_exit(0);
}
