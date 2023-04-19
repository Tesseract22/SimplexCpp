#pragma once
#include "Matrix.h"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <initializer_list>
#include <stdexcept>

template <typename T, size_t N> using Array = Matrix<T, 1, N>;
template <typename T, size_t N> class Matrix<T, 1, N> {

public:
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
  template <typename Floating,
            std::enable_if_t<std::is_floating_point<Floating>::value, bool>>

  void static arrayAddition(Array<T, N> &dest, const Array<T, N> &source,
                            T mul) {
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
  template <typename Floating,
            std::enable_if_t<std::is_floating_point<Floating>::value, bool>>
  void arrayMultiplication(T factor) {
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

  size_t save(const std::string &path);

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
size_t Matrix<T, 1, N>::save(const std::string &path) {
  FILE *f = fopen(path.data(), "wb+");
  if (!f)
    throw std::runtime_error("cannot open file: " + path);
  size_t bytes_to_write = 1 * ALIGN_COL(N) * sizeof(T);
  size_t bytes_written = 0;
  ssize_t bytes;
  while (bytes_written < bytes_to_write) {
    bytes = fwrite((char *)arr_ + bytes_written, 1,
                   bytes_to_write - bytes_written, f);
    if (bytes < 0)
      throw std::runtime_error("cannot write to file: " + path);
    bytes_written += bytes;
  }
  if (f)
    fclose(f);
  return bytes_written;
}

template <typename T, size_t N>
Matrix<T, 1, N>::Matrix(const std::string &path) {
  FILE *f = fopen(path.data(), "rb");
  if (!f)
    throw std::runtime_error("cannot open file: " + path);
  size_t bytes_to_read = 1 * ALIGN_COL(N) * sizeof(T);
  size_t bytes_read = 0;
  ssize_t bytes;
  while (feof(f) == 0) {
    bytes = fread((char *)arr_ + bytes_read, 1, bytes_to_read - bytes_read, f);
    // std::cout << *(size_t *)arr_;
    if (bytes == -1) {
      throw std::runtime_error("failed to read file: " + path);
    }
    if (bytes == 0)
      break;
    bytes_read += bytes;
  };
  std::cout << bytes_read << std::endl;

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
