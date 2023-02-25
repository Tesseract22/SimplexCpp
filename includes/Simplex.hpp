#include <Matrix.h>
#include <array>
template <unsigned long M, unsigned long N>
Matrix<float, M + 1, N + M + 2> ToTableau(const std::array<float, N>& object,  const Matrix<float, M, N>& ineq_lhs, const std::array<float, M>& ineq_rhs) {\
    (void)object; (void)ineq_rhs;
    Matrix<float, M + 1, N + M + 2> result;
    for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < N; ++j) {
            result.get(i, j) = ineq_lhs.at(i, j);
        }
        result.get(i, N + i) = 1;
        result.get(i, N + M + 1) = ineq_rhs[i];
    }
    for (size_t j = 0; j < N; ++j) {
        result.get(M, j) = -object[j];
    }
    result.get(M, N + M) = 1;

    return result;
}


template <size_t M, size_t N>
size_t getPivotCol(const Matrix<float, M + 1, N + M + 2>& m) {
    float min = 0;
    size_t col = N + M + 2;
    for (size_t j = 0; j < N; ++j) {
        if (m.get(M, j) < min) {
            min = m.get(M, j);
            col = j;
        }
    }
    return col;
}


template <size_t M, size_t N>
size_t getPivotRow(const Matrix<float, M + 1, N + M + 2>& m, size_t pivot_col) {
    float min = 0;
    size_t row = M + 1;
    // @vector_multiplication
    for (size_t i = 0; i < M + 1; ++i) {
        float ratio = m.get(i, N + M + 1) / m.get(i, pivot_col);
        if (ratio > 0 && ratio < min) {
            min = ratio;
            row = i;
        }
    }
    return row;
}

template <size_t M, size_t N>
void pivotMatrix(const Matrix<float, M + 1, N + M + 2>& m, size_t pivot_row, size_t pivot_col) {

}