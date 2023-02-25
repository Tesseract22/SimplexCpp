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

    return result;
}