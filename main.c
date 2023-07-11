#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

#include "analyzer.h"

static int threadNumber = 0;

void get_number_of_processor_cores() {
	FILE *file;
	char buffer[256];
	// Open /proc/stat for reading
	file = fopen("/proc/stat", "r");
	if (file == NULL) {
		perror("File open error");
		pthread_exit(NULL);
	}

	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		if (strncmp(buffer, "cpu", 3) == 0 && isdigit(buffer[3])) {
			threadNumber++;
		}
	}

	printf("threadNumber = %d\n", threadNumber);

}

void* reader(void *arg) {
	pthread_t analyzer;
	FILE *file;
	char buffer[256];

	while (1) {
		//Display read data
		for (int i = 0; i < threadNumber; i++) {
			struct ThreadParams* stats; // Initialization of the  CPUStats structure
			stats = (struct ThreadParams*)malloc(sizeof(struct ThreadParams));
			// Open /proc/stat for reading
			file = fopen("/proc/stat", "r");
			if (file == NULL) {
				perror("File open error");
				pthread_exit(NULL);
			}
			while (fgets(buffer, sizeof(buffer), file) != NULL) {

				if (strncmp(buffer, "cpu", 3) == 0 && ((buffer[3] - '0') == i)) {
					sscanf(buffer, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
							stats->prev.name, &stats->prev.user,
							&stats->prev.nice, &stats->prev.system,
							&stats->prev.idle, &stats->prev.iowait,
							&stats->prev.irq,  &stats->prev.softirq,
							&stats->prev.steal, &stats->prev.guest,
							&stats->prev.guest_nice);
				}
			}

			fclose(file);
			sleep(1);
			file = fopen("/proc/stat", "r");
			if (file == NULL) {
				perror("File open error");
				pthread_exit(NULL);
			}
			while (fgets(buffer, sizeof(buffer), file) != NULL) {
				if (strncmp(buffer, "cpu", 3) == 0 && ((buffer[3] - '0') == i)) {
					{
						sscanf(buffer,
								"%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
								stats->current.name, &stats->current.user,
								&stats->current.nice, &stats->current.system,
								&stats->current.idle, &stats->current.iowait,
								&stats->current.irq, &stats->current.softirq,
								&stats->current.steal, &stats->current.guest,
								&stats->current.guest_nice);

						int result = pthread_create(&analyzer, NULL, analyze_cpu_usage, (void*)stats);
						    if (result != 0) {
						        printf("Thread creation error: %d\n", result);
						        pthread_exit(NULL);
						    }

						    pthread_join(analyzer, NULL);
					}
				}

			}
			fclose(file);

		}
		printf("\n");
		printf("\n");
	}

	pthread_exit(NULL);
}
int main() {
	printf("cpu_usage_tracker\n");

	pthread_t thread;

	get_number_of_processor_cores();


	// Create thread
	if (pthread_create(&thread, NULL, reader, NULL) != 0) {
		fprintf(stderr, "Thread creation error\n");
		return 1;
	}

	// Wait for the thread to end
	if (pthread_join(thread, NULL) != 0) {
		fprintf(stderr, "Thread waiting error\n");
		return 1;
	}
	return 0;
}
