# CC = clang
# CFLAGS = -Weverything -pthread

CC = gcc -g
CFLAGS = -O3 -Wall -Werror -pthread

BUILD_DIR = build

all: cpu_usage_tracker

cpu_usage_tracker: $(BUILD_DIR)/main.o $(BUILD_DIR)/analyzer.o
	$(CC) $(CFLAGS) -o cpu_usage_tracker $(BUILD_DIR)/main.o $(BUILD_DIR)/analyzer.o

$(BUILD_DIR)/main.o: main.c analyzer.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c main.c -o $(BUILD_DIR)/main.o
	
$(BUILD_DIR)/analyzer.o: analyzer.c analyzer.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c analyzer.c -o $(BUILD_DIR)/analyzer.o	

.PHONY: clean
clean:
	rm -rf cpu_usage_tracker $(BUILD_DIR)
