#include "array.h"

int shared_array_init(arrayParam* args, char* servicedFile, char* resultsFile){
    args->s->top = 0;
    args->prodExited = 0;
    args->serviced = fopen(servicedFile, "w");
    args->results = fopen(resultsFile, "w");

    sem_init(&args->argMutex, 0, 1);
    sem_init(&args->mutex, 0, 1);
    sem_init(&args->full, 0, 0);
    sem_init(&args->empty, 0, ARRAY_SIZE);

    sem_init(&args->resMutex, 0, 1);
    sem_init(&args->servMutex, 0, 1);

    return 0;
}

//shared array push is straightforward producer function as in producer-consumer problem given in class
int shared_array_push(arrayParam* args, char* hostname){
    sem_wait(&args->empty);
    sem_wait(&args->mutex);

    if(args->s->top >= ARRAY_SIZE){
	sem_post(&args->mutex);
        return -1;
    }
    
    strncpy(args->s->array[args->s->top++], hostname, MAX_NAME_LENGTH);

    sem_post(&args->mutex);
    sem_post(&args->full);

    return 0;
}

//mostly same as consumer function given in class for basic producer-consumer problem
int shared_array_pop(arrayParam* args, char* hostname){
//must protect this whole section by argMutex, as we reference arguments in checking whether producers are finished, and failing to protect args->s->top in the read section can lead to deadlock
    sem_wait(&args->argMutex);

//if consumer empty, return -1; if consumer is empty and all producers are finished, return -2
    if(args->s->top < 1){
	if(args->prodExited==args->prodCount){
	    sem_post(&args->argMutex);
	    return -2;
	}
	sem_post(&args->argMutex);
	return -1;
    }
    sem_wait(&args->full);
    sem_wait(&args->mutex);

    strncpy(hostname, args->s->array[--args->s->top], MAX_NAME_LENGTH);

    sem_post(&args->mutex);
    sem_post(&args->empty);
    sem_post(&args->argMutex);

    return 0;
}

void shared_array_free(arrayParam* args){
    fclose(args->serviced);
    fclose(args->results);
}

