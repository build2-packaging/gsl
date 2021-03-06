#include <doctest/doctest.h>
//
#include <gsl/gsl_util>  // for at
//
#include <array>             // for array
#include <cstddef>           // for size_t
#include <initializer_list>  // for initializer_list
#include <iostream>
#include <vector>  // for vector

namespace {
static constexpr char deathstring[] = "Expected Death";
}

TEST_CASE("at_tests, static_array") {
  int a[4] = {1, 2, 3, 4};
  const int(&c_a)[4] = a;

  for (int i = 0; i < 4; ++i) {
    CHECK(&gsl::at(a, i) == &a[i]);
    CHECK(&gsl::at(c_a, i) == &a[i]);
  }

  std::set_terminate([] {
    std::cerr << "Expected Death. static_array";
    std::abort();
  });

  // EXPECT_DEATH(gsl::at(a, -1), deathstring);
  // EXPECT_DEATH(gsl::at(a, 4), deathstring);
  // EXPECT_DEATH(gsl::at(c_a, -1), deathstring);
  // EXPECT_DEATH(gsl::at(c_a, 4), deathstring);
}

TEST_CASE("at_tests, std_array") {
  std::array<int, 4> a = {1, 2, 3, 4};
  const std::array<int, 4>& c_a = a;

  for (int i = 0; i < 4; ++i) {
    CHECK(&gsl::at(a, i) == &a[static_cast<std::size_t>(i)]);
    CHECK(&gsl::at(c_a, i) == &a[static_cast<std::size_t>(i)]);
  }

  std::set_terminate([] {
    std::cerr << "Expected Death. std_array";
    std::abort();
  });

  // EXPECT_DEATH(gsl::at(a, -1), deathstring);
  // EXPECT_DEATH(gsl::at(a, 4), deathstring);
  // EXPECT_DEATH(gsl::at(c_a, -1), deathstring);
  // EXPECT_DEATH(gsl::at(c_a, 4), deathstring);
}

TEST_CASE("at_tests, std_vector") {
  std::vector<int> a = {1, 2, 3, 4};
  const std::vector<int>& c_a = a;

  for (int i = 0; i < 4; ++i) {
    CHECK(&gsl::at(a, i) == &a[static_cast<std::size_t>(i)]);
    CHECK(&gsl::at(c_a, i) == &a[static_cast<std::size_t>(i)]);
  }

  std::set_terminate([] {
    std::cerr << "Expected Death. std_vector";
    std::abort();
  });

  // EXPECT_DEATH(gsl::at(a, -1), deathstring);
  // EXPECT_DEATH(gsl::at(a, 4), deathstring);
  // EXPECT_DEATH(gsl::at(c_a, -1), deathstring);
  // EXPECT_DEATH(gsl::at(c_a, 4), deathstring);
}

TEST_CASE("at_tests, InitializerList") {
  const std::initializer_list<int> a = {1, 2, 3, 4};

  for (int i = 0; i < 4; ++i) {
    CHECK(gsl::at(a, i) == i + 1);
    CHECK(gsl::at({1, 2, 3, 4}, i) == i + 1);
  }

  std::set_terminate([] {
    std::cerr << "Expected Death. InitializerList";
    std::abort();
  });

  // EXPECT_DEATH(gsl::at(a, -1), deathstring);
  // EXPECT_DEATH(gsl::at(a, 4), deathstring);
  // EXPECT_DEATH(gsl::at({1, 2, 3, 4}, -1), deathstring);
  // EXPECT_DEATH(gsl::at({1, 2, 3, 4}, 4), deathstring);
}

#if !defined(_MSC_VER) || defined(__clang__) || _MSC_VER >= 1910
static constexpr bool test_constexpr() {
  int a1[4] = {1, 2, 3, 4};
  const int(&c_a1)[4] = a1;
  std::array<int, 4> a2 = {1, 2, 3, 4};
  const std::array<int, 4>& c_a2 = a2;

  for (int i = 0; i < 4; ++i) {
    if (&gsl::at(a1, i) != &a1[i]) return false;
    if (&gsl::at(c_a1, i) != &a1[i]) return false;
    // requires C++17:
    // if (&gsl::at(a2, i) != &a2[static_cast<std::size_t>(i)]) return false;
    if (&gsl::at(c_a2, i) != &c_a2[static_cast<std::size_t>(i)]) return false;
    if (gsl::at({1, 2, 3, 4}, i) != i + 1) return false;
  }

  return true;
}

static_assert(test_constexpr(), "FAIL");
#endif
