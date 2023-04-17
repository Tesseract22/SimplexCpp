#pragma once
#include <cstddef>
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

#include <iostream>
#include <xmmintrin.h>

template<typename T, unsigned long M, unsigned long N>
void Matrix<T, M, N>::debugPrint() {
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0;j  < N; ++j) {
            std::cout << get(i, j) << ' ';
        }
        std::cout << std::endl;
    }
}


template<typename T, unsigned long M, unsigned long N>
Matrix<T, M, N>::Matrix(T (&arr)[M][N]) {
    align_col_ = (N + PADDING - 1) / PADDING * PADDING;
    arr_ = new T[M * align_col_];
    for (size_t i = 0;i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            arr_[index(i,j)] = arr[i][j];
        }
    }
}


template<typename T, unsigned long M, unsigned long N>
Matrix<T, M, N>::~Matrix() {
    delete[] arr_;
}


template<typename T, unsigned long M, unsigned long N>
void Matrix<T, M, N>::rowAddition(size_t dest_row, size_t other_row, float mul) {
    if (mul == 0) return;
    size_t j;
    __m128 mul_vec = _mm_set1_ps(mul);
    for (j = 0; j < N / 4 * 4; j += 4) {
        __m128 dest_vec = _mm_load_ps(arr_ + index(dest_row, j));
        __m128 other_vec = _mm_load_ps(arr_ + index(other_row, j));
        __m128 result_vec = _mm_add_ps(dest_vec, _mm_mul_ps(other_vec, mul_vec));
        _mm_store_ps(arr_ + index(dest_row, j), result_vec);
}
    for (; j < N; ++j) {
        arr_[index(dest_row, j)] += arr_[index(other_row, j)] * mul;
    }
   
}

template<typename T, unsigned long M, unsigned long N>
void Matrix<T, M, N>::rowMultiplication(size_t row, float factor) {
    size_t j;
    __m128 mul_vec = _mm_set1_ps(factor);
    for (j = 0; j < N / 4 * 4; j += 4) {

        __m128 result_vec = _mm_mul_ps(_mm_load_ps(arr_ + index(row, j)), mul_vec);
        _mm_store_ps(arr_ + index(row, j), result_vec);
    }
    for (; j < N; ++j) {
        arr_[index(row, j)] *= factor;
    }
}


