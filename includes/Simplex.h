#pragma once

#include "Array.h"
#include "Matrix.h"
#include <Approx.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <limits>
#include <map>
#include <ostream>
#include <set>
#include <vector>
#define SIMPLEX_RED "\x1b[31m"
#define SIMPLEX_GREEN "\x1b[32m"
#define SIMPLEX_YELLOW "\x1b[33m"
#define SIMPLEX_BLUE "\x1b[34m"
#define SIMPLEX_MAGENTA "\x1b[35m"
#define SIMPLEX_CYAN "\x1b[36m"
#define SIMPLEX_RESET "\x1b[0m"

#define SIMPLEX_REVERSE_SIGN(b) (b ? 1 : -1)
#define SIMPLEX_DEFAULT_LEQ_ARR(M) Array<bool, M>(true)
#define SIMPLEX_DEFAULT_GEQ_ARR(M) Array<bool, M>(false)

#ifdef SIMPLEX_DEBUG
#define LOG(msg) std::cout << msg;
#define LOGC(msg, color) std::cout << color << msg << SIMPLEX_RESET;

#else
#define LOG(msg)                                                               \
  do {                                                                         \
  } while (0);
#define LOGC(msg, color)                                                       \
  do {                                                                         \
  } while (0);
#endif
typedef float F;
template <size_t M, size_t N> class Simplex {
public:
  Simplex(const Array<F, N> &object, const Matrix<F, M, N> &ineq_lhs,
          const Array<F, M> &ineq_rhs,
          Array<bool, M> is_less_than = SIMPLEX_DEFAULT_LEQ_ARR(M),
          bool is_maximize = true, ssize_t iterations = -1)
      : is_less_than(is_less_than), is_maximize(is_maximize),
        iterations(iterations) {
    vars.row_basic = Array<size_t, M>(-1);
    solution = solveLP(object, ineq_lhs, ineq_rhs);
    LOG(tab)
  }

  struct Vars {
    std::map<size_t, size_t> basic_vars;
    std::set<size_t> non_basic_vars;
    Array<size_t, M> row_basic;
    friend std::ostream &operator<<(std::ostream &stream, const Vars &s) {
      stream << "Basic variables: \n";
      for (auto &it : s.basic_vars) {
        stream << it.first << " at row: " << it.second << '\n';
      }
      stream << "Non-basic variables: \n";
      for (auto &it : s.non_basic_vars) {
        stream << it << ' ';
      }
      stream << '\n';
      stream << "Basic variables for each row: \n";
      for (size_t i = 0; i < M; ++i) {
        stream << s.row_basic[i] << '\n';
      }
      stream << std::endl;
      return stream;
    }
  };
  struct Solution {
    F res = -std::numeric_limits<F>::max();
    bool success = false;
    Array<F, N> variables;
    size_t iterations = 0;
    friend std::ostream &operator<<(std::ostream &stream, const Solution &s) {
      stream << SIMPLEX_YELLOW << "Solution: " << SIMPLEX_RESET " ";
      stream << (s.success ? SIMPLEX_GREEN : SIMPLEX_RED)
             << "Success: " << s.success << ' ';
      stream << "Result: " << s.res << ' ';
      stream << "Iterations: " << s.iterations << SIMPLEX_RESET "\n";
      for (size_t i = 0; i < N; ++i) {
        stream << "x" << i << ": " << s.variables[i] << ' ';
      }
      stream << std::endl;
      return stream;
    }
  };
  Solution solution;

  template <size_t TM, size_t TN>

  Solution solveLP(Matrix<F, TM, TN> &tab) {
    LOGC("starting to solve...\n\n", SIMPLEX_GREEN)
    Solution s;
    s.res = -std::numeric_limits<F>::max();
    while (true) {
      s.iterations += 1;
      LOGC("Iterations: " << s.iterations << '\n', SIMPLEX_BLUE)
      LOG(tab)
      LOG(vars)
      //   std::cout << tab;
      //   std::cout << s << std::endl;
      //   for (size_t j = 0; j < TN + TM + 2; ++j)
      //     if (Approx::isApporxZero<F>(tab.at(TM, j), SIMP)) {
      //       //   std::cout << "zeroing: " << j << '\n';
      //       tab(TM, j) = 0;
      //     }

      if (s.res == tab.at(TM - 1, TN - 1)) {
        is_cycling = true;
        LOGC("Cycling!\n", SIMPLEX_RED)
      } else {
        is_cycling = false;
      }

      s.res = tab.at(TM - 1, TN - 1);

      LOGC("Result after iteration: " << s.res << '\n', SIMPLEX_BLUE)
      if (s.iterations > iterations) {
        LOGC("Too many iterations. Exiting\n\n", SIMPLEX_RED)
        return s;
      }

      //   std::cout << vars;a
      size_t pivot_col =
          getPivotCol<TM, TN>(tab, vars); // this would enter the basic vars
      if (pivot_col == TN) {
        LOGC("Simplex method find a solution. Exiting\n\n", SIMPLEX_RED)
        s.success = true;
        // std::cout << s << std::endl;
        for (auto &it : vars.basic_vars) {
          if (it.first < TN - TM) {
            s.variables[it.first] = tab.at(it.second, TN - 1);
          }
        }
        return s;
      }
      size_t pivot_row = getPivotRow<TM, TN>(tab, pivot_col, vars);
      if (pivot_row == TM) {
        LOGC("Simpelx method is unbound. Exiting\n\n.", SIMPLEX_RED)
        return s;
      }
      F a = tab.at(TM, pivot_col);
      size_t exiting_var = vars.row_basic[pivot_row];
      //   std::cout << "bottom: " << a << std::endl;

      LOGC("entering var (col): " << pivot_col
                                  << "; exiting var (col): " << exiting_var
                                  << "; pivot row: " << pivot_row << '\n',
           SIMPLEX_YELLOW)

      pivotVar(pivot_row, pivot_col);
      pivotMatrix<TM, TN>(tab, pivot_row, pivot_col);
      prev_enter_var = pivot_col;
      prev_exiting_var = exiting_var;
    }
    return {};
  }
  /**
   * @brief Solve the linear programming problem using the Simplex method with
   * the object function and constraint. This function would compose a new
   * matrix with the size of <M + 1, N + M + 2>, so a copy of the original
   * matrix is neccessary. In the future, we could implement some Matrix_view to
   * avoid copying. For information on the Simplext method
   * https://people.richland.edu/james/ictcm/2006/simplex.html
   *
   * By the default, It would try to `maximize` the `object`, with the
   * constraint being '<=' (less or equal than)
   *
   * @tparam M the row, or the number of constraints
   * @tparam N the col, or the number of variable
   * @param object The object with length <number of variables>, each entry
   * correspond to the coefficient of the corresponding variable
   * @param ineq_lhs The left-hand side of the constraint matrix. By default
   * using
   * '<'
   * @param ineq_rhs The right-hand side of the constraint matrix. By default
   * using
   * '<'
   * @return Solution
   */
  Solution solveLP(const Array<F, N> &object, const Matrix<F, M, N> &ineq_lhs,
                   const Array<F, M> &ineq_rhs) {

    if (!createTableau(object, ineq_lhs, ineq_rhs))
      return {};
    // https://www.matem.unam.mx/~omar/math340/2-phase.htmlhttps://www.matem.unam.mx/~omar/math340/2-phase.html
    // we need convert all negative rhs to positive
    // to do that, we first need to solve the auxiliary lp
    auto s = solveLP<M + 1, N + M + 1>(tab);
    s.res *= SIMPLEX_REVERSE_SIGN(is_maximize);
    return s;
  }

private:
  void pivotVar(size_t pivot_row, size_t pivot_col) {
    size_t exiting_var = vars.row_basic[pivot_row];
    vars.basic_vars.erase(exiting_var);
    vars.basic_vars.insert({pivot_col, pivot_row});
    vars.non_basic_vars.insert(exiting_var);
    vars.non_basic_vars.erase(pivot_col);
    vars.row_basic[pivot_row] = pivot_col;
  }
  /**
   * @brief create a tableau. The leq and geq, as well as minimize and maximize
   * are taken into account when creating. This function add slack variable to
   * transform every inequality constraint into equality constraint.
   *
   * The resulting tableau would have some row in rhs having negative value.
   * @param object
   * @param ineq_lhs
   * @param ineq_rhs
   * @return true
   * @return false
   */
  bool createTableau(const Array<F, N> &object, const Matrix<F, M, N> &ineq_lhs,
                     const Array<F, M> &ineq_rhs) {

    Array<bool, M> rhs_row_sign(true);
    // set all the regular entry
    for (size_t i = 0; i < M; ++i) {
      // this fill all the *norml* entry in the tab
      for (size_t j = 0; j < N; ++j) {
        tab.get(i, j) = ineq_lhs.at(i, j);
        if (ineq_lhs.at(i, j) != 0) {
          // records the frequency of non_zero entry for each variables
        }
      }
      // we would the flip the sign of the designated rows

      // this create the slack variable
      tab.get(i, N + i) = 1 * SIMPLEX_REVERSE_SIGN(is_less_than[i]);
      tab.get(i, N + M) = ineq_rhs[i];
      if (ineq_rhs[i] < 0) {
        rhs_row_sign.get(i) = false;
        //   std::cout << "least " << least_row <
      }
    }
    // set objection function
    for (size_t j = 0; j < N; ++j) {
      tab.get(M, j) = -object[j] * SIMPLEX_REVERSE_SIGN(is_maximize);
    }
    // set the object "itself"
    // tab.get(M, N + M) = 1;
    // ----------------------------------------------------
    // if not feasible, we have to use the auxiliary matrix
    return solveAuxiliary(rhs_row_sign);
    //   aux_tab.debugPrint();
  }
  bool solveAuxiliary(const Array<bool, M> &rhs_row_sign) {
    LOGC("starting to solve aux...\n", SIMPLEX_GREEN)
    const size_t old_obj = M;
    const size_t aux_obj = M;
    const size_t aux_rhs = N + M + M;
    const size_t old_rhs = N + M;
    // another M columns for auxiliary variables
    // another row for auxiliary objective
    Matrix<F, M + 1, N + M + M + 1> aux_tab; // one more variable
    // copy the original matrix                  // the new variable idx
    for (size_t i = 0; i < old_obj; ++i) {
      float mul = SIMPLEX_REVERSE_SIGN(rhs_row_sign.at(i));
      for (size_t j = 0; j < old_rhs; ++j) {
        aux_tab.get(i, j) = mul * tab.at(i, j);
      }
      // copy the rhs
      aux_tab.get(i, aux_rhs) = mul * tab.at(i, old_rhs);
      // create the auxiliary variables
      // this is always going to be +1, because we already ensure b >= 0
      aux_tab.get(i, N + M + i) = 1;
      // the auxiliary variables would be our basic variables
      vars.basic_vars.insert({N + M + i, i});
      vars.row_basic[i] = N + M + i;
    }
    // create the auxiliary objective

    for (size_t j = old_rhs; j < aux_rhs; ++j) {
      aux_tab.get(aux_obj, j) = 1;
      aux_tab.rowAddition(aux_obj, j - M - N, -1);
    }
    // the rest of the columns would be non-basic variables
    for (size_t j = 0; j < old_rhs; ++j) {
      vars.non_basic_vars.insert(j);
    }
    LOG(aux_tab)
    LOG(vars)
    Solution aux_solution = solveLP(aux_tab);
    LOG("auxiliary solution: : " << aux_solution)
    if (!aux_solution.success)
      return false;
    LOG(aux_tab)
    LOG(vars)

    for (size_t i = 0; i < old_obj; ++i) {
      // copy the lhs
      for (size_t j = 0; j < old_rhs; ++j) {
        tab.get(i, j) = aux_tab.at(i, j);
      }
      // copy the rhs
      tab.get(i, old_rhs) = aux_tab.at(i, aux_rhs);
    }
    for (size_t j = old_rhs; j < aux_rhs; ++j) {
      vars.non_basic_vars.erase(j);
    }
    for (size_t i = 0; i < old_obj; ++i) {
      size_t j = vars.row_basic.at(i);
      tab.rowAddition(old_obj, i, -tab.at(old_obj, j));
    }
    LOG(tab)
    LOG(vars)
    LOGC("TODO\n", SIMPLEX_RED)
    // std::cout << vars;
    // std::cout << tab;
    // LOGC("Successfully solved auxiliary matrix! :)\n\n", SIMPLEX_RED)
    return true;
  }

  template <size_t TM, size_t TN>
  size_t getPivotCol(const Matrix<F, TM, TN> &m, Vars &vars) {
    F min = 0;
    size_t col = TN;
    for (auto &j : vars.non_basic_vars) {
      if (Approx::isDefLessThan<F>(m.at(TM - 1, j), min,
                                   SIMPLEX_FLOAT_PRECISION)) {
        min = m.at(TM - 1, j);
        col = j;
        if (is_cycling)
          break;
      }
    }
    LOG("Choosing pivot column ")
    LOGC(col, SIMPLEX_YELLOW)
    LOG(" with value ")
    LOGC(min, SIMPLEX_YELLOW)
    LOG(" (pivot column with the most negative value, unless cycling)\n")

    return col;
  }
  /**
   * @brief Get the the pivot row, where first represent the row and second
   * represent the exiting basic variable
   *
   * @tparam M
   * @tparam N
   * @param m
   * @param pivot_col
   * @param vars
   * @return std::pair<size_t, size_t>
   */
  template <size_t TM, size_t TN>
  size_t getPivotRow(const Matrix<F, TM, TN> &m, size_t pivot_col, Vars &vars) {
    F min = __FLT_MAX__;
    size_t row = TM;
    // @vector_multiplication
    for (size_t i = 0; i < TM - 1; ++i) {
      F ratio;
      LOG(m.at(i, TN - 1) << ' ' << m.at(i, pivot_col) << '\n')
      LOG(m.at(i, TN - 1) / m.at(i, pivot_col) << '\n')
      if (Approx::isDefGreaterThan<F>(m.at(i, pivot_col), 0,
                                      SIMPLEX_FLOAT_PRECISION) &&
          (ratio = m.at(i, TN - 1) / m.at(i, pivot_col)) >= 0 &&
          Approx::isDefLessThan<F>(ratio, min, SIMPLEX_FLOAT_PRECISION)) {
        min = ratio;
        row = i;
        // if (is_cycling)
        //   break;
      }
    }

    LOG("Choosing pivot row ")
    LOGC(row, SIMPLEX_YELLOW)
    LOG(" with value ")
    LOGC(min, SIMPLEX_YELLOW)
    LOG(" (pivot row with the smallest non-negative ratio, unless cycling)\n")
    return row;
  }

  template <size_t TM, size_t TN>
  void pivotMatrix(Matrix<F, TM, TN> &m, size_t pivot_row, size_t pivot_col) {
    LOG("performing pivot...\n")
    F pivot_element = m.at(pivot_row, pivot_col);
    LOG("pivot element: ")
    LOGC(pivot_element << '\n', SIMPLEX_YELLOW)
    m.rowMultiplication(pivot_row, 1 / pivot_element);
    LOG("pivot element: ")
    LOGC(m.at(pivot_row, pivot_col) << '\n', SIMPLEX_CYAN)
    for (size_t i = 0; i < TM + 1; ++i) {
      if (i == pivot_row)
        continue;
      F r = m.at(i, pivot_col) / m.at(pivot_row, pivot_col);
      m.rowAddition(i, pivot_row, -r);
    }
    //   m.debugPrint();
  }
  Vars vars;
  // one extra row for objective function
  // M extra column for slack variable
  // 1 extra column for rhs
  Matrix<F, M + 1, M + N + 1> tab;
  F epsilon = SIMPLEX_FLOAT_PRECISION;
  Array<bool, M> is_less_than;
  bool is_maximize;
  bool is_cycling = false;
  size_t prev_enter_var = M + N + 3;
  size_t prev_exiting_var = M + N + 3;
  ssize_t iterations = -1;

  //   const size_t obj_row = M + N + 1
}; // namespace Simplex
