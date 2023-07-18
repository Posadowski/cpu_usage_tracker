#include "utils.h"
#include <sys/select.h>

void usleep_custom(unsigned int microseconds) {
    struct timeval tv;
    tv.tv_sec = microseconds / 1000000;
    tv.tv_usec = microseconds % 1000000;
    select(0, NULL, NULL, NULL, &tv);
}
