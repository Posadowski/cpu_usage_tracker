#include "analyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "printer.h"

extern int threadNumber;

int get_semaphore_value(sem_t *sem) {
	int sval;
	sem_getvalue(sem, &sval);
	return sval;
}

void* analyze_cpu_usage(void *args) {

	while (1) {
		sem_wait(&readerSemaphore);
		sem_wait(&analyzerSemaphore);

		for (int i = 0; i < threadNumber; i++) {

			double prevIdle = params_array[i]->prev.idle
					+ params_array[i]->prev.iowait;
			double idle = params_array[i]->current.idle
					+ params_array[i]->current.iowait;

			double prevNonIdle = params_array[i]->prev.user
					+ params_array[i]->prev.nice + params_array[i]->prev.system
					+ params_array[i]->prev.irq + params_array[i]->prev.softirq
					+ params_array[i]->prev.steal;
			double nonIdle = params_array[i]->current.user
					+ params_array[i]->current.nice
					+ params_array[i]->current.system
					+ params_array[i]->current.irq
					+ params_array[i]->current.softirq
					+ params_array[i]->current.steal;

			double prevTotal = prevIdle + prevNonIdle;
			double total = idle + nonIdle;

			total -= prevTotal;
			idle -= prevIdle;

			char nameBuffer[256];

			snprintf(nameBuffer, sizeof(nameBuffer), "%s",
					params_array[i]->prev.name);

			snprintf(usage[i]->name, sizeof(usage[i]->name), "%s", nameBuffer);
			usage[i]->usage = (total - idle) * 100 / total;
		}
		sem_post(&analyzerSemaphore);
		sem_post(&readerSemaphore);

		usleep(READ_DELAY*2);
	}
	pthread_exit(NULL);
}
