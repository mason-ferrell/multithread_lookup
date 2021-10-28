#ifndef ARRAY_H
#define ARRAY_H

#include <semaphore.h>
#include <stdio.h>
#include <string.h>

#define ARRAY_SIZE 10
#define MAX_NAME_LENGTH 255
#define MAX_INPUT_FILES 100

//the following struct holds the array and the top counter, this struct is the array our threads will be concurrently accessing
typedef struct{
    char array[ARRAY_SIZE][MAX_NAME_LENGTH];
    int top;
} shared_array;

//this struct defines the arguments we use in our shared array functions, so that we can pass all the arguments as one argument call in our pthread_create calls
typedef struct{
    shared_array* s;

//"mutex, full, and empty" are for synchronization of shared array
//argMutex, servMutex, and resMutex are for protecting arguments, the serviced file, and the results file respectively
    sem_t mutex;
    sem_t full;
    sem_t empty;

    sem_t argMutex;
    sem_t servMutex;
    sem_t resMutex;

//prodCount holds the total number of producers, prodExited holds the total number of producers that have exited so far
    int fileIndex;
    int prodCount;
    int prodExited;
    int errLastFile;

    int finalFilePosition;
    char argv[MAX_INPUT_FILES + 5][MAX_NAME_LENGTH];

    FILE* serviced;
    FILE* results;
} arrayParam;

int  shared_array_init(arrayParam* args, char* servicedFile, char* resultsFile);
int  shared_array_push(arrayParam* args, char* hostname);
int  shared_array_pop (arrayParam* args, char* hostname);
void shared_array_free(arrayParam* args);

#endif
