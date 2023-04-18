#include "Array.h"
#include "Matrix.h"
#include "Simplex.h"
#include <array>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;
int main() {
  const int M = 2;
  const int N = 5; // 3 constraints and 2 variables
  // float ineq_lhs[M][N] = {
  //     {1, 1},
  //     {2, 1},
  // };
  Array<float, M> ineq_rhs({12, 16});
  Array<float, N> obj({40, 30});
  Matrix<float, M, N> l({
      {1, 1},
      {2, 1},
  });
  // // auto t = ToTableau<M, N>(obj, l, ineq_rhs);
  // // // t.rowAddition(0, 1);
  // // t.debugPrint();

  // // auto pivot_col = getPivotCol<M, N>(t);
  // // auto pivot_row = getPivotRow<M, N>(t, pivot_col);

  // // cout << pivot_col << ' '<< pivot_row << endl;
  // // pivotMatrix<M, N>(t, pivot_row, pivot_col);
  cout << Simplex::solveLP<M, N>(obj, l, ineq_rhs);

  // cout << l[0][0] << endl;

  return 0;
}