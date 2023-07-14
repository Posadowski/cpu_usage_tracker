#include "analyzer.h"
#include "printer.h"

int threadNumber = 0;


void get_number_of_processor_cores() {
	FILE *file;
	char buffer[256];
	// Open /proc/stat for reading
	file = fopen("/proc/stat", "r");
	if (file == NULL) {
		perror("File open error");
		return;
	}

	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		if (strncmp(buffer, "cpu", 3) == 0 && isdigit(buffer[3])) {
			threadNumber++;
		}
	}

	fclose(file);
}

void* reader(void *arg) {
	FILE *file;
	char buffer[256];
	while (1) {
		//Display read data
		sem_wait(&readerSemaphore);

		// Open /proc/stat for reading
		file = fopen("/proc/stat", "r");
		if (file == NULL) {
			perror("File open error");
			pthread_exit(NULL);
		}
		while (fgets(buffer, sizeof(buffer), file) != NULL) {
			for (int i = 0; i < threadNumber; i++) {
				if (strncmp(buffer, "cpu", 3) == 0
						&& ((buffer[3] - '0') == i)) {
					sscanf(buffer, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
							params_array[i]->prev.name,
							&params_array[i]->prev.user,
							&params_array[i]->prev.nice,
							&params_array[i]->prev.system,
							&params_array[i]->prev.idle,
							&params_array[i]->prev.iowait,
							&params_array[i]->prev.irq,
							&params_array[i]->prev.softirq,
							&params_array[i]->prev.steal,
							&params_array[i]->prev.guest,
							&params_array[i]->prev.guest_nice);
					//printf("reader prev %s\n", buffer);
				}
			}
		}

		fclose(file);
		usleep(READ_DELAY);
		file = fopen("/proc/stat", "r");
		if (file == NULL) {
			perror("File open error");
			pthread_exit(NULL);
		}
		while (fgets(buffer, sizeof(buffer), file) != NULL) {
			for (int i = 0; i < threadNumber; i++) {
				if (strncmp(buffer, "cpu", 3) == 0
						&& ((buffer[3] - '0') == i)) {
					{
						sscanf(buffer,
								"%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
								params_array[i]->current.name,
								&params_array[i]->current.user,
								&params_array[i]->current.nice,
								&params_array[i]->current.system,
								&params_array[i]->current.idle,
								&params_array[i]->current.iowait,
								&params_array[i]->current.irq,
								&params_array[i]->current.softirq,
								&params_array[i]->current.steal,
								&params_array[i]->current.guest,
								&params_array[i]->current.guest_nice);
						//printf("reader curr %s\n", buffer);
					}
				}
			}
		}
		fclose(file);
		sem_post(&readerSemaphore);
		usleep(READ_DELAY);
	}
	pthread_exit(NULL);
}

int main() {
	printf("cpu_usage_tracker\n");

	pthread_t readerThread, analyzerThread, printerThread;

	get_number_of_processor_cores();

	params_array = malloc(threadNumber * sizeof(struct ThreadParams*));
	usage = malloc(threadNumber * sizeof(struct CPUusage*));


	for (int i = 0; i < threadNumber; i++) {
		params_array[i] = malloc(sizeof(struct ThreadParams));
		usage[i] = malloc(sizeof(struct CPUusage));
	}

	sem_init(&readerSemaphore, 0, 1);
	sem_init(&analyzerSemaphore, 0, 1);

	// Create readerThread
	if (pthread_create(&readerThread, NULL, reader, NULL) != 0) {
		fprintf(stderr, "Thread creation error\n");
		return 1;
	}
	// Create analyzerThread
	if (pthread_create(&analyzerThread, NULL, analyze_cpu_usage, NULL) != 0) {
		fprintf(stderr, "Thread creation error\n");
		return 1;
	}

	if (pthread_create(&printerThread, NULL, print_cpu_usage, NULL) != 0) {
		fprintf(stderr, "Thread creation error\n");
		return 1;
	}

	// Wait for the readerThread to end
	if (pthread_join(readerThread, NULL) != 0) {
		fprintf(stderr, "Thread waiting error\n");
		return 1;
	}
	// Wait for the analyzerThread to end
	if (pthread_join(analyzerThread, NULL) != 0) {
		fprintf(stderr, "Thread waiting error\n");
		return 1;
	}
	// Wait for the printerThread to end
	if (pthread_join(printerThread, NULL) != 0) {
		fprintf(stderr, "Thread waiting error\n");
		return 1;
	}
	sem_destroy(&readerSemaphore);
	sem_destroy(&analyzerSemaphore);

	return 0;
}
