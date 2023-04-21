#pragma once
#include "Approx.h"
#include "SimplexLP.h"
#include <array>
#include <assert.h>
#include <cmath>
#include <cstddef>
#include <limits.h>
#include <limits>
#include <math.h>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

template <typename T, size_t M, size_t N>
void checkMatrix(const Matrix<T, M, N> &matrix, const std::string &s) {
  stringstream ss;
  auto buf = cout.rdbuf();
  cout.rdbuf(ss.rdbuf());
  cerr << matrix << endl;
  string expected = ss.str();
  if (!expected.starts_with(s)) {

    cerr << "Expected:\n|" << ss.str() << "|\n";
    cerr << "Got:\n|" << s << "|\n";
    assert(false);
  }
  cout.rdbuf(buf);
}

template <typename T, size_t M, size_t N>
void checkMatrix(const Matrix<T, M, N> &matrix,
                 const std::vector<std::vector<T>> &vec) {
  for (size_t i = 0; i < M; ++i) {
    for (size_t j = 0; j < N; ++j) {
      assert(matrix.at(i, j) == vec[i][j]);
    }
  }
}

template <typename T, size_t N>
void checkMatrix(const Array<T, N> &matrix, const std::vector<T> &vec) {
  for (size_t j = 0; j < N; ++j) {
    if (!isApproxEqual<T>(matrix[j], vec[j], SIMPLEX_FLOAT_PRECISION)) {
      cerr << "checkMatrix with Array<" + to_string(N) + ">\n";
      cerr << "at position <" + to_string(j) +
                  ">, expect: " + to_string(vec[j]) + ", but got " +
                  to_string(matrix[j]) + " instead"
           << '\n';
      assert(false);
    }
  }
}

bool inline startWith(const std::string &longer, const std::string &prefix) {
  return equal(prefix.begin(), prefix.end(), longer.begin());
}