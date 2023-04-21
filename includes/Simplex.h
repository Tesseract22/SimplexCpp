#pragma once

#include "Array.h"
#include "Matrix.h"
#include <Approx.h>
#include <cstddef>
#include <iostream>
#include <map>
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
    size_t it = 0;
    friend std::ostream &operator<<(std::ostream &stream, const Solution &s) {
      stream << "Success: " << s.success << '\n';
      stream << "Result: " << s.res << '\n';
      stream << "Iterations: " << s.it << '\n';
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
    Solution s;
    while (true) {
      s.it += 1;
      //   std::cout << "iterations: " << s.it << std::endl;
      s.res = tab.at(TM, TN + TM + 1);
      //   std::cout << "res " << s.res << std::endl;
      //   tab.debugPrint();
      //   std::cout << vars;
      size_t pivot_col =
          getPivotCol<TM, TN>(tab, vars); // this would enter the basic vars
      if (pivot_col == TN + TM + 2) {
        s.success = true;

        for (auto &it : vars.basic_vars) {
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
  template <size_t TM, size_t TN>
  Solution solveLP(const Array<float, TN> &object,
                   const Matrix<float, TM, TN> &ineq_lhs,
                   const Array<float, TM> &ineq_rhs) {

    Matrix<float, TM + 1, TN + TM + 2> tab =
        toTableau<TM, TN>(object, ineq_lhs, ineq_rhs);
    // https://www.matem.unam.mx/~omar/math340/2-phase.htmlhttps://www.matem.unam.mx/~omar/math340/2-phase.html
    // we need convert all negative rhs to positive
    // to do that, we first need to solve the auxiliary lp
    // tab.debugPrint();
    return solveLP<TM, TN>(tab);
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
  template <size_t TM, size_t TN>
  Matrix<float, TM + 1, N + TM + 2>
  toTableau(const Array<float, TN> &object,
            const Matrix<float, TM, TN> &ineq_lhs,
            const Array<float, TM> &ineq_rhs) {
    Matrix<float, TM + 1, TN + TM + 2> tab;

    bool is_infeasible = false;

    size_t least_var = 0;
    size_t least_row = 0;
    float least = 0;

    Array<size_t, TN> basic_var_arr; // if basic var, each entry denote at which
                                     // row it is basic
    Array<size_t, TN> count_arr;
    // set all the regular entry
    for (size_t i = 0; i < TM; ++i) {
      for (size_t j = 0; j < TN; ++j) {
        tab.get(i, j) =
            ineq_lhs.at(i, j) * SIMPLEX_REVERSE_SIGN(is_less_than[i]);
        if (ineq_lhs.at(i, j) != 0) {
          // records the frequency of non_zero entry for each variables
          count_arr[j]++;
          basic_var_arr[j] = i;
        }
      }

      if (ineq_rhs[i] * SIMPLEX_REVERSE_SIGN(is_less_than[i]) < least) {
        least = ineq_rhs[i] * SIMPLEX_REVERSE_SIGN(is_less_than[i]);
        least_row = i;
        is_infeasible = true;
        //   std::cout << "least " << least_row <
      }
      size_t basic_row = TN + i;
      vars.basic_vars.insert({TN + i, i});
      vars.row_basic[i] = TN + i;
      tab.get(i, TN + i) = 1;
      tab.get(i, TN + TM + 1) =
          ineq_rhs[i] * SIMPLEX_REVERSE_SIGN(is_less_than[i]);
    }
    // find the basic vars
    for (size_t j = 0; j < TN; ++j) {
      //   size_t row = basic_var_arr[j];
      //   if (count_arr[j] == 1 && vars.basic_vars[row] != -1) {
      //     vars.basic_vars.insert({j, row});
      //     vars.row_basic[row] = j;
      //   } else
      vars.non_basic_vars.insert(j);
    }
    // set objection function
    for (size_t j = 0; j < TN; ++j) {
      tab.get(TM, j) = -object[j] * SIMPLEX_REVERSE_SIGN(is_minimize);
    }
    // set the object "itself"
    tab.get(TM, TN + TM) = 1;
    // tab.debugPrint();
    // std::cout << vars;
    // ----------------------------------------------------
    // if not feasible, we have to use the auxiliary matrix
    if (is_infeasible) {

      Matrix<float, TM + 1, TN + TM + 3> aux_tab; // one more variable
      size_t aux_var = TN + TM;                   // the new variable idx
      for (size_t i = 0; i < TM; ++i) {
        for (size_t j = 0; j < aux_var; ++j) {
          aux_tab.get(i, j) = tab.at(i, j);
        }
      }
      for (size_t i = 0; i < TM; ++i) {
        aux_tab.get(i, aux_var) = -1;
      }
      aux_tab.get(TM, aux_var) = 1;
      aux_tab.get(TM, aux_var + 1) = 1;
      for (size_t i = 0; i < TM; ++i) {
        aux_tab.get(i, TN + TM + 2) = tab.get(i, TN + TM + 1);
      }
      //   aux_tab.debugPrint();
      size_t pivot_row = least_row;
      // vars.basic_vars.insert({aux_var, })

      //   std::cout << "pivot ";
      //   std::cout << pivot_row << ' ' << aux_var << '\n';

      pivotVar(pivot_row, aux_var);

      aux_tab.rowMultiplication(pivot_row, -1);
      for (size_t i = 0; i < TM; ++i) {
        if (i != pivot_row)
          aux_tab.rowAddition(i, pivot_row, 1);
      }
      aux_tab.rowAddition(TM, pivot_row, -1);
      //   aux_tab.debugPrint();
      //   std::cout << vars;
      Solution solution = solveLP<TM, TN + 1>(aux_tab);
      //   std::cout << solution << '\n';
      if (!solution.success ||
          !isApproxEqual<float>(solution.res, 0, SIMPLEX_FLOAT_PRECISION)) {
        return {};
      }

      //   aux_tab.debugPrint();

      // place it back to result
      for (size_t i = 0; i < M; ++i) {
        for (size_t j = 0; j < TN + M; ++j) {
          tab.get(i, j) = aux_tab.at(i, j);
        }
      }
      for (size_t i = 0; i < M; ++i) {
        tab.get(i, TN + M + 1) = aux_tab.get(i, TN + M + 2);
      }
      // aux the object function
      for (const auto &it : vars.basic_vars) {
        size_t j = it.first;
        size_t i = it.second;
        if (tab.at(M, j) != 0) {
          tab.rowAddition(M, i, -tab.at(M, j));
        }
      }
      //   aux_tab.debugPrint();
    }

    return tab;
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
  float epsilon = SIMPLEX_FLOAT_PRECISION;
  Array<bool, M> is_less_than;
  bool is_minimize;
}; // namespace Simplex
