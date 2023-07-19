#ifndef READER_H_
#define READER_H_

#include <pthread.h>


int get_number_of_processor_cores();
void* reader(void *arg);

#endif /* READER_H_ */
