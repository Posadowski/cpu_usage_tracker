#ifndef LOGGER_H_
#define LOGGER_H_

#define DEVELOP_VERSION 0

#if DEVELOP_VERSION == 1
#define LOG_DEBUG(...) printf("[DEBUG] %s\n", __VA_ARGS__);
#define LOG_INFO(...)  printf("[INFO] %s\n", __VA_ARGS__);
#else
#define LOG_DEBUG(x)
#define LOG_INFO(x)
#endif
#define LOG_ERROR(...) printf("[ERROR] %s\n", __VA_ARGS__);

#endif /* LOGGER_H_ */
