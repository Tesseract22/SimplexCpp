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
  Array<float, 5> a({1, 2, 3, 4, 5});
  Array<float, 5> b({1, 2, 3, 4, 5});

  Array<float, 5>::arrayAddition(a, b, 5);
  cout << a << endl;

  Matrix<float, 3, 3> lhs({
      {2, -1, 2},
      {2, -3, 1},
      {-1, 1, -2},
  });
  lhs.precision = 1;
  cout << lhs << endl;
  return 0;
}