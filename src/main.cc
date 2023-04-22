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

  //   Array<float, 5>::arrayAddition(a, b, 5);

  Array<string, 5> name({"aaaaaa", "bb", "cc", "aaa", "bbbb"});
  a.header = &name;
  cout << a << endl;
  return 0;
}