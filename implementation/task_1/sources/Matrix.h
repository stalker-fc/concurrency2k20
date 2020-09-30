#ifndef MULTIPLICATION_MATRIX_H
#define MULTIPLICATION_MATRIX_H

class Matrix {
public:
    int n_rows{};
    int n_columns{};
    double *data;

    Matrix();

    Matrix(int n_rows, int n_columns);

    explicit Matrix(char *filename);

    ~Matrix();

    void print() const;
};

#endif //MULTIPLICATION_MATRIX_H
