#ifndef PRINTER_H_
#define PRINTER_H_
#include <pthread.h>
struct CPUusage{
	char name[256];
	double usage;
};

struct CPUusage **usage;

pthread_mutex_t printer_mutex;
pthread_cond_t printer_cond;
void* print_cpu_usage(void* args);

#endif /* PRINTER_H_ */
