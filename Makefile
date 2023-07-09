# CC = clang
# CFLAGS = -Weverything -pthread

CC = gcc -g
CFLAGS = -O3 -Wall -Werror -pthread

BUILD_DIR = build

all: cpu_usage_tracker

cpu_usage_tracker: $(BUILD_DIR)/main.o
	$(CC) $(CFLAGS) -o cpu_usage_tracker $(BUILD_DIR)/main.o

$(BUILD_DIR)/main.o: main.c
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c main.c -o $(BUILD_DIR)/main.o

.PHONY: clean
clean:
	rm -rf cpu_usage_tracker $(BUILD_DIR)
