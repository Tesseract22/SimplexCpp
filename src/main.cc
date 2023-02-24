#include "Simplex.h"
#include "Matrix.h"
#include <iostream>
#include <cstdlib>
using namespace std;
int main() {
    const int M = 3; const int N = 2; // 3 constraints and 2 variables
    float ineq_lhs[M][N] = {
        {1, 2},
        {1, 1},
        {3, 2},
    };
    float ineq_rhs[M] = {16, 9, 24};
    float obj[N] = {40, 30};
    cout << ineq_lhs[0][0] << endl;
    auto res = ToTableau(3, 2, obj, (float**)ineq_lhs, ineq_rhs);
    // for (int i = 0; i < 3; ++i) {
    //     for (int j = 0; j < 2; ++j) {
    //         cout << res[i][j] << endl;
    //     }
    // }
    (void)res;

    Matrix<float, 3, 2> m;
    m.debugPrint();
    return 0;
}