#include <stdio.h>
#include <stdbool.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include "reader.h"

//definitions required by reader.c
int cpuNumber = 0;
bool reader_active = false;
bool done = 0;

// Mock fopen() replacement function
FILE* __wrap_fopen(const char *path, const char *mode) {
	check_expected_ptr(path);
	check_expected_ptr(mode);
	return mock_ptr_type(FILE*);
}

// Get_number_of_processor_cores() function test in case of file open error
static void test_get_number_of_processor_cores_file_open_error(void **state) {
	expect_string(__wrap_fopen, path, "/proc/stat");
	expect_string(__wrap_fopen, mode, "r");
	will_return(__wrap_fopen, NULL); // Simulation of a file open error

	// Calling the tested function
	int result = get_number_of_processor_cores();
	// Assert
	assert_int_equal(result, 0);
}

// Get_number_of_processor_cores() test for valid data
static void test_get_number_of_processor_cores_success(void **state) {
	const char *proc_stat_data = "cpu  123 456 789\n"
			"cpu0 111 222 333\n"
			"cpu1 444 555 666\n"
			"cpu2 444 555 666\n";

	expect_string(__wrap_fopen, path, "/proc/stat");
	expect_string(__wrap_fopen, mode, "r");
	FILE *mock_file = fmemopen((void*) proc_stat_data, strlen(proc_stat_data),
			"r");
	will_return(__wrap_fopen, mock_file);

	// Calling the tested function
	int result = get_number_of_processor_cores();

	// Assert
	assert_int_equal(result, 3);
}

int main(void) {
	const struct CMUnitTest tests[] = {
	cmocka_unit_test(test_get_number_of_processor_cores_file_open_error),
	cmocka_unit_test(test_get_number_of_processor_cores_success), };
	return cmocka_run_group_tests(tests, NULL, NULL);
}
