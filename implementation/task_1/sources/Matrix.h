#ifndef MULTIPLICATION_MATRIX_H
#define MULTIPLICATION_MATRIX_H

class Matrix {
public:
    int n_rows;
    int n_columns;
    double *data;

    Matrix();

    Matrix(const Matrix &matrix);

    Matrix(int n_rows, int n_columns);

    Matrix(int n_rows, int n_columns, double *input_data);

    Matrix(char *filename);

    ~Matrix();

    void print();
};

#endif //MULTIPLICATION_MATRIX_H
