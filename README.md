# cpu_usage_tracker
cpu_usage_tracker is a program that tracks the CPU usage on a Linux system and provides information about the number of processor cores.

## Installation

To run the cpu_usage_tracker program and its tests, you need to install the libcmocka-dev library on your Linux system. You can do this by executing the following command:

```bash
sudo apt-get install libcmocka-dev
```

## Usage

The cpu_usage_tracker program can be compiled and executed by using the provided Makefile. To compile the program, navigate to the cpu_usage_tracker directory and run the following command:

```bash
make
```

This will build the cpu_usage_tracker executable. To run the program, simply execute it:

```bash
./cpu_usage_tracker
```

## Testing 

cpu_usage_tracker comes with unit tests that can be executed to verify the correctness of its functionalities. To run the tests, use the following command:

```bash
make test
```

Please note that the tests require the libcmocka-dev library, which needs to be installed as mentioned in the Installation section.

The test suite will run and provide information about the test results, ensuring that the program is functioning as expected.

