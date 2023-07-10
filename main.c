#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

struct CPUStats {
	char name[256];
	long unsigned int user;
	long unsigned int nice;
	long unsigned int system;
	long unsigned int idle;
	long unsigned int iowait;
	long unsigned int irq;
	long unsigned int softirq;
	long unsigned int steal;
	long unsigned int prevuser;
	long unsigned int prevnice;
	long unsigned int prevsystem;
	long unsigned int previrq;
	long unsigned int prevsoftirq;
	long unsigned int prevsteal;
	long unsigned int previdle;
	long unsigned int previowait;
	long unsigned int guest;
	long unsigned int guest_nice;
};

static double parseCPUStats(struct CPUStats prev,
		struct CPUStats current) {
	double prevIdle = prev.idle + prev.iowait;
	double idle = current.idle + current.iowait;

	double prevNonIdle = prev.user + prev.nice + prev.system + prev.irq
			+ prev.softirq + prev.steal;
	double nonIdle = current.user + current.nice + current.system
			+ current.irq + current.softirq + current.steal;

	double prevTotal = prevIdle + prevNonIdle;
	double total = idle + nonIdle;

	total -= prevTotal;
	idle -= prevIdle;


	return (total - idle) * 100 / total;
}

void* reader(void *arg) {
	FILE *file;
	char buffer[256];
	struct CPUStats statsPrev, statsCur; // Inicjalizacja struktury CPUStats
	while (1) {
		// Open /proc/stat for reading
		file = fopen("/proc/stat", "r");
		if (file == NULL) {
			perror("File open error");
			pthread_exit(NULL);
		}

		// Read data from a file line by line
		while (fgets(buffer, sizeof(buffer), file) != NULL) {

			file = fopen("/proc/stat", "r");
			if (file == NULL) {
				perror("File open error");
				pthread_exit(NULL);
			}
			//Display read data
			while (fgets(buffer, sizeof(buffer), file) != NULL) {
				if (strncmp(buffer, "cpu0", 4) == 0) {
					sscanf(buffer, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
							statsPrev.name, &statsPrev.user, &statsPrev.nice,
							&statsPrev.system, &statsPrev.idle,
							&statsPrev.iowait, &statsPrev.irq,
							&statsPrev.softirq, &statsPrev.steal,
							&statsPrev.guest, &statsPrev.guest_nice);
					//printf("%s", buffer);
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
				if (strncmp(buffer, "cpu0", 4) == 0) {
					sscanf(buffer, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
							statsCur.name, &statsCur.user, &statsCur.nice,
							&statsCur.system, &statsCur.idle, &statsCur.iowait,
							&statsCur.irq, &statsCur.softirq, &statsCur.steal,
							&statsCur.guest, &statsCur.guest_nice);
					//printf("%s", buffer);
					printf("CPU0 USAGE %.2f%%\n", parseCPUStats(statsPrev, statsCur));
				}
			}
			fclose(file);

		}

	}
	pthread_exit(NULL);
}
int main() {
	printf("cpu_usage_tracker\n");

	pthread_t thread;

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
