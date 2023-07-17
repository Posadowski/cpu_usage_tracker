#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include "reader.h"
#include "analyzer.h"
#include "printer.h"
#include "logger.h"

#define TIMEOUT_SECONDS 2

#define LOG_WATCHDOG(...) printf("[WATCHDOG] %s\n", __VA_ARGS__);

bool reader_active = false;
bool analyzer_active = false;
bool printer_active = false;
int threadNumber = 0;

void* watchdog(void *arg) {
	while (1) {
		sleep(2);
		if (!reader_active) {
			LOG_ERROR("reader is not active");
			exit(1);
		} else {
			reader_active = false;
		}
		if (!analyzer_active) {
			LOG_ERROR("analyzer is not active");
			exit(1);
		} else {
			analyzer_active = false;
		}
		if (!printer_active) {
			LOG_ERROR("printer is not active");
			exit(1);
		} else {
			printer_active = false;
		}
	}
	return NULL;
}

int main() {
	printf("cpu_usage_tracker\n");

	pthread_t readerThread, analyzerThread, printerThread, watchdogThread;
	threadNumber = get_number_of_processor_cores();

	printf("[INFO] detected %d cpu's\n", threadNumber);

	params_array = malloc(threadNumber * sizeof(struct ThreadParams*));
	usage = malloc(threadNumber * sizeof(struct CPUusage*));

	for (int i = 0; i < threadNumber; i++) {
		params_array[i] = malloc(sizeof(struct ThreadParams));
		usage[i] = malloc(sizeof(struct CPUusage));
	}

	if (pthread_mutex_init(&reader_mutex, NULL) != 0) {
		perror("reader_mutex initialization error");
		return 1;
	}
	if (pthread_cond_init(&reader_cond, NULL) != 0) {
		perror("reader_cond variable initialization error");
		return 1;
	}

	if (pthread_mutex_init(&analyzer_mutex, NULL) != 0) {
		perror("analyzer_mutex initialization error");
		return 1;
	}
	if (pthread_cond_init(&analyzer_cond, NULL) != 0) {
		perror("analyzer_cond variable initialization error");
		return 1;
	}

	if (pthread_mutex_init(&printer_mutex, NULL) != 0) {
		perror("printer_mutex initialization error");
		return 1;
	}
	if (pthread_cond_init(&printer_cond, NULL) != 0) {
		perror("printer_cond variable initialization error");
		return 1;
	}

	LOG_INFO("initialized mutexes");

	// Create readerThread
	if (pthread_create(&readerThread, NULL, reader, NULL) != 0) {
		LOG_ERROR("readerThread creation error\n");
		return 1;
	}
	// Create analyzerThread
	if (pthread_create(&analyzerThread, NULL, analyze_cpu_usage, NULL) != 0) {
		LOG_ERROR("analyzerThread creation error\n");
		return 1;
	}
	// Create printerThread
	if (pthread_create(&printerThread, NULL, print_cpu_usage, NULL) != 0) {
		LOG_ERROR("printerThread creation error\n");
		return 1;
	}
	// Create watchdogThread
	if (pthread_create(&watchdogThread, NULL, watchdog, NULL) != 0) {
		LOG_ERROR("watchdogThread creation error\n");
		return 1;
	}LOG_INFO("threads created");
	// Wait for the readerThread to end
	if (pthread_join(readerThread, NULL) != 0) {
		LOG_ERROR("readerThread waiting error\n");
		return 1;
	}
	// Wait for the analyzerThread to end
	if (pthread_join(analyzerThread, NULL) != 0) {
		LOG_ERROR("analyzerThread waiting error\n");
		return 1;
	}
	// Wait for the printerThread to end
	if (pthread_join(printerThread, NULL) != 0) {
		LOG_ERROR("printerThread waiting error\n");
		return 1;
	}

	// Wait for the watchdogThread to end
	if (pthread_join(watchdogThread, NULL) != 0) {
		LOG_ERROR("watchdogThread waiting error\n");
		return 1;
	}

	pthread_mutex_destroy(&reader_mutex);
	pthread_cond_destroy(&reader_cond);

	pthread_mutex_destroy(&analyzer_mutex);
	pthread_cond_destroy(&analyzer_cond);

	pthread_mutex_destroy(&printer_mutex);
	pthread_cond_destroy(&printer_cond);

	return 0;
}
