#include "multi-lookup.h"

int main(int argc, char** argv){
//first we make sure we don't have too few or too many arguments

    if(argc<6){
	fprintf(stderr, "Missing arguments\n");
	printf("Usage synopsis: multi-lookup <# requester> <# resolver> <requester log> <resolver log> [ <data file> ... ]\n");
	exit(-1);
    }
    if(argc > MAX_INPUT_FILES + 5){
	fprintf(stderr, "Too many input files, maximum %d input files allowed\n", MAX_INPUT_FILES);
	exit(-1);
    }

//initialize data structures

    shared_array s;
    int reqTh, resTh = 0;
    
    if((reqTh = atoi(argv[1]))==0){
	fprintf(stderr, "You must enter a positive integer for requester threads\n");
	exit(-1);
    }
    if((resTh = atoi(argv[2]))==0){
	fprintf(stderr, "You must enter a positive integer for resolver threads\n");
	exit(-1);
    }

    if(reqTh>10){
	fprintf(stderr, "Can only use a max of %d requester threads\n", MAX_REQUESTER_THREADS);
	exit(-1);
    }
    if(resTh > 10){
	fprintf(stderr, "Can only use a max of %d resolver threads\n", MAX_RESOLVER_THREADS);
	exit(-1);
    }

    struct timeval t0, t1;

    arrayParam args = {.s = &s, 
		       .fileIndex = 5, 
		       .finalFilePosition = argc-1, 
		       .prodCount = reqTh};
    for(int i = 0; i < argc; i++){
	strcpy(args.argv[i], argv[i]);
    }

    shared_array_init(&args, argv[3], argv[4]);

    pthread_t produce[reqTh];
    pthread_t consume[resTh];

    pthread_attr_t attr;

    pthread_attr_init(&attr);

    gettimeofday(&t0, 0);

    for(int i=0; i<reqTh; i++)
	pthread_create(&produce[i], &attr, producer, &args);
    for(int j=0; j<resTh; j++)
    	pthread_create(&consume[j], &attr, consumer, &args);

    for(int i=0; i<reqTh; i++)
    	pthread_join(produce[i], NULL);
    for(int j=0; j<resTh; j++){
	pthread_join(consume[j], NULL);
    }

    shared_array_free(&args);

    gettimeofday(&t1, 0);
    float elapsed = (float)(t1.tv_sec - t0.tv_sec) +(float) (t1.tv_usec - t0.tv_usec)/1000000;

    printf("./multi-lookup: total time is %2.6f seconds\n", elapsed);

    return 0;
}

void* producer(void* args){
    FILE* fp;
    arrayParam* p = (arrayParam*) args;
    char hostname[MAX_NAME_LENGTH];
    int filesRead = 0;

    while(1){
	sem_wait(&p->argMutex);
//if file to read does not exist, let user know and move to next file
	while((fp = fopen(p->argv[p->fileIndex++], "r"))==NULL){
	    fprintf(stderr, "invalid filename %s\n", p->argv[p->fileIndex-1]);
//if final file does not exist, note that this was last file so we can break from outer loop without trying to read a nonexistent file
	    if(p->fileIndex > p->finalFilePosition){p->errLastFile = 1; break;}
	}
	if(p->errLastFile) break;
	sem_post(&p->argMutex);

	while(fgets(hostname, MAX_NAME_LENGTH, fp)!=NULL){
	    shared_array_push(p, hostname);
	    sem_wait(&p->servMutex);
	    fputs(hostname, p->serviced);
	    sem_post(&p->servMutex);
	}
	fclose(fp);
	filesRead++;

	sem_wait(&p->argMutex);
	if(p->fileIndex > p->finalFilePosition) break;
	sem_post(&p->argMutex);
    }
    p->prodExited++;
    sem_post(&p->argMutex);

    printf("thread %02x serviced %d files\n",(int) pthread_self(), filesRead);

    return 0;
}

void* consumer(void* args){
    arrayParam* p = (arrayParam*) args;
    char hostname[MAX_NAME_LENGTH];
    char ipStr[MAX_IP_LENGTH];
    int popVal;
    int hostCount = 0;
    
    while(1){
	popVal = shared_array_pop(p, hostname);
	if(popVal==0){
	    hostname[strlen(hostname)-1] = '\0';
	    if(dnslookup(hostname, ipStr, MAX_IP_LENGTH)==UTIL_FAILURE) strcpy(ipStr, "NOT_RESOLVED");
	
	    sem_wait(&p->resMutex);
	    fputs(strcat(strcat(hostname, " , "), strcat(ipStr, "\n")), p->results);
	    hostCount++;
	    sem_post(&p->resMutex);
	}
	else if(popVal==-2) break;
    }
    printf("thread %02x resolved %d hostnames\n", (int) pthread_self(), hostCount);
    return 0;
}
