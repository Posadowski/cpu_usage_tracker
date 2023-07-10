#include <stdio.h>
#include <pthread.h>

void* reader(void *arg) {
	FILE *file;
	char buffer[256];

	// Open /proc/stat for reading
	file = fopen("/proc/stat", "r");
	if (file == NULL) {
		perror("File open error");
		pthread_exit(NULL);
	}

	// Read data from a file line by line
	while (fgets(buffer, sizeof(buffer), file) != NULL) {
		//Display read data
		printf("%s", buffer);
	}

	// Close the file
	fclose(file);

	pthread_exit(NULL);
}

int main() {
	printf("cpu_usage_tracker\n");

	pthread_t thread;

	// Create thread
	if (pthread_create(&thread, NULL, reader, NULL) != 0) {
		fprintf(stderr, "Thread creation error\n");
		return 1;
	}

	// Wait for the thread to end
	if (pthread_join(thread, NULL) != 0) {
		fprintf(stderr, "Thread waiting error\n");
		return 1;
	}
	return 0;
}
