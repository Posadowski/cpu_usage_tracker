#ifndef PRINTER_H_
#define PRINTER_H_
#include <pthread.h>
struct CPUusage{
	char name[256];
	double usage;
};



void* print_cpu_usage(void* args);

#endif /* PRINTER_H_ */
