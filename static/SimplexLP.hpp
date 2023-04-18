#pragma once
#include <xmmintrin.h>
#include <cstdio>
#include <cstddef>
#include <vector>
#include <initializer_list>
#include <string>
#include <type_traits>
#include <iostream>
#define PADDING 4
#define ALIGN_COL(N) (((N + PADDING - 1) / PADDING * PADDING))

template <typename T, size_t M, size_t N> class Matrix {

public:
  static_assert(std::is_arithmetic<T>::value, "Matrix type must be numeric");
  Matrix() = default;

  Matrix(const std::string &path);

  Matrix(T (&arr)[M][N]);

  Matrix(std::initializer_list<std::initializer_list<T>> list);

  /**
   * retunrs a reference at the given location
   */
  inline T *operator[](size_t row) { return arr_ + index(row, 0); }

  inline T &operator()(size_t row, size_t col) { return arr_[index(row, col)]; }

  inline const T &operator()(size_t row, size_t col) const {
    return arr_[index(row, col)];
  }

  inline T &get(size_t row, size_t col) { return arr_[index(row, col)]; }

  const T &at(size_t row, size_t col) const { return arr_[index(row, col)]; }

  void debugPrint() const;

  void rowAddition(size_t dest_row, size_t other_row, T mul);

  void rowMultiplication(size_t row, T factor);

  bool save(const std::string &path);

private:
  size_t index(size_t row, size_t col) const {
    return row * ALIGN_COL(N) + col;
  }
  T arr_[M * ALIGN_COL(N)] = {0};
};


template <typename T, size_t M, size_t N>
Matrix<T, M, N>::Matrix(const std::string &path) {
  (void)path;
  FILE *f = fopen(path.data(), "rb");

  // SIMPLEX_DEFER_FUNC(
  //     size_t bytes_to_written = M * N * sizeof(T);
  //     size_t bytes_wriiten = 0;
  //     ssize_t bytes;
  //     while ((bytes = fread(arr_, 1024, 1, f)) > 0 ) {
  //         if (bytes < 0)
  //         bytes_wriiten += bytes;
  //     };
  // )

  if (f)
    fclose(f);
}

template <typename T, size_t M, size_t N>
Matrix<T, M, N>::Matrix(std::initializer_list<std::initializer_list<T>> list) {
  size_t row = 0;
  // if (list.size() > )
  for (auto &sub_list : list) {
    if (row == M)
      break;
    size_t col = 0;
    for (T el : sub_list) {
      if (col == N)
        break;
      arr_[index(row, col)] = el;
      col++;
    }
    row++;
  }
}

template <typename T, size_t M, size_t N>
void Matrix<T, M, N>::debugPrint() const {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      std::cout << at(i, j) << ' ';
    }
    std::cout << std::endl;
  }
}

template <typename T, size_t M, size_t N>
Matrix<T, M, N>::Matrix(T (&arr)[M][N]) {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      arr_[index(i, j)] = arr[i][j];
    }
  }
}

template <typename T, size_t M, size_t N>
void Matrix<T, M, N>::rowAddition(size_t dest_row, size_t other_row, T mul) {
  if (mul == 0)
    return;
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

template <typename T, size_t M, size_t N>
void Matrix<T, M, N>::rowMultiplication(size_t row, T factor) {
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

template <typename T, size_t N> using Array = Matrix<T, 1, N>;
template <typename T, size_t N> class Matrix<T, 1, N> {

public:
  static_assert(std::is_arithmetic<T>::value, "Array type must be numeric");
  Matrix() = default;

  Matrix(const std::string &path);

  Matrix(T (&arr)[N]);

  Matrix(std::initializer_list<T> list);

  //   Matrix operator=(std::initializer_list<T> list);

  inline T &operator[](size_t col) { return arr_[index(col)]; }

  inline const T &operator[](size_t col) const { return arr_[index(col)]; }

  inline T &get(size_t col) { return arr_[index(col)]; }

  const T &at(size_t col) const { return arr_[index(col)]; }

  void debugPrint() const;

  void static arrayAddition(Array<T, N> &dest, const Array<T, N> &source,
                            T mul);

  void arrayMultiplication(T factor);

  bool save(const std::string &path);

private:
  inline size_t index(size_t col) const { return col; }
  T arr_[ALIGN_COL(N)] = {};
};

template <typename T, size_t N>
Matrix<T, 1, N>::Matrix(std::initializer_list<T> list) {
  size_t col = 0;
  for (T el : list) {
    if (col == N)
      break;
    arr_[col++] = el;
  }
}

template <typename T, size_t N>
Matrix<T, 1, N>::Matrix(const std::string &path) {
  (void)path;
  FILE *f = fopen(path.data(), "rb");

  // SIMPLEX_DEFER_FUNC(
  //     size_t bytes_to_written = M * N * sizeof(T);
  //     size_t bytes_wriiten = 0;
  //     ssize_t bytes;
  //     while ((bytes = fread(arr_, 1024, 1, f)) > 0 ) {
  //         if (bytes < 0)
  //         bytes_wriiten += bytes;
  //     };
  // )

  if (f)
    fclose(f);
}

template <typename T, size_t N> void Matrix<T, 1, N>::debugPrint() const {
  for (size_t j = 0; j < N; ++j) {
    std::cout << at(j) << ' ';
  }
  std::cout << std::endl;
}

template <typename T, size_t N> Matrix<T, 1, N>::Matrix(T (&arr)[N]) {
  for (size_t j = 0; j < N; ++j) {
    arr_[index(j)] = arr[j];
  }
}

template <typename T, size_t N>
void Matrix<T, 1, N>::arrayAddition(Array<T, N> &dest,
                                    const Array<T, N> &source, T mul) {
  if (mul == 0)
    return;
  size_t j;
  __m128 mul_vec = _mm_set1_ps(mul);
  for (j = 0; j < N / 4 * 4; j += 4) {
    __m128 dest_vec = _mm_load_ps(dest.arr_ + j);
    __m128 other_vec = _mm_load_ps(source.arr_ + j);
    __m128 result_vec = _mm_add_ps(dest_vec, _mm_mul_ps(other_vec, mul_vec));
    _mm_store_ps(dest.arr_ + j, result_vec);
  }
  for (; j < N; ++j) {
    // std::cout << j << std::endl;
    // std::cout << source.arr_[j] << std::endl;
    dest.arr_[j] += source.arr_[j] * mul;
  }
}
template <typename T, size_t N>
void Matrix<T, 1, N>::arrayMultiplication(T factor) {
  size_t j;
  __m128 mul_vec = _mm_set1_ps(factor);
  for (j = 0; j < N / 4 * 4; j += 4) {

    __m128 result_vec = _mm_mul_ps(_mm_load_ps(arr_ + j), mul_vec);
    _mm_store_ps(arr_ + j, result_vec);
  }
  for (; j < N; ++j) {
    arr_[j] *= factor;
  }
}

namespace Simplex {
struct Solution {
  float res;
  bool success;
  std::vector<float> variables;
  friend std::ostream &operator<<(std::ostream &stream, const Solution &s) {
    stream << "Success: " << s.success << '\n';
    stream << "Result: " << s.res << '\n';
    for (size_t i = 0; i < s.variables.size(); ++i) {
      stream << "x" << i << ": " << s.variables[i] << ' ';
    }
    stream << std::endl;
    return stream;
  }
};

template <size_t M, size_t N>
Matrix<float, M + 1, N + M + 2> ToTableau(const Array<float, N> &object,
                                          const Matrix<float, M, N> &ineq_lhs,
                                          const Array<float, M> &ineq_rhs);

template <size_t M, size_t N>
size_t getPivotCol(const Matrix<float, M + 1, N + M + 2> &m);

template <size_t M, size_t N>
size_t getPivotRow(const Matrix<float, M + 1, N + M + 2> &m, size_t pivot_col);

template <size_t M, size_t N>
void pivotMatrix(Matrix<float, M + 1, N + M + 2> &m, size_t pivot_row,
                 size_t pivot_col);

template <size_t M, size_t N>
Solution solveLP(const Array<float, N> &object,
                 const Matrix<float, M, N> &ineq_lhs,
                 const Array<float, M> &ineq_rhs);

template <size_t M, size_t N>
Matrix<float, M + 1, N + M + 2> ToTableau(const Array<float, N> &object,
                                          const Matrix<float, M, N> &ineq_lhs,
                                          const Array<float, M> &ineq_rhs) {
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
size_t getPivotCol(const Matrix<float, M + 1, N + M + 2> &m) {
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
size_t getPivotRow(const Matrix<float, M + 1, N + M + 2> &m, size_t pivot_col) {
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
void pivotMatrix(Matrix<float, M + 1, N + M + 2> &m, size_t pivot_row,
                 size_t pivot_col) {
  float pivot_variable = m.at(pivot_row, pivot_col);
  m.rowMultiplication(pivot_row, 1 / pivot_variable);
  for (size_t i = 0; i < M + 1; ++i) {
    if (i == pivot_row)
      continue;
    float r = m.at(i, pivot_col) / m.at(pivot_row, pivot_col);
    m.rowAddition(i, pivot_row, -r);
  }
  m.debugPrint();
}

template <size_t M, size_t N>
Solution solveLP(const Array<float, N> &object,
                 const Matrix<float, M, N> &ineq_lhs,
                 const Array<float, M> &ineq_rhs) {
  Matrix<float, M + 1, N + M + 2> res =
      ToTableau<M, N>(object, ineq_lhs, ineq_rhs);
  Solution s;
  s.success = false;
  while (true) {
    auto pivot_col = getPivotCol<M, N>(res);
    if (pivot_col == N + M + 2) {
      s.success = true;
      s.variables.resize(N);
      Array<float, N> var_arr;
      Array<size_t, N> count_arr;
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
    s.res = res.at(M, N + M + 1);
  }
}
} // namespace Simplex
