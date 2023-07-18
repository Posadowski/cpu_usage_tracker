#include <stdbool.h>
#include <time.h>
#include <signal.h>
#include "reader.h"
#include "analyzer.h"
#include "printer.h"
#include "logger.h"
#include "utils.h"
#define TIMEOUT_SECONDS 2

#define LOG_WATCHDOG(...) printf("[WATCHDOG] %s\n", __VA_ARGS__);

bool reader_active = false;
bool analyzer_active = false;
bool printer_active = false;
int cpuNumber = 0;

volatile sig_atomic_t done = 0;

void term(int signum) {
	LOG_INFO("SIGTERM signal detected");
	done = 1;
}

void* watchdog(void *arg) {
	while (!done) {
		sleep(2);
		pthread_mutex_lock(&reader_mutex);
		if (!reader_active) {
			LOG_ERROR("reader is not active");
			exit(1);
		} else {
			reader_active = false;
		}
		pthread_mutex_unlock(&reader_mutex);
		pthread_mutex_lock(&analyzer_mutex);
		if (!analyzer_active) {
			LOG_ERROR("analyzer is not active");
			exit(1);
		} else {
			analyzer_active = false;
		}
		pthread_mutex_unlock(&analyzer_mutex);
		pthread_mutex_lock(&printer_mutex);
		if (!printer_active) {
			LOG_ERROR("printer is not active");
			exit(1);
		} else {
			printer_active = false;
		}
		pthread_mutex_unlock(&printer_mutex);
	}
	LOG_INFO("watchdog thread finished");
	pthread_exit(NULL);
}

int main() {
	printf("cpu_usage_tracker\n");

	struct sigaction action;
	memset(&action, 0, sizeof(struct sigaction));
	action.sa_handler = term;
	sigaction(SIGTERM, &action, NULL);

	pthread_t readerThread, analyzerThread, printerThread, watchdogThread;
	cpuNumber = get_number_of_processor_cores();

	printf("[INFO] detected %d cpu's\n", cpuNumber);

	params_array = malloc(cpuNumber * sizeof(struct ThreadParams*));
	usage = malloc(cpuNumber * sizeof(struct CPUusage*));

	for (int i = 0; i < cpuNumber; i++) {
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

    while (!done)
    {
        int t = sleep(3);
        /* sleep returns the number of seconds left if
         * interrupted */
        while (t > 0)
        {
            t = sleep(t);
        }
    }

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

	printf("cpu_usage_tracker done");
	return 0;
}
