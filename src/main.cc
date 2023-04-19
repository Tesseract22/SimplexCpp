#include "Array.h"
#include "Engine/LuaState.hpp"
#include "Engine/LuaTNumber.hpp"
#include "Engine/LuaTString.hpp"
#include "Engine/LuaType.hpp"
#include "LuaMatrix.h"
#include "LuaStrArray.h"
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

  const string &a = "aaa";
  const int M = 212;
  const int N = 220;
  LuaCpp::LuaContext lua;
  LuaMatrix<float, M, N> t;
  //   lua.CompileString("test", "foo[1] = 2 print(foo[1])");
  std::shared_ptr<LuaCpp::Registry::LuaLibrary> lib =
      std::make_shared<LuaCpp::Registry::LuaLibrary>("MATRIX");
  lib->AddCFunction("index", index);
  lua.AddLibrary(lib);
  //   lua.CompileStringAndRun("print(MATRIX.index(212,219,1,1))");

  //   lua.CompileFile("test", "./matrix.lua");
  int res;
  LuaCpp::Engine::LuaTNumber luaM(M);
  LuaCpp::Engine::LuaTNumber luaN(N);

  lua.CompileFile("main", "./matrix.lua");
  lua.CompileString("test", "print(m)");
  LuaMatrix<float, M, N> luaMatrix;
  LuaStringArray items;
  LuaStringArray recipes;
  LuaStringArray raws;

  std::unique_ptr<LuaCpp::Engine::LuaState> state = lua.newStateFor("main");

  luaM.PushGlobal(*state, "M");
  luaN.PushGlobal(*state, "N");
  luaMatrix.PushGlobal(*state, "matrix");
  items.PushGlobal(*state, "citems");
  recipes.PushGlobal(*state, "crecipes");
  raws.PushGlobal(*state, "craws");

  res = lua_pcall(*state, 0, LUA_MULTRET, 0);
  if (res != LUA_OK) {
    std::cout << "Error Executing " << res << " " << lua_tostring(*state, 1)
              << "\n";
  }

  //   lua.CompileStringAndRun();
  //   Matrix<float, M, N> matrix("data/test.mat");
  //   obj.matrix.debugPrint();;
  //   ofstream out1("data/matrix.txt");
  //   std::cout.rdbuf(out1.rdbuf());
  //   luaMatrix.matrix.debugPrint();

  //   luaMatrix.matrix.save("data/matrix.mat");

  cout << items.vec.size() << endl;
  for (auto &s : raws.vec) {
    cout << s << ' ';
  }
  return 0;
}