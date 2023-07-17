#include "analyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include "reader.h"
#include "printer.h"
#include "logger.h"

extern int threadNumber;
extern bool analyzer_active;

void* analyze_cpu_usage(void *args) {

	while (1) {
		LOG_DEBUG("anylze wait for reader");
		pthread_mutex_lock(&reader_mutex);

		pthread_cond_wait(&reader_cond, &reader_mutex); // Wait for a notification from the reader thread

		LOG_DEBUG("anylze start");
		pthread_mutex_lock(&analyzer_mutex);
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

		pthread_mutex_unlock(&reader_mutex);

		pthread_cond_signal(&analyzer_cond); // Notification that the reader thread has completed its work
		LOG_INFO("analyzer_cond send notification");
		analyzer_active = true;
		pthread_mutex_unlock(&analyzer_mutex);
		LOG_INFO("analyzer_mutex unlock");
		usleep(READ_DELAY);
	}
	pthread_exit(NULL);
}
