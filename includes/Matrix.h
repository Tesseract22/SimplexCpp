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
    T& get(int row, int col) {
        return arr_[index(row, col)];
    }

    T& at(int row, int col) const {
        return arr_[index(row, col)];
    }

    void debugPrint();

    


private:
    unsigned index(int row, int col) const {
        return row * N + col;
    }
    
    T* arr_;
};

#include "Matrix.hpp"