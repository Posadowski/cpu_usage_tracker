# CC = clang
# CFLAGS = -Weverything -pthread

CC = gcc -g
CFLAGS = -O3 -Wall -Werror -pthread -std=c99

BUILD_DIR = build

all: cpu_usage_tracker

cpu_usage_tracker: $(BUILD_DIR)/main.o $(BUILD_DIR)/reader.o $(BUILD_DIR)/analyzer.o $(BUILD_DIR)/printer.o
	$(CC) $(CFLAGS) -o cpu_usage_tracker $(BUILD_DIR)/main.o $(BUILD_DIR)/reader.o  $(BUILD_DIR)/analyzer.o $(BUILD_DIR)/printer.o

$(BUILD_DIR)/main.o: main.c reader.h analyzer.h printer.h logger.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c main.c -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/reader.o: reader.c reader.h analyzer.h logger.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c reader.c -o $(BUILD_DIR)/reader.o	
	
$(BUILD_DIR)/analyzer.o: analyzer.c analyzer.h printer.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c analyzer.c -o $(BUILD_DIR)/analyzer.o	
	
$(BUILD_DIR)/printer.o: printer.c printer.h
	mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c printer.c -o $(BUILD_DIR)/printer.o		

.PHONY: clean
clean:
	rm -rf cpu_usage_tracker $(BUILD_DIR)
