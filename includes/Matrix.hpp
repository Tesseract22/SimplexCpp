#include <Matrix.h>
#include <iostream>
#include <xmmintrin.h>
using namespace std;
template<typename T, unsigned long M, unsigned long N>
void Matrix<T, M, N>::debugPrint() {
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0;j  < N; ++j) {
            cout << get(i, j) << ' ';
        }
        cout << endl;
    }
}


template<typename T, unsigned long M, unsigned long N>
Matrix<T, M, N>::Matrix(T (&arr)[M][N]) {
    arr_ = new T[M * N];
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
void Matrix<T, M, N>::rowAddition(size_t dest_row, size_t other_row) {
    size_t j;
    for (j = 0; j < N; j += 4) {
         __m128 dest_vec = _mm_load_ps(arr_ + dest_row * N);
         __m128 other_vec = _mm_load_ps(arr_ + other_row * N);
         __m128 result_vec = _mm_add_ps(dest_vec, other_vec);
         _mm_store_ps(arr_ + dest_row * N, result_vec);
    }
   
}

template<typename T, unsigned long M, unsigned long N>
void Matrix<T, M, N>::rowMultiplication(size_t row, float factor) {

}