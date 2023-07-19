#include "analyzer.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include "reader.h"
#include "printer.h"
#include "logger.h"
#include "utils.h"

extern int cpuNumber;
extern bool analyzer_active;
extern volatile sig_atomic_t done;
extern pthread_mutex_t analyzer_mutex;
extern pthread_cond_t analyzer_cond;
extern pthread_mutex_t reader_mutex;
extern pthread_cond_t reader_cond;
extern struct ThreadParams **params_array;
extern struct CPUusage **usage;
void* analyze_cpu_usage(void *args) {

	while (!done) {
		LOG_DEBUG("anylze wait for reader");
		pthread_mutex_lock(&reader_mutex);

		pthread_cond_wait(&reader_cond, &reader_mutex); // Wait for a notification from the reader thread

		LOG_DEBUG("anylze start");
		pthread_mutex_lock(&analyzer_mutex);
		for (int i = 0; i < cpuNumber; i++) {

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
	pthread_mutex_unlock(&reader_mutex);
	pthread_mutex_unlock(&analyzer_mutex);
	LOG_INFO("analyzer thread finished");
	pthread_exit(NULL);
}
