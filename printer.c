#include "printer.h"
#include <stdio.h>
#include <pthread.h>

void* print_cpu_usage(void* args){
	struct CPUusage *usage = (struct CPUusage*)args;


	 printf("%s usage %.2f%% \n",usage->name,usage->usage);
	 pthread_exit(NULL);
}
