#include "analyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "printer.h"

void* analyze_cpu_usage(void *args) {
	printf("waiting");
	sem_wait(&readerSemaphore);
	printf("done");
	pthread_t printer;
	while(1){
	struct ThreadParams *params = stats;
	struct CPUusage *usage = malloc(sizeof(struct CPUusage));



	struct CPUStats prev = params->prev;
	struct CPUStats current = params->current;

	double prevIdle = prev.idle + prev.iowait;
	double idle = current.idle + current.iowait;

	double prevNonIdle = prev.user + prev.nice + prev.system + prev.irq
			+ prev.softirq + prev.steal;
	double nonIdle = current.user + current.nice + current.system + current.irq
			+ current.softirq + current.steal;

	double prevTotal = prevIdle + prevNonIdle;
	double total = idle + nonIdle;

	total -= prevTotal;
	idle -= prevIdle;

	//memset(&usage, 0, sizeof(usage));
	snprintf(usage->name, sizeof(usage->name), "%s", params->prev.name);

	usage->usage = (total - idle) * 100 / total;
	sem_post(&readerSemaphore);
	int result = pthread_create(&printer, NULL, print_cpu_usage, (void*)usage);
	    if (result != 0) {
	        printf("Thread creation error: %d\n", result);
	        pthread_exit(NULL);
	    }

	    pthread_join(printer, NULL);
	}
	pthread_exit(NULL);
}
