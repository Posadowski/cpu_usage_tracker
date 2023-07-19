#include "printer.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "analyzer.h"
#include "analyzer.h"
#include "logger.h"
#include "utils.h"

extern int cpuNumber;
extern bool printer_active;
extern volatile sig_atomic_t done;
void* print_cpu_usage(void* args){
	LOG_INFO("printer init");
	while(!done){
		pthread_mutex_lock(&analyzer_mutex);

	    pthread_cond_wait(&analyzer_cond, &analyzer_mutex); // Wait for a notification from the analyzer thread

#if DEVELOP_VERSION == 0
	    LOG_DEBUG("printer start");
	    int ret = system("clear");
	    if (ret == -1) {
	      	LOG_ERROR(" call clear in linux");
	      	return 0;
	      }
#endif
		for (int i = 0; i < cpuNumber; i++) {
			if(strlen(usage[i]->name) != 0){
				if(strlen(usage[i]->name) != 0)printf("%s usage %.2f%% \n",usage[i]->name,usage[i]->usage);
			}
		}
		printf("\n");
		pthread_mutex_lock(&printer_mutex);
		printer_active = true;
		pthread_mutex_unlock(&printer_mutex);
		pthread_mutex_unlock(&analyzer_mutex);
		usleep(READ_DELAY);
	}
	pthread_mutex_unlock(&printer_mutex);
	pthread_mutex_unlock(&analyzer_mutex);
	LOG_INFO("printer thread finished");
	pthread_exit(NULL);
}
