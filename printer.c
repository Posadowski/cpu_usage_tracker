#include "printer.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "analyzer.h"
#include "analyzer.h"
#include "logger.h"
extern int threadNumber;
extern bool printer_active;

void* print_cpu_usage(void* args){
	LOG_INFO("printer init");
	while(1){
		pthread_mutex_lock(&analyzer_mutex);

	    pthread_cond_wait(&analyzer_cond, &analyzer_mutex); // Wait for a notification from the analyzer thread

	    LOG_DEBUG("printer start");
		for (int i = 0; i < threadNumber; i++) {
			if(strlen(usage[i]->name) != 0){
				if(usage[i]->name != 0)printf("%s usage %.2f%% \n",usage[i]->name,usage[i]->usage);
			}
		}
		printf("\n");
		printer_active = true;
		pthread_mutex_unlock(&analyzer_mutex);
		sleep(1);
	}
	pthread_exit(NULL);
}
