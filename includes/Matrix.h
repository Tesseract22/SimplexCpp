#pragma once

template<typename T, unsigned long M, unsigned long N>
class Matrix {

public:
    Matrix() {
        arr_ = new T[M * N];
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

    void rowAddition(size_t dest_row, size_t other_row);

    void rowMultiplication(size_t row, float factor);

    


private:
    size_t index(size_t row, size_t col) const {
        return row * N + col;
    }
    
    T* arr_;
};

#include "Matrix.hpp"