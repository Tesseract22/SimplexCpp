#include "Array.h"
#include "Matrix.h"
#include "Simplex.h"
#include <array>
#include <bits/types/FILE.h>
#include <cstdlib>
#include <iostream>
#include <stdio.h>
#include <string>
using namespace std;
int main() {
  const int M = 2;
  const int N = 5; // 3 constraints and 2 variables
  Matrix<float, 2, 5> arr({
      {1, 2, 3, 4, 5},
      {5, 4, 3, 2, 1},
  });
  arr.debugPrint();
  ssize_t i = arr.save("data/test.mat");
  cout << i << '\n';
  Array<float, 5> arr2("data/test.mat");
  arr2.debugPrint();

  // FILE *f = fopen("data/test.mat", "r");
  // cout << getcwd() << endl;
  return 0;
}