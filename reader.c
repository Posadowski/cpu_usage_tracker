#include "reader.h"
#include <stdbool.h>
#include "analyzer.h"
#include "logger.h"

extern int threadNumber;
extern pthread_mutex_t reader_mutex;
extern pthread_cond_t reader_cond;
extern bool reader_active;
int get_number_of_processor_cores() {
	FILE *file;
	int cpu_number =0;
	char buffer[256];
	// Open /proc/stat for reading
	file = fopen("/proc/stat", "r");
	if (file == NULL) {
		LOG_ERROR("File open error. CPU core count");
		return 0;
	}

	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		if (strncmp(buffer, "cpu", 3) == 0 && isdigit(buffer[3])) {
			cpu_number++;
		}
	}

	fclose(file);
	return cpu_number;
}

void* reader(void *arg) {
	FILE *file;
	char buffer[256];
	while (1) {

		pthread_mutex_lock(&reader_mutex);
		// Open /proc/stat for reading
		file = fopen("/proc/stat", "r");
		if (file == NULL) {
			LOG_ERROR("File open error. First opening");
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
					LOG_DEBUG("reader prev "); LOG_DEBUG(buffer);
				}
			}
		}

		fclose(file);
		usleep(READ_DELAY);
		file = fopen("/proc/stat", "r");
		if (file == NULL) {
			LOG_ERROR("File open error. Second opening");
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
						LOG_DEBUG("reader curr "); LOG_DEBUG(buffer);
						//printf("reader curr %s\n", buffer);
					}
				}
			}
		}
		fclose(file);
		reader_active = true;
		pthread_mutex_unlock(&reader_mutex);
		pthread_cond_signal(&reader_cond); // Notification for analyzer that the reader thread has completed its work




		usleep(READ_DELAY);
	}
	pthread_exit(NULL);
}
