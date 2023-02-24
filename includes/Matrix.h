#pragma once

template<typename T, int M, int N>
class Matrix {

public:
    Matrix() {
        arr_ = new T[M * N];
    }

    Matrix(T** arr);



    T& get(int row, int col) {
        return arr_[index(row, col)];
    }

    void debugPrint();

    


private:
    unsigned index(int row, int col) {
        return row * N + col;
    }
    
    T* arr_;
};

#include "Matrix.hpp"