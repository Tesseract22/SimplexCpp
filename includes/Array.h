#pragma once
#include "Matrix.h"
#include <cstddef>
#include <initializer_list>

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

#include <iostream>
#include <xmmintrin.h>
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