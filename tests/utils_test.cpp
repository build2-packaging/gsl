#include <doctest/doctest.h>
//
#include <gsl/gsl_util>  // for narrow, finally, narrow_cast, narrowing_e...
//
#include <stdint.h>  // for uint32_t, int32_t

#include <algorithm>    // for move
#include <cstddef>      // for std::ptrdiff_t
#include <functional>   // for reference_wrapper, _Bind_helper<>::type
#include <limits>       // for numeric_limits
#include <type_traits>  // for is_same

using namespace gsl;

namespace {
void f(int& i) { i += 1; }
static int j = 0;
void g() { j += 1; }
}  // namespace

TEST_CASE("utils_tests, sanity_check_for_gsl_index_typedef") {
  static_assert(std::is_same<gsl::index, std::ptrdiff_t>::value,
                "gsl::index represents wrong arithmetic type");
}

TEST_CASE("utils_tests, finally_lambda") {
  int i = 0;
  {
    auto _ = finally([&]() { f(i); });
    CHECK(i == 0);
  }
  CHECK(i == 1);
}

TEST_CASE("utils_tests, finally_lambda_move") {
  int i = 0;
  {
    auto _1 = finally([&]() { f(i); });
    {
      auto _2 = std::move(_1);
      CHECK(i == 0);
    }
    CHECK(i == 1);
    {
      auto _2 = std::move(_1);
      CHECK(i == 1);
    }
    CHECK(i == 1);
  }
  CHECK(i == 1);
}

TEST_CASE("utils_tests, finally_function_with_bind") {
  int i = 0;
  {
    auto _ = finally(std::bind(&f, std::ref(i)));
    CHECK(i == 0);
  }
  CHECK(i == 1);
}

TEST_CASE("utils_tests, finally_function_ptr") {
  j = 0;
  {
    auto _ = finally(&g);
    CHECK(j == 0);
  }
  CHECK(j == 1);
}

TEST_CASE("utils_tests, narrow_cast") {
  int n = 120;
  char c = narrow_cast<char>(n);
  CHECK(c == 120);

  n = 300;
  unsigned char uc = narrow_cast<unsigned char>(n);
  CHECK(uc == 44);
}

TEST_CASE("utils_tests, narrow") {
  int n = 120;
  const char c = narrow<char>(n);
  CHECK(c == 120);

  n = 300;
  CHECK_THROWS_AS(narrow<char>(n), narrowing_error);

  const auto int32_max = std::numeric_limits<int32_t>::max();
  const auto int32_min = std::numeric_limits<int32_t>::min();

  CHECK(narrow<uint32_t>(int32_t(0)) == 0);
  CHECK(narrow<uint32_t>(int32_t(1)) == 1);
  CHECK(narrow<uint32_t>(int32_max) == static_cast<uint32_t>(int32_max));

  CHECK_THROWS_AS(narrow<uint32_t>(int32_t(-1)), narrowing_error);
  CHECK_THROWS_AS(narrow<uint32_t>(int32_min), narrowing_error);

  n = -42;
  CHECK_THROWS_AS(narrow<unsigned>(n), narrowing_error);
}
