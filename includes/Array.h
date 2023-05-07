#pragma once
#include "Matrix.h"
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

  Matrix(T fill) {
    for (size_t i = 0; i < ALIGN_COL(N); ++i)
      get(i) = fill;
  }

  T *data() { return arr_; }

  //   Matrix operator=(std::initializer_list<T> list);

  inline T &operator[](size_t col) { return arr_[index(col)]; }

  inline const T &operator[](size_t col) const { return arr_[index(col)]; }

  inline T &get(size_t col) { return arr_[index(col)]; }

  const T &at(size_t col) const { return arr_[index(col)]; }

  friend std::ostream &operator<<(std::ostream &stream, const Array<T, N> &s) {

    char out[(N + 1) * 20 + 100];

    size_t position = 0;
    position += std::sprintf(out + position, "\n");

    // index header
    for (size_t j = 0; j < N; ++j) {
      position += std::sprintf(out + position, "%20zu", j);
    }
    position += std::sprintf(out + position, "\n");
    stream << out;
    position = 0;

    // provided header

    if (s.header) {
      for (size_t j = 0; j < N; ++j) {
        position += std::sprintf(out + position, "%20s",
                                 s.header->at(j).substr(0, 20).data());
      }
      position += std::sprintf(out + position, "\n");
      stream << out;
      position = 0;
    }

    for (size_t j = 0; j < N; ++j) {
      position += std::sprintf(out + position, "%20f", s.at(j));
    }
    position += std::sprintf(out + position, "\n");

    stream << out;
    position = 0;

    return stream;
  }

  //   template <class Q = T>
  //   typename std::enable_if<std::is_same<Q, float>::value, void>::
  //       type static arrayAddition(Array<T, N> &dest, const Array<T, N>
  //       &source,
  //                                 T mul) {
  //     if (mul == 0)
  //       return;
  //     size_t j;
  //     __m128 mul_vec = _mm_set1_ps(mul);
  //     std::cout << ALIGN_COL(N) << std::endl;
  //     for (j = 0; j < N / 4 * 4; j += 4) {
  //       __m128 dest_vec = _mm_load_ps(dest.arr_ + j);
  //       __m128 other_vec = _mm_load_ps(source.arr_ + j);
  //       __m128 result_vec = _mm_add_ps(dest_vec, _mm_mul_ps(other_vec,
  //       mul_vec)); _mm_store_ps(dest.arr_ + j, result_vec);
  //     }
  //     for (; j < N; ++j) {
  //       // std::cout << j << std::endl;
  //       // std::cout << source.arr_[j] << std::endl;
  //       dest.arr_[j] += source.arr_[j] * mul;
  //     }
  //   }
  //   template <class Q = T>
  //   typename std::enable_if<std::is_same<Q, float>::value, void>::type
  //   arrayMultiplication(T factor) {
  //     size_t j;
  //     __m128 mul_vec = _mm_set1_ps(factor);
  //     for (j = 0; j < N / 4 * 4; j += 4) {

  //       __m128 result_vec = _mm_mul_ps(_mm_load_ps(arr_ + j), mul_vec);
  //       _mm_store_ps(arr_ + j, result_vec);
  //     }
  //     for (; j < N; ++j) {
  //       arr_[j] *= factor;
  //     }
  //   }

  Array<std::string, N> *header = nullptr;
  size_t save(const std::string &path);

private:
  //   size_t precision;
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

template <typename T, size_t N> Matrix<T, 1, N>::Matrix(T (&arr)[N]) {
  for (size_t j = 0; j < N; ++j) {
    arr_[index(j)] = arr[j];
  }
}
