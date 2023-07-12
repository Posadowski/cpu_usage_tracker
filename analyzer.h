#ifndef ANALYZER_H_
#define ANALYZER_H_

#include <semaphore.h>

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

struct ThreadParams{
	struct CPUStats prev;
	struct CPUStats current;
};
struct ThreadParams *stats;
sem_t readerSemaphore;
void* analyze_cpu_usage(void* args);

#endif /* ANALYZER_H_ */
