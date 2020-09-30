#ifndef MULTIPLICATION_MATRIX_MULTIPLER_H
#define MULTIPLICATION_MATRIX_MULTIPLER_H

#include "Matrix.h"

class MatrixMultiplier {
public:
    Matrix *A;
    Matrix *B;

    MatrixMultiplier(Matrix *a, Matrix *b);

    Matrix get_sequential_result();

//    Matrix get_parallel_for_result();
//
//    Matrix get_static_schedule_result();
//
//    Matrix get_dynamic_schedule_result();
//
//    Matrix get_guided_schedule_result();

private:
    int n_rows;
    int n_columns;

    double calculate_cell_value(int row_idx, int column_idx) const;
};


#endif //MULTIPLICATION_MATRIX_MULTIPLER_H
