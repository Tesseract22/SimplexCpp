#include <Matrix.h>
#include <iostream>

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