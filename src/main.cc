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
int main() { std::cout << Approx::isDefLessThan<float>(1e-5, 1e-4) << '\n'; }