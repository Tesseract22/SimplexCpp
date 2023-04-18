#include "Array.h"
#include "Matrix.h"
#include "Simplex.h"
#include <array>
#include <bits/types/FILE.h>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;
int main() {
  const int M = 211;
  const int N = 209; // 3 constraints and 2 variables
  // Array<double, 10> arr2("data/test.mat");
  // arr2.debugPrint();
  Matrix<float, M, N> matrix("data/test.mat");
  matrix.debugPrint();
  // cout << sizeof(float) << ' ';

  // FILE *f = fopen("data/test.mat", "r");
  // cout << getcwd() << endl;
  return 0;
}