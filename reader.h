#ifndef READER_H_
#define READER_H_

#include <pthread.h>

pthread_mutex_t reader_mutex;
pthread_cond_t reader_cond;
int get_number_of_processor_cores();
void* reader(void *arg);

#endif /* READER_H_ */
