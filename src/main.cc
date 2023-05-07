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
  Matrix<float, 2, 3> f;
  f.save("a");

  //   Matrix<float, 2, 3> f2("a");
  //   cout << f2;
}