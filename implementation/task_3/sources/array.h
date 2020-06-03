//
// Created by merlin on 03.06.2020.
//

#ifndef SORT_ARRAY_H
#define SORT_ARRAY_H

struct Array {
    std::size_t length;
    int* data;
};

Array get_array(char* filename);
bool is_result_correct(Array array);
#endif //SORT_ARRAY_H
