

float**  ToTableau(int m, int n, float* object, float** ineq_lhs, float* ineq_rhs) {
    float** res = (float**)malloc(sizeof(float*) * m);
    for (int row = 0; row < m; ++row) {
        res[row] = (float*)malloc(sizeof(float) * (m + n + 1));
        for (int col = 0; col < n; ++col) {
            res[row][col] = ineq_lhs[row][col];
        }
        res[row][row + n] = 1;
        res[row][m + n] = ineq_rhs[row];
    }

    return res;
}