#include <doctest/doctest.h>
//
#include <gsl/pointers>  // for owner

using namespace gsl;

GSL_SUPPRESS(f .23)  // NO-FORMAT: attribute
void f(int* i) { *i += 1; }

TEST_CASE("owner_tests, basic_test") {
  owner<int*> p = new int(120);
  CHECK(*p == 120);
  f(p);
  CHECK(*p == 121);
  delete p;
}

TEST_CASE("owner_tests, check_pointer_constraint") {
#ifdef CONFIRM_COMPILATION_ERRORS
  {
    owner<int> integerTest = 10;
    owner<std::shared_ptr<int>> sharedPtrTest(new int(10));
  }
#endif
}
