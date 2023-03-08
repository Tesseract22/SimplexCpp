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
    Matrix<float, M + 1, N + M + 2> t = ToTableau(object, ineq_lhs, ineq_rhs);
    Solution s;
    s.success = false;
    while (true) {
        auto pivot_col = getPivotCol<M, N>(t);
        if (pivot_col == N + M + 2) {
            s.success = true;
            return s;
        }
        auto pivot_row = getPivotRow<M, N>(t, pivot_col);
        if (pivot_row == M + 1) {
            return s;
        }
        pivotMatrix<M, N>(t, pivot_row, pivot_col);
        s.res = t.at(M , N + M + 1);
    }
}