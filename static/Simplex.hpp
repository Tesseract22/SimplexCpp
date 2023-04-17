#pragma once
#include <cstddef>
#include <iostream>
#include <array>
#include <xmmintrin.h>
#include <vector>
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




struct Solution {
    float res;
    bool success;
    std::vector<float> variables;
    friend std::ostream& operator<<(std::ostream& stream, const Solution& s) {
        stream << "Success: " << s.success << std::endl;
        stream << "Result: " << s.res << std::endl;
        for (size_t i = 0; i < s.variables.size(); ++i) {
            stream << "x" << i << ": " << s.variables[i] << ' ';
        }
        stream << std::endl;
        return stream;
    }
};






template <size_t M, size_t N>
Matrix<float, M + 1, N + M + 2> ToTableau(const std::array<float, N>& object,  const Matrix<float, M, N>& ineq_lhs, const std::array<float, M>& ineq_rhs);


template <size_t M, size_t N>
size_t getPivotCol(const Matrix<float, M + 1, N + M + 2>& m);

template <size_t M, size_t N>
size_t getPivotRow(const Matrix<float, M + 1, N + M + 2>& m, size_t pivot_col);

template <size_t M, size_t N>
void pivotMatrix(Matrix<float, M + 1, N + M + 2>& m, size_t pivot_row, size_t pivot_col);

template <size_t M, size_t N>
Solution solveLP(const std::array<float, N>& object,  const Matrix<float, M, N>& ineq_lhs, const std::array<float, M>& ineq_rhs);





template <unsigned long M, unsigned long N>
Matrix<float, M + 1, N + M + 2> ToTableau(const std::array<float, N>& object,  const Matrix<float, M, N>& ineq_lhs, const std::array<float, M>& ineq_rhs) {
    Matrix<float, M + 1, N + M + 2> res;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            res.get(i, j) = ineq_lhs.at(i, j);
            if (ineq_lhs.at(i, j) != 0) {

            }
        }
        res.get(i, N + i) = 1;
        res.get(i, N + M + 1) = ineq_rhs[i];
    }
    for (size_t j = 0; j < N; ++j) {
        res.get(M, j) = -object[j];
    }
    res.get(M, N + M) = 1;

    return res;
}


template <size_t M, size_t N>
size_t getPivotCol(const Matrix<float, M + 1, N + M + 2>& m) {
    float min = 0;
    size_t col = N + M + 2;
    for (size_t j = 0; j < N; ++j) {
        if (m.at(M, j) < min) {
            min = m.at(M, j);
            col = j;
        }
    }
    return col;
}


template <size_t M, size_t N>
size_t getPivotRow(const Matrix<float, M + 1, N + M + 2>& m, size_t pivot_col) {
    float min = __FLT_MAX__;
    size_t row = M + 1;
    // @vector_multiplication
    for (size_t i = 0; i < M; ++i) {
        float ratio = (float)m.at(i, N + M + 1) / (float)m.at(i, pivot_col);
        if (ratio >= 0 && ratio < min) {
            min = ratio;
            row = i;
        }
    }
    return row;
}

template <size_t M, size_t N>
void pivotMatrix(Matrix<float, M + 1, N + M + 2>& m, size_t pivot_row, size_t pivot_col) {
    float pivot_variable = m.at(pivot_row, pivot_col);
    m.rowMultiplication(pivot_row, 1 / pivot_variable);
    for (size_t i = 0; i < M + 1; ++i) {
        if (i == pivot_row) continue;
        float r = m.at(i, pivot_col) / m.at(pivot_row, pivot_col);
        m.rowAddition(i, pivot_row, -r);
    }
    m.debugPrint();
}



template <size_t M, size_t N>
Solution solveLP(const std::array<float, N>& object,  const Matrix<float, M, N>& ineq_lhs, const std::array<float, M>& ineq_rhs) {
    Matrix<float, M + 1, N + M + 2> res = ToTableau<M, N>(object, ineq_lhs, ineq_rhs);
    Solution s;
    s.success = false;
    while (true) {
        auto pivot_col = getPivotCol<M, N>(res);
        if (pivot_col == N + M + 2) {
            s.success = true;
            s.variables.resize(N);
            std::array<float, N> var_arr;
            std::array<size_t, N> count_arr;
            count_arr.fill(0);
            for (size_t i = 0; i < M; ++i) {
                for (size_t j = 0; j < N; j++) {
                    if (res.at(i, j) != 0) {
                        count_arr[j]++;
                        var_arr[j] = i;
                    }
                }
            }
            for (size_t j = 0; j < N; j++) {
                if (count_arr[j] == 1) {
                    s.variables.at(j) = res.at(var_arr[j], N + M + 1);
                }
            }

            
            
            return s;
        }
        auto pivot_row = getPivotRow<M, N>(res, pivot_col);
        if (pivot_row == M + 1) {
            return s;
        }
        pivotMatrix<M, N>(res, pivot_row, pivot_col);
        s.res = res.at(M , N + M + 1);
    }
}

