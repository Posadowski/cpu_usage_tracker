#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pthread.h>

static int threadNumber = 0;
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

void read_thread_number() {
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

static double parseCPUStats(struct CPUStats prev, struct CPUStats current) {
	double prevIdle = prev.idle + prev.iowait;
	double idle = current.idle + current.iowait;

	double prevNonIdle = prev.user + prev.nice + prev.system + prev.irq
			+ prev.softirq + prev.steal;
	double nonIdle = current.user + current.nice + current.system + current.irq
			+ current.softirq + current.steal;

	double prevTotal = prevIdle + prevNonIdle;
	double total = idle + nonIdle;

	total -= prevTotal;
	idle -= prevIdle;

	return (total - idle) * 100 / total;
}

void* reader(void *arg) {
	struct CPUStats statsPrev[threadNumber], statsCur[threadNumber]; // Initialization of the  CPUStats structure
	FILE *file;
	char buffer[256];

	while (1) {
		//Display read data
		for (int i = 0; i < threadNumber; i++) {

			// Open /proc/stat for reading
			file = fopen("/proc/stat", "r");
			if (file == NULL) {
				perror("File open error");
				pthread_exit(NULL);
			}
			while (fgets(buffer, sizeof(buffer), file) != NULL) {

				if (strncmp(buffer, "cpu", 3) == 0 && ((buffer[3] - '0') == i)) {
					sscanf(buffer, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu",
							statsPrev[i].name, &statsPrev[i].user,
							&statsPrev[i].nice, &statsPrev[i].system,
							&statsPrev[i].idle, &statsPrev[i].iowait,
							&statsPrev[i].irq, &statsPrev[i].softirq,
							&statsPrev[i].steal, &statsPrev[i].guest,
							&statsPrev[i].guest_nice);
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
								statsCur[i].name, &statsCur[i].user,
								&statsCur[i].nice, &statsCur[i].system,
								&statsCur[i].idle, &statsCur[i].iowait,
								&statsCur[i].irq, &statsCur[i].softirq,
								&statsCur[i].steal, &statsCur[i].guest,
								&statsCur[i].guest_nice);
						printf("CPU%d USAGE %.2f%%\n", i,
								parseCPUStats(statsPrev[i], statsCur[i]));
					}
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

	read_thread_number();

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
