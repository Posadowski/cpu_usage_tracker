#ifndef PRINTER_H_
#define PRINTER_H_
#include <semaphore.h>
struct CPUusage{
	char name[256];
	double usage;
};

struct CPUusage **usage;

sem_t analyzerSemaphore;
void* print_cpu_usage(void* args);

#endif /* PRINTER_H_ */
