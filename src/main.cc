#include "Approx.h"
#include "Array.h"
#include "Matrix.h"
#include "Simplex.h"
#include <fstream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
using namespace std;
int main() {
  Matrix<double, 3, 7> a({
      {1, 1, 1, 1, 1, 1, 1},
      {2, 2, 2, 2, 2, 2, 2},
      {3, 3, 3, 3, 3, 3, 3},
  });
  a.rowAddition(0, 1, 2);
  cout << a;
}