#pragma once
#define PADDING 4
template<typename T, unsigned long M, unsigned long N>
class Matrix {

public:
    Matrix() {
        align_col_ = (N + PADDING - 1) / PADDING * PADDING;
        arr_ = new T[M * align_col_];
    }

    Matrix(T (&arr)[M][N]);

    ~Matrix();


    /**
     * retunrs a reference at the given location
    */
    T& get(size_t row, size_t col) {
        return arr_[index(row, col)];
    }

    T& at(size_t row, size_t col) const {
        return arr_[index(row, col)];
    }

    void debugPrint();

    void rowAddition(size_t dest_row, size_t other_row, float mul);

    void rowMultiplication(size_t row, float factor);

    


private:
    size_t index(size_t row, size_t col) const {
        return row * align_col_ + col;
    }

    size_t align_col_;
    T* arr_;
};

#include "Matrix.hpp"