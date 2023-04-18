#pragma once
#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <string>
#include <type_traits>
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

#include <iostream>
#include <xmmintrin.h>

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
