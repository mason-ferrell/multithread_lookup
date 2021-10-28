#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include "util.h"
#include "array.h"
#include <pthread.h>
#include <stdlib.h>
#include <sys/time.h>

#define MAX_IP_LENGTH INET6_ADDRSTRLEN
#define MAX_REQUESTER_THREADS 10
#define MAX_RESOLVER_THREADS 10

void* producer(void* args);
void* consumer(void* args);

#endif
