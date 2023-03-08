#include "Simplex.h"
#include "Matrix.h"
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <array>
using namespace std;
int main() {
    const int M = 3; const int N = 2; // 3 constraints and 2 variables
    float ineq_lhs[M][N] = {
        {1, 2},
        {1, 1},
        {3, 2},
    };
    array<float, M> ineq_rhs = {16, 9, 24};
    array<float, N> obj = {40, 30};
    Matrix<float, 3, 2> l(ineq_lhs);
    // auto t = ToTableau<M, N>(obj, l, ineq_rhs);
    // // t.rowAddition(0, 1);
    // t.debugPrint();

    // auto pivot_col = getPivotCol<M, N>(t);
    // auto pivot_row = getPivotRow<M, N>(t, pivot_col);

    // cout << pivot_col << ' '<< pivot_row << endl;
    // pivotMatrix<M, N>(t, pivot_row, pivot_col);
    cout << solveLP<M, N>(obj, l, ineq_rhs).res << endl;
    return 0;
}