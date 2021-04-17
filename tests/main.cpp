#include <cassert>
#include <iostream>
//
#include <gsl/gsl>

using namespace std;
using namespace gsl;

int main() {
  int v = 42;
  not_null<int*> p = &v;
  assert(typeid(*p) == typeid(*(&v)));
  *p = 43;
  assert(v == 43);
}