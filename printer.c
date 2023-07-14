#include "printer.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
extern int threadNumber;
void* print_cpu_usage(void* args){
	while(1){
		sem_wait(&analyzerSemaphore);
		for (int i = 0; i < threadNumber; i++) {
			if(strlen(usage[i]->name) != 0){
				if(usage[i]->name != 0)printf("%s usage %.2f%% \n",usage[i]->name,usage[i]->usage);
			}
		}
		printf("\n");
		sem_post(&analyzerSemaphore);
		usleep(2000000);
	}
	pthread_exit(NULL);
}
