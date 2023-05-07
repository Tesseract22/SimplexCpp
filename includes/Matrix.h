#pragma once
#include "Approx.h"
#include <cstdio>
#include <cstring>
#include <immintrin.h>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#ifndef __EMSCRIPTEN__
#define PADDING 8
#define ALIGN_COL(N) (((N + PADDING - 1) / PADDING * PADDING))
#else
#define ALIGN_COL(N) N
#endif

// ssize_t readData(FILE* f, void* buffer) {
//   const static block_size = 1024;
//   while (feof(f) == 0) {
//     size_t bytes_read = fread(buffer, 1, block_size, f);
//   }
// }
template <typename T, size_t M, size_t N> class Matrix;
template <typename T, size_t N> using Array = Matrix<T, 1, N>;
template <typename T, size_t M, size_t N> class Matrix {

public:
  template <typename, size_t, size_t> friend struct LuaMatrix;
  Matrix() = default;

  Matrix(const std::string &path);

  Matrix(T (&arr)[M][N]);

  Matrix(std::initializer_list<std::initializer_list<T>> list);

  Matrix(T fill) {
    for (size_t i = 0; i < ALIGN_COL(N) * M; ++i)
      get(i) = fill;
  }

  Matrix(const Matrix &other) {
    memcpy(arr_, other.arr_, ALIGN_COL(N) * M * sizeof(T));
  }

  T *data() { return arr_; }

  const size_t spaceUsed = M * ALIGN_COL(N);

  //   ~Matrix() { delete[] arr_; }

  /**
   * retunrs a reference at the given location
   */
  inline T *operator[](size_t row) { return arr_ + index(row, 0); }

  inline T &operator()(size_t row, size_t col) { return arr_[index(row, col)]; }

  inline const T &operator()(size_t row, size_t col) const {
    return arr_[index(row, col)];
  }

  inline T &get(size_t row, size_t col) {
#if SIMPLEX_DEBUG
    if (row >= M || col >= N)
      throw std::runtime_error("row and col " + std::to_string(row) + " " +
                               std::to_string(col) + " out of range");
#endif
    return arr_[index(row, col)];
  }

  const T &at(size_t row, size_t col) const {
#if SIMPLEX_DEBUG
    if (row >= M || col >= N)
      throw std::runtime_error("row and col " + std::to_string(row) + " " +
                               std::to_string(col) + " out of range");
#endif
    return arr_[index(row, col)];
  }

  friend std::ostream &operator<<(std::ostream &stream,
                                  const Matrix<T, M, N> &s) {

    char out[(N + 1) * 20 + 100];
    int space = 10;
    int p = 3;
    const static auto formatter = [space](const char *type) -> std::string {
      return "%" + std::to_string(space) + type;
    };
    const static auto precison = [space, p]() -> std::string {
      return "%" + std::to_string(space) + "." + std::to_string(p) + "f";
    };

    size_t position = 0;
    position += std::sprintf(out + position, "\n");

    // index header
    position += std::sprintf(out + position, "%4s", "");
    if (s.row_header)
      position += std::sprintf(out + position, formatter("").data(), "");
    for (size_t j = 0; j < N; ++j) {
      position += std::sprintf(out + position, formatter("zu").data(), j);
    }
    position += std::sprintf(out + position, "\n");
    stream << out;
    position = 0;

    // provided header

    if (s.col_header) {
      position += std::sprintf(out + position, "%4s", "");
      if (s.row_header)
        position += std::sprintf(out + position, formatter("").data(), "");
      for (size_t j = 0; j < N; ++j) {
        position += std::sprintf(out + position, formatter("s").data(),
                                 s.col_header->at(j).substr(0, 20).data());
      }
    }

    position += std::sprintf(out + position, "\n");
    stream << out;
    position = 0;
    for (size_t i = 0; i < M; ++i) {
      position += std::sprintf(out + position, "%4zu", i);
      if (s.row_header)
        position += std::sprintf(out + position, formatter("s").data(),
                                 s.row_header->at(i).substr(0, 20).data());
      for (size_t j = 0; j < N; ++j) {
        position += std::sprintf(out + position, precison().data(), s.at(i, j));
      }
      position += std::sprintf(out + position, "\n");

      stream << out;
      position = 0;
    }
    return stream;
  }

  template <class Q = T>
  typename std::enable_if<std::is_same<Q, float>::value, void>::type
  rowAddition(size_t dest_row, size_t other_row, T mul) {
    if (Approx::isApporxZero<Q>(mul, 1e-6))
      return;
    size_t j = 0;
#ifndef __EMSCRIPTEN__
    __m128 mul_vec = _mm_set1_ps(mul);
    for (; j < N / 4 * 4; j += 4) {
      __m128 dest_vec = _mm_load_ps(arr_ + index(dest_row, j));
      __m128 other_vec = _mm_load_ps(arr_ + index(other_row, j));
      __m128 result_vec = _mm_add_ps(dest_vec, _mm_mul_ps(other_vec, mul_vec));
      _mm_store_ps(arr_ + index(dest_row, j), result_vec);
    }
#endif
    for (; j < N; ++j) {
      arr_[index(dest_row, j)] += arr_[index(other_row, j)] * mul;
    }
  }

  template <class Q = T>
  typename std::enable_if<std::is_same<Q, float>::value, void>::type
  rowMultiplication(size_t row, T factor) {
    size_t j = 0;
#ifndef __EMSCRIPTEN__
    __m128 mul_vec = _mm_set1_ps(factor);
    for (; j < N / 4 * 4; j += 4) {

      __m128 result_vec =
          _mm_mul_ps(_mm_load_ps(arr_ + index(row, j)), mul_vec);
      _mm_store_ps(arr_ + index(row, j), result_vec);
    }
#endif
    for (; j < N; ++j) {
      arr_[index(row, j)] *= factor;
    }
  }

  template <class Q = T>
  typename std::enable_if<std::is_same<Q, double>::value, void>::type
  rowAddition(size_t dest_row, size_t other_row, T mul) {
    if (mul == 0)
      return;
    size_t j = 0;
#ifndef __EMSCRIPTEN__
    __m256d mul_vec = _mm256_set1_pd(mul);
    for (; j < N / 4 * 4; j += 4) {
      __m256d dest_vec = _mm256_load_pd(arr_ + index(dest_row, j));
      __m256d other_vec = _mm256_load_pd(arr_ + index(other_row, j));
      __m256d result_vec =
          _mm256_add_pd(dest_vec, _mm256_mul_pd(other_vec, mul_vec));
      _mm256_store_pd(arr_ + index(dest_row, j), result_vec);
    }
#endif
    for (; j < N; ++j) {
      arr_[index(dest_row, j)] += arr_[index(other_row, j)] * mul;
    }
  }

  template <class Q = T>
  typename std::enable_if<std::is_same<Q, double>::value, void>::type
  rowMultiplication(size_t row, T factor) {
    size_t j = 0;
#ifndef __EMSCRIPTEN__
    __m256d mul_vec = _mm256_set1_pd(factor);
    for (; j < N / 4 * 4; j += 4) {

      __m256d result_vec =
          _mm256_mul_pd(_mm256_load_pd(arr_ + index(row, j)), mul_vec);
      _mm256_store_pd(arr_ + index(row, j), result_vec);
    }
#endif
    for (; j < N; ++j) {
      arr_[index(row, j)] *= factor;
    }
  }

  template <class Q = T>
  typename std::enable_if<std::is_fundamental<Q>::value, size_t>::type
  save(const std::string &path) {
    FILE *f = fopen(path.data(), "wb+");
    if (!f)
      throw std::runtime_error("cannot open file: " + path);
    size_t bytes_to_write = M * ALIGN_COL(N) * sizeof(T);
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
  Array<std::string, N> *col_header = nullptr;
  Array<std::string, M> *row_header = nullptr;

private:
  size_t index(size_t row, size_t col) const {
    return row * ALIGN_COL(N) + col;
  }
#ifndef __EMSCRIPTEN__
  alignas(32) T arr_[M * ALIGN_COL(N)] = {0};
#else
  arr_[M * ALIGN_COL(N)] = {0};
#endif
  //   T *arr_ = new T[ALIGN_COL(N) * M]();
};

template <typename T, size_t M, size_t N>
Matrix<T, M, N>::Matrix(const std::string &path) {
  FILE *f = fopen(path.data(), "rb");
  if (!f)
    throw std::runtime_error("cannot open file: " + path);
  size_t bytes_to_read = M * ALIGN_COL(N) * sizeof(T);
  size_t bytes_read = 0;
  ssize_t bytes;
  while (feof(f) == 0) {
    bytes = fread(arr_, 1, bytes_to_read - bytes_read, f);
    if (bytes == -1) {
      throw std::runtime_error("failed to read file: " + path);
    }
    bytes_read += bytes;
    if (bytes == 0)
      break;
  };

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
Matrix<T, M, N>::Matrix(T (&arr)[M][N]) {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      arr_[index(i, j)] = arr[i][j];
    }
  }
}
