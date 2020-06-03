#include <cstring>
#include <iomanip>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include "array.h"


bool is_result_correct(Array array) {
    for (std::size_t i = 1; i < array.length; ++i) {
        if (array.data[i] < array.data[i - 1])
            return false;
    }

    return true;
}

Array get_array(char *filename) {
    std::ifstream input;
    input.open(filename);
    if (!input) {
        std::cout << "Unable to open file";
        std::exit(1);
    }

    int *data = NULL;

    int BUFFER_SIZE = 100;
    int *buffer = new int[BUFFER_SIZE];
    int current_buffer_length = 0;
    std::size_t total_array_length = 0;

    while (!input.eof()) {
        input >> buffer[current_buffer_length];
        if (current_buffer_length == BUFFER_SIZE) {
            if (data) {
                data = (int *) realloc(data, sizeof(int) * total_array_length);
                std::memcpy(&data[total_array_length - current_buffer_length], buffer, sizeof(int) * BUFFER_SIZE);
            } else {
                data = (int *) malloc(sizeof(int) * BUFFER_SIZE);
                std::memcpy(data, buffer, sizeof(int) * BUFFER_SIZE);
            }
            current_buffer_length = 0;
        } else {
            current_buffer_length++;
            total_array_length++;
        }
    }

    if (current_buffer_length > 0) {
        if (data) {
            data = (int *) realloc(data, sizeof(int) * total_array_length);
            std::memcpy(&data[total_array_length - current_buffer_length], buffer, sizeof(int) * current_buffer_length);
        } else {
            data = (int *) malloc(sizeof(int) * current_buffer_length);
            std::memcpy(data, buffer, sizeof(int) * current_buffer_length);
        }
    }
    Array array;
    array.length = total_array_length;
    array.data = data;
    return array;
}