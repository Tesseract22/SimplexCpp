#include "Array.h"
#include "Engine/LuaState.hpp"
#include "Engine/LuaTNumber.hpp"
#include "Engine/LuaTString.hpp"
#include "Engine/LuaType.hpp"
#include "Matrix.h"
#include "Simplex.h"
#include "lua.h"
#include <LuaCpp.hpp>
#include <fstream>
#include <memory>
#include <ostream>
#include <string>
using namespace std;
int main() {
  Array<float, 5> a({1, 2, 3, 4, 5});
  Array<float, 5> b({1, 2, 3, 4, 5});

  Array<float, 5>::arrayAddition(a, b, 5);
  a.debugPrint();
  return 0;
}