#include <doctest/doctest.h>
//
#include <gsl/gsl_assert>  // for fail_fast (ptr only), Ensures, Expects
//
#include <iostream>

using namespace gsl;

namespace {
static constexpr char deathstring[] = "Expected Death";

int f(int i) {
  Expects(i > 0 && i < 10);
  return i;
}

int g(int i) {
  i++;
  Ensures(i > 0 && i < 10);
  return i;
}
}  // namespace

TEST_CASE("assertion_tests, expects") {
  std::set_terminate([] {
    std::cerr << "Expected Death. expects";
    std::abort();
  });

  CHECK(f(2) == 2);
  // EXPECT_DEATH(f(10), deathstring);
}

TEST_CASE("assertion_tests, ensures") {
  std::set_terminate([] {
    std::cerr << "Expected Death. ensures";
    std::abort();
  });

  CHECK(g(2) == 3);
  // EXPECT_DEATH(g(9), deathstring);
}
