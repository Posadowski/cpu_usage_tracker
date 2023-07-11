#include "analyzer.h"
#include <stdio.h>
#include <pthread.h>

void* analyze_cpu_usage(void* args){
	struct ThreadParams* params = (struct ThreadParams*)args;

	 struct CPUStats prev = params->prev;
	 struct CPUStats current = params->current;

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

	 printf("%s usage %.2f%% \n",params->prev.name,(total - idle) * 100 / total);
	 pthread_exit(NULL);
}
