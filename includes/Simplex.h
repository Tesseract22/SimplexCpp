#pragma once

#include "Array.h"
#include "Matrix.h"
#include <Approx.h>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <map>
#include <ostream>
#include <set>
#include <vector>

#define SIMPLEX_REVERSE_SIGN(b) (b ? 1 : -1)
#define SIMPLEX_DEFAULT_LEQ_ARR(M) Array<bool, M>(true)
#define SIMPLEX_DEFAULT_GEQ_ARR(M) Array<bool, M>(false)
template <size_t M, size_t N> class Simplex {
public:
  Simplex(const Array<float, N> &object, const Matrix<float, M, N> &ineq_lhs,
          const Array<float, M> &ineq_rhs,
          Array<bool, M> is_less_than = SIMPLEX_DEFAULT_LEQ_ARR(M),
          bool is_minimize = true)
      : is_less_than(is_less_than), is_minimize(is_minimize) {
    vars.row_basic = Array<size_t, M>(-1);
    solution = solveLP(object, ineq_lhs, ineq_rhs);
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
    float res = 0;
    bool success = false;
    Array<float, N> variables;
    size_t iterations = 0;
    friend std::ostream &operator<<(std::ostream &stream, const Solution &s) {
      stream << "Success: " << s.success << '\n';
      stream << "Result: " << s.res << '\n';
      stream << "Iterations: " << s.iterations << '\n';
      for (size_t i = 0; i < N; ++i) {
        stream << "x" << i << ": " << s.variables[i] << ' ';
      }
      stream << std::endl;
      return stream;
    }
  };
  Solution solution;

  template <size_t TM, size_t TN>
  Solution solveLP(Matrix<float, TM + 1, TN + TM + 2> &tab) {
    std::cout << "solve lp\n";
    Solution s;
    while (true) {
      s.iterations += 1;
      //   std::cout << "iterations: " << s.iterations << std::endl;
      //   std::cout << s << std::endl;
      s.res = tab.at(TM, TN + TM + 1) * SIMPLEX_REVERSE_SIGN(is_minimize);
      //   std::cout << "res " << s.res << std::endl;
      //   tab.debugPrint();
      //   std::cout << vars;a
      size_t pivot_col =
          getPivotCol<TM, TN>(tab, vars); // this would enter the basic vars
      if (pivot_col == TN + TM + 2) {
        s.success = true;
        // std::cout << s << std::endl;
        for (auto &it : vars.basic_vars) {
          if (it.first < TN)
            s.variables[it.first] = tab.at(it.second, TM + TN + 1);
        }
        return s;
      }
      size_t pivot_row = getPivotRow<TM, TN>(tab, pivot_col, vars);
      if (pivot_row == TM + 1) {
        return s;
      }
      size_t exiting_var = vars.row_basic[pivot_row];
      //   std::cout << "entering: " << pivot_col << "  exiting: " <<
      //   exiting_var
      //             << " pivot row: " << pivot_row << '\n';

      pivotVar(pivot_row, pivot_col);
      pivotMatrix<TM, TN>(tab, pivot_row, pivot_col);
    }
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
  Solution solveLP(const Array<float, N> &object,
                   const Matrix<float, M, N> &ineq_lhs,
                   const Array<float, M> &ineq_rhs) {

    if (!createTableau(object, ineq_lhs, ineq_rhs))
      return {};
    // https://www.matem.unam.mx/~omar/math340/2-phase.htmlhttps://www.matem.unam.mx/~omar/math340/2-phase.html
    // we need convert all negative rhs to positive
    // to do that, we first need to solve the auxiliary lp
    auto s = solveLP<M, N>(tab);
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
  bool createTableau(const Array<float, N> &object,
                     const Matrix<float, M, N> &ineq_lhs,
                     const Array<float, M> &ineq_rhs) {
    bool is_infeasible = false;

    size_t least_var = 0;
    size_t least_row = 0;
    float least = 0;
    // set all the regular entry
    for (size_t i = 0; i < M; ++i) {
      for (size_t j = 0; j < N; ++j) {
        tab.get(i, j) =
            ineq_lhs.at(i, j) * SIMPLEX_REVERSE_SIGN(is_less_than[i]);
        if (ineq_lhs.at(i, j) != 0) {
          // records the frequency of non_zero entry for each variables
        }
      }

      if (ineq_rhs[i] * SIMPLEX_REVERSE_SIGN(is_less_than[i]) < least) {
        least = ineq_rhs[i] * SIMPLEX_REVERSE_SIGN(is_less_than[i]);
        least_row = i;
        is_infeasible = true;
        //   std::cout << "least " << least_row <
      }
      size_t basic_row = N + i;
      vars.basic_vars.insert({N + i, i});
      vars.row_basic[i] = N + i;
      tab.get(i, N + i) = 1;
      tab.get(i, N + M + 1) =
          ineq_rhs[i] * SIMPLEX_REVERSE_SIGN(is_less_than[i]);
    }
    // find the basic vars
    for (size_t j = 0; j < N; ++j) {
      vars.non_basic_vars.insert(j);
    }
    // set objection function
    for (size_t j = 0; j < N; ++j) {
      tab.get(M, j) = -object[j] * SIMPLEX_REVERSE_SIGN(is_minimize);
    }
    // set the object "itself"
    tab.get(M, N + M) = 1;
    // tab.debugPrint();
    // std::cout << vars;
    // ----------------------------------------------------
    // if not feasible, we have to use the auxiliary matrix
    if (is_infeasible) {
      return solveAuxiliary(least_row);
      //   aux_tab.debugPrint();
    }

    return true;
  }
  bool solveAuxiliary(size_t pivot_row) {
    std::cout << "solve aux\n";

    Matrix<float, M + 1, N + M + 3> aux_tab; // one more variable
    size_t aux_var = N + M;                  // the new variable idx
    for (size_t i = 0; i < M; ++i) {
      for (size_t j = 0; j < aux_var; ++j) {
        aux_tab.get(i, j) = tab.at(i, j);
      }
    }
    for (size_t i = 0; i < M; ++i) {
      aux_tab.get(i, aux_var) = -1;
    }
    aux_tab.get(M, aux_var) = 1;
    aux_tab.get(M, aux_var + 1) = 1;
    for (size_t i = 0; i < M; ++i) {
      aux_tab.get(i, N + M + 2) = tab.get(i, N + M + 1);
    }
    std::ofstream f("b.txt");
    f << tab;
    pivotVar(pivot_row, aux_var);

    aux_tab.rowMultiplication(pivot_row, -1);
    for (size_t i = 0; i < M; ++i) {
      if (i != pivot_row)
        aux_tab.rowAddition(i, pivot_row, 1);
    }
    aux_tab.rowAddition(M, pivot_row, -1);
    //   aux_tab.debugPrint();
    //   std::cout << vars;
    Solution solution = solveLP<M, N + 1>(aux_tab);
    //   std::cout << solution << '\n';
    if (!solution.success ||
        !isApproxEqual<float>(solution.res, 0, SIMPLEX_FLOAT_PRECISION)) {
      return false;
    }
    // place it back to result
    for (size_t i = 0; i < M; ++i) {
      for (size_t j = 0; j < N + M; ++j) {
        tab.get(i, j) = aux_tab.at(i, j);
      }
    }
    for (size_t i = 0; i < M; ++i) {
      tab.get(i, N + M + 1) = aux_tab.get(i, N + M + 2);
    }
    // aux the object function
    for (const auto &it : vars.basic_vars) {
      size_t j = it.first;
      size_t i = it.second;
      if (tab.at(M, j) != 0) {
        tab.rowAddition(M, i, -tab.at(M, j));
      }
    }

    // std::cout << vars;
    // std::cout << tab;
    return true;
  }

  template <size_t TM, size_t TN>
  size_t getPivotCol(const Matrix<float, TM + 1, TN + TM + 2> &m, Vars &vars) {
    float min = 0;
    size_t col = TN + TM + 2;
    for (auto &j : vars.non_basic_vars) {
      if (m.at(TM, j) < min) {
        min = m.at(TM, j);
        col = j;
      }
      // if (m.at(M, j) < min) {
      //   min = m.at(M, j);
      //   col = j;
      // }
    }
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
  size_t getPivotRow(const Matrix<float, TM + 1, TN + TM + 2> &m,
                     size_t pivot_col, Vars &vars) {
    float min = __FLT_MAX__;
    size_t row = TM + 1;
    // @vector_multiplication
    for (auto &it : vars.basic_vars) {
      size_t i = it.second;
      float ratio = (float)m.at(i, TN + TM + 1) / (float)m.at(i, pivot_col);
      if (ratio >= 0 &&
          isDefLessThan<float>(ratio, min, SIMPLEX_FLOAT_PRECISION)) {
        min = ratio;
        row = i;
      }
    }
    return row;
  }

  template <size_t TM, size_t TN>
  void pivotMatrix(Matrix<float, TM + 1, TN + TM + 2> &m, size_t pivot_row,
                   size_t pivot_col) {
    float pivot_variable = m.at(pivot_row, pivot_col);
    m.rowMultiplication(pivot_row, 1 / pivot_variable);
    for (size_t i = 0; i < TM + 1; ++i) {
      if (i == pivot_row)
        continue;
      float r = m.at(i, pivot_col) / m.at(pivot_row, pivot_col);
      m.rowAddition(i, pivot_row, -r);
    }
    //   m.debugPrint();
  }
  Vars vars;
  Matrix<float, M + 1, M + N + 2> tab;
  float epsilon = SIMPLEX_FLOAT_PRECISION;
  Array<bool, M> is_less_than;
  bool is_minimize;
}; // namespace Simplex
