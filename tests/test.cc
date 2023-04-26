#include "Approx.h"
#include "Array.h"
#include "Matrix.h"
#include "Simplex.h"
#include "test_util.hpp"
#include <cassert>
#include <cstddef>
#include <fstream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

void matrix_test() {
  // test initializtion
  Matrix<float, 2, 2> m1;
  checkMatrix(m1, "0 0 \n0 0");
  Matrix<float, 5, 5> m2;
  checkMatrix(m2, vector<vector<float>>(5, vector<float>(5)));
  // test creating with initializer list
  Matrix<float, 2, 2> m3({
      {1, 2},
      {10, 9},
  });
  checkMatrix(m3, {
                      {1, 2},
                      {10, 9},
                  });

  // test addition
  m3.rowAddition(0, 0, 2);
  checkMatrix(m3, {
                      {3, 6},
                      {10, 9},
                  });
  m3.rowAddition(0, 1, 0.5);
  checkMatrix(m3, {
                      {8, 10.5},
                      {10, 9},
                  });
}

void simplex_method_basics() {
#if 0
  {
    // basic example
    // https://people.richland.edu/james/ictcm/2006/
    const size_t M = 3;
    const size_t N = 2;
    Matrix<float, M, N> lhs({
        {1, 2},
        {1, 1},
        {3, 2},
    });
    Array<float, N> obj({40, 30});
    Array<float, M> rhs({16, 9, 24});

    Simplex<M, N> s(obj, lhs, rhs);
    auto solution = s.solution;
    cerr << solution << '\n';
    assert(solution.success);
    assert(solution.res == 330);
    checkMatrix(solution.variables, vector<float>({6, 3}));
  }
#endif
#if 0
  {
    // manually inverting constraint and object
    const size_t M = 2;
    const size_t N = 3;
    Matrix<float, M, N> lhs({
        {1, 3, 2},
        {1, 5, 1},
    });
    Array<float, N> obj({8, 10, 7});
    Array<float, M> rhs({10, 8});

    Simplex<M, N> s(obj, lhs, rhs);
    auto solution = s.solution;
    cerr << solution << '\n';
    assert(solution.success);
    assert(solution.res == 64);
    checkMatrix(solution.variables, vector<float>({8, 0, 0}));
  }
#endif
}

void simplex_method_phase_2() {
#if 0
  {
    // large example
    // infeasible
    // manually inverting constraint and object
    // https://www.matem.unam.mx/~omar/math340/2-phase.html
    const size_t M = 3;
    const size_t N = 3;
    Matrix<float, M, N> lhs({
        {2, -1, 2},
        {2, -3, 1},
        {-1, 1, -2},
    });
    Array<float, N> obj({1, -1, 1});
    Array<float, M> rhs({4, -5, -1});

    Simplex<M, N> s(obj, lhs, rhs);
    auto solution = s.solution;
    cerr << solution << '\n';
    assert(solution.success);
    assert(Approx::isApproxEqual<float>(solution.res, 3.0 / 5.0,
                                        SIMPLEX_FLOAT_PRECISION));
    checkMatrix(solution.variables, vector<float>({0, 14.0 / 5.0, 17.0 / 5.0}));
  }
#endif
#if 0
  {
    // large example
    // manually inverting constraint and object
    // http://kirkmcdonald.github.io/posts/calculation.html
    const size_t M = 5;
    const size_t N = 6;
    Matrix<float, M, N> lhs({
        {40, 0, -30, -10, 0, 0},
        {-30, 30, -30, -45, 0, 0},
        {0, -20, -40, -55, 0, 0},
        {30, 30, 0, 50, -1, 0},
        {0, 0, 100, 100, 0, -1},

    });
    Array<float, N> obj({0, 0, 0, 0, 0, -1});
    Array<float, M> rhs({-10, 0, -45, 0, 0});
    Simplex s(obj, lhs, rhs);
    auto &solution = s.solution;
    cerr << solution << '\n';
    assert(solution.success);
    assert(Approx::isApproxEqual<float>(solution.res, -2300. / 39.,
                                        SIMPLEX_FLOAT_PRECISION));
    checkMatrix(solution.variables,
                vector<float>({0, 61. / 78., 8.0 / 39.0, 5.0 / 13, 555. / 13.,
                               2300. / 39.}));
  }
#endif
#if 0
  {
    // large example
    // http://kirkmcdonald.github.io/posts/calculation.html
    const size_t M = 5;
    const size_t N = 6;
    Matrix<float, M, N> lhs({
        {-40, 0, 30, 10, 0, 0, 0},
        {30, -30, 30, 45, 0, 0, 0},
        {0, 20, 40, 55, 0, 0, 0},
        {-30, -30, 0, -50, 1, 0, 0},
        {0, 0, -100, -100, 0, 1, 0},

    });
    Array<float, N> obj({0, 0, 0, 0, 0, 1, 0});
    Array<float, M> rhs({10, 0, 45, 0, 0});
    Simplex s(obj, lhs, rhs, SIMPLEX_DEFAULT_GEQ_ARR(M), false);
    auto &solution = s.solution;
    cerr << solution << '\n';
    assert(solution.success);
    assert(Approx::isApproxEqual<float>(solution.res, 2300. / 39.,
                                        SIMPLEX_FLOAT_PRECISION));
    checkMatrix(solution.variables,
                vector<float>({0, 61. / 78., 8.0 / 39.0, 5.0 / 13, 555. / 13.,
                               2300. / 39.}));
  }
#endif
#if 0
  {
    // case 1. Should failed at Phase 1
    const size_t M = 3;
    const size_t N = 3;
    Matrix<float, M, N> lhs({
        {2, -1, -1},
        {2, -3, -1},
        {-1, 1, 1},
    });
    Array<float, N> obj({1, -1, 1});
    Array<float, M> rhs({4, -5, -1});
    Simplex s(obj, lhs, rhs, SIMPLEX_DEFAULT_GEQ_ARR(M), false);
    auto &solution = s.solution;
    cerr << solution << '\n';
    assert(!solution.success);
  }
#endif
#if 0
  {
    // degenerate example
    const size_t M = 3;
    const size_t N = 4;
    Matrix<float, M, N> lhs({
        {1. / 2, -11. / 2, -5. / 2, 9},
        {1. / 2, -3. / 2, -1. / 2, 1},
        {1, 0, 0, 0},
    });
    Array<float, N> obj({10, -57, -9, -24});
    Array<float, M> rhs({0, 0, 1});
    Simplex s(obj, lhs, rhs, SIMPLEX_DEFAULT_LEQ_ARR(M), true);
    auto &solution = s.solution;
    cerr << solution << '\n';
    assert(solution.success);
    assert(Approx::isApproxEqual<float>(solution.res, 1.,
                                        SIMPLEX_FLOAT_PRECISION));
    checkMatrix(solution.variables, vector<float>({1, 0, 1, 0}));
  }
#endif
#if 1
  {
    // large example
    // http://kirkmcdonald.github.io/posts/calculation.html
    const size_t M = 5;
    const size_t N = 5;
    Matrix<float, M, N> lhs({
        {0, -1, 1, 0, 0},
        {-1, 0, 0, 1, 0},
        {0, 1, 0, 0, 0},
        {1, 0, 0, 0, 0},
        {-1, -1, -1, -1, 1},

    });
    Array<float, N> obj({0, 100, 0, 0, 0});
    Array<float, M> rhs({0, 0, 0, 1000, 0});
    Simplex s(obj, lhs, rhs, SIMPLEX_DEFAULT_GEQ_ARR(M), false);
    auto &solution = s.solution;
    cerr << solution << '\n';
    assert(solution.success);
    assert(Approx::isApproxEqual<float>(solution.res, -2300. / 39.,
                                        SIMPLEX_FLOAT_PRECISION));
    checkMatrix(solution.variables,
                vector<float>({0, 61. / 78., 8.0 / 39.0, 5.0 / 13, 555. / 13.,
                               2300. / 39.}));
  }
#endif
}

int main() {
  simplex_method_basics();
  simplex_method_phase_2();
}
