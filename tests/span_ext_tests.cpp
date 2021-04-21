#include <doctest/doctest.h>
//
#include <gsl/gsl_util>  // for narrow_cast, at
#include <gsl/span_ext>  // for operator==, operator!=, make_span
//
#include <array>     // for array
#include <iostream>  // for cerr
#include <vector>    // for vector

using namespace std;
using namespace gsl;

namespace {
static constexpr char deathstring[] = "Expected Death";
}  // namespace

TEST_CASE("span_ext_test, make_span_from_pointer_length_constructor") {
  std::set_terminate([] {
    std::cerr << "Expected Death. from_pointer_length_constructor";
    std::abort();
  });
  int arr[4] = {1, 2, 3, 4};

  {
    auto s = make_span(&arr[0], 2);
    CHECK(s.size() == 2);
    CHECK(s.data() == &arr[0]);
    CHECK(s[0] == 1);
    CHECK(s[1] == 2);
  }

  {
    int* p = nullptr;
    auto s = make_span(p, narrow_cast<span<int>::size_type>(0));
    CHECK(s.size() == 0);
    CHECK(s.data() == nullptr);
  }

  {
    int* p = nullptr;
    auto workaround_macro = [=]() { make_span(p, 2); };
    // EXPECT_DEATH(workaround_macro(), deathstring);
  }
}

TEST_CASE("span_ext_test, make_span_from_pointer_pointer_construction") {
  int arr[4] = {1, 2, 3, 4};

  {
    auto s = make_span(&arr[0], &arr[2]);
    CHECK(s.size() == 2);
    CHECK(s.data() == &arr[0]);
    CHECK(s[0] == 1);
    CHECK(s[1] == 2);
  }

  {
    auto s = make_span(&arr[0], &arr[0]);
    CHECK(s.size() == 0);
    CHECK(s.data() == &arr[0]);
  }

  {
    int* p = nullptr;
    auto s = make_span(p, p);
    CHECK(s.size() == 0);
    CHECK(s.data() == nullptr);
  }
}

TEST_CASE("span_ext_test, make_span_from_array_constructor") {
  int arr[5] = {1, 2, 3, 4, 5};
  int arr2d[2][3] = {1, 2, 3, 4, 5, 6};
  int arr3d[2][3][2] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

  {
    const auto s = make_span(arr);
    CHECK(s.size() == 5);
    CHECK(s.data() == std::addressof(arr[0]));
  }

  {
    const auto s = make_span(std::addressof(arr2d[0]), 1);
    CHECK(s.size() == 1);
    CHECK(s.data() == std::addressof(arr2d[0]));
  }

  {
    const auto s = make_span(std::addressof(arr3d[0]), 1);
    CHECK(s.size() == 1);
    CHECK(s.data() == std::addressof(arr3d[0]));
  }
}

TEST_CASE("span_ext_test, make_span_from_dynamic_array_constructor") {
  double(*arr)[3][4] = new double[100][3][4];

  {
    auto s = make_span(&arr[0][0][0], 10);
    CHECK(s.size() == 10);
    CHECK(s.data() == &arr[0][0][0]);
  }

  delete[] arr;
}

TEST_CASE("span_ext_test, make_span_from_std_array_constructor") {
  std::array<int, 4> arr = {1, 2, 3, 4};

  {
    auto s = make_span(arr);
    CHECK(s.size() == arr.size());
    CHECK(s.data() == arr.data());
  }

  // This test_CASE "checks for the" bug found in
  // gcc 6.1, 6.2, 6.3, 6.4, 6.5 7.1, 7.2, 7.3 - issue #590
  {
    span<int> s1 = make_span(arr);

    static span<int> s2;
    s2 = s1;

#if defined(__GNUC__) && __GNUC__ == 6 &&           \
    (__GNUC_MINOR__ == 4 || __GNUC_MINOR__ == 5) && \
    __GNUC_PATCHLEVEL__ == 0 && defined(__OPTIMIZE__)
    // Known to be broken in gcc 6.4 and 6.5 with optimizations
    // Issue in gcc: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=83116
    CHECK(s1.size() == 4);
    CHECK(s2.size() == 0);
#else
    CHECK(s1.size() == s2.size());
#endif
  }
}

TEST_CASE("span_ext_test, make_span_from_const_std_array_constructor") {
  const std::array<int, 4> arr = {1, 2, 3, 4};

  {
    auto s = make_span(arr);
    CHECK(s.size() == arr.size());
    CHECK(s.data() == arr.data());
  }
}

TEST_CASE("span_ext_test, make_span_from_std_array_const_constructor") {
  std::array<const int, 4> arr = {1, 2, 3, 4};

  {
    auto s = make_span(arr);
    CHECK(s.size() == arr.size());
    CHECK(s.data() == arr.data());
  }
}

TEST_CASE("span_ext_test, make_span_from_container_constructor") {
  std::vector<int> v = {1, 2, 3};
  const std::vector<int> cv = v;

  {
    auto s = make_span(v);
    CHECK(s.size() == v.size());
    CHECK(s.data() == v.data());

    auto cs = make_span(cv);
    CHECK(cs.size() == cv.size());
    CHECK(cs.data() == cv.data());
  }
}

TEST_CASE("span_test, interop_with_gsl_at") {
  int arr[5] = {1, 2, 3, 4, 5};
  span<int> s{arr};
  CHECK(at(s, 0) == 1);
  CHECK(at(s, 1) == 2);
}

TEST_CASE("span_ext_test, iterator_free_functions") {
  int a[] = {1, 2, 3, 4};
  span<int> s{a};

  CHECK((std::is_same<decltype(s.begin()), decltype(begin(s))>::value));
  CHECK((std::is_same<decltype(s.end()), decltype(end(s))>::value));

  CHECK((std::is_same<decltype(std::cbegin(s)), decltype(cbegin(s))>::value));
  CHECK((std::is_same<decltype(std::cend(s)), decltype(cend(s))>::value));

  CHECK((std::is_same<decltype(s.rbegin()), decltype(rbegin(s))>::value));
  CHECK((std::is_same<decltype(s.rend()), decltype(rend(s))>::value));

  CHECK((std::is_same<decltype(std::crbegin(s)), decltype(crbegin(s))>::value));
  CHECK((std::is_same<decltype(std::crend(s)), decltype(crend(s))>::value));

  CHECK(s.begin() == begin(s));
  CHECK(s.end() == end(s));

  CHECK(s.rbegin() == rbegin(s));
  CHECK(s.rend() == rend(s));

  CHECK(s.begin() == cbegin(s));
  CHECK(s.end() == cend(s));

  CHECK(s.rbegin() == crbegin(s));
  CHECK(s.rend() == crend(s));
}

TEST_CASE("span_ext_test, ssize_free_function") {
  int a[] = {1, 2, 3, 4};
  span<int> s{a};

  CHECK_FALSE((std::is_same<decltype(s.size()), decltype(ssize(s))>::value));
  CHECK(s.size() == static_cast<std::size_t>(ssize(s)));
}

TEST_CASE("span_ext_test, comparison_operators") {
  {
    span<int> s1;
    span<int> s2;
    CHECK(s1 == s2);
    CHECK_FALSE(s1 != s2);
    CHECK_FALSE(s1 < s2);
    CHECK(s1 <= s2);
    CHECK_FALSE(s1 > s2);
    CHECK(s1 >= s2);
    CHECK(s2 == s1);
    CHECK_FALSE(s2 != s1);
    CHECK_FALSE(s2 != s1);
    CHECK(s2 <= s1);
    CHECK_FALSE(s2 > s1);
    CHECK(s2 >= s1);
  }

  {
    int arr[] = {2, 1};
    span<int> s1 = arr;
    span<int> s2 = arr;

    CHECK(s1 == s2);
    CHECK_FALSE(s1 != s2);
    CHECK_FALSE(s1 < s2);
    CHECK(s1 <= s2);
    CHECK_FALSE(s1 > s2);
    CHECK(s1 >= s2);
    CHECK(s2 == s1);
    CHECK_FALSE(s2 != s1);
    CHECK_FALSE(s2 < s1);
    CHECK(s2 <= s1);
    CHECK_FALSE(s2 > s1);
    CHECK(s2 >= s1);
  }

  {
    int arr[] = {2, 1};  // bigger

    span<int> s1;
    span<int> s2 = arr;

    CHECK(s1 != s2);
    CHECK(s2 != s1);
    CHECK_FALSE(s1 == s2);
    CHECK_FALSE(s2 == s1);
    CHECK(s1 < s2);
    CHECK_FALSE(s2 < s1);
    CHECK(s1 <= s2);
    CHECK_FALSE(s2 <= s1);
    CHECK(s2 > s1);
    CHECK_FALSE(s1 > s2);
    CHECK(s2 >= s1);
    CHECK_FALSE(s1 >= s2);
  }

  {
    int arr1[] = {1, 2};
    int arr2[] = {1, 2};
    span<int> s1 = arr1;
    span<int> s2 = arr2;

    CHECK(s1 == s2);
    CHECK_FALSE(s1 != s2);
    CHECK_FALSE(s1 < s2);
    CHECK(s1 <= s2);
    CHECK_FALSE(s1 > s2);
    CHECK(s1 >= s2);
    CHECK(s2 == s1);
    CHECK_FALSE(s2 != s1);
    CHECK_FALSE(s2 < s1);
    CHECK(s2 <= s1);
    CHECK_FALSE(s2 > s1);
    CHECK(s2 >= s1);
  }

  {
    int arr[] = {1, 2, 3};

    span<int> s1 = {&arr[0], 2};  // shorter
    span<int> s2 = arr;           // longer

    CHECK(s1 != s2);
    CHECK(s2 != s1);
    CHECK_FALSE(s1 == s2);
    CHECK_FALSE(s2 == s1);
    CHECK(s1 < s2);
    CHECK_FALSE(s2 < s1);
    CHECK(s1 <= s2);
    CHECK_FALSE(s2 <= s1);
    CHECK(s2 > s1);
    CHECK_FALSE(s1 > s2);
    CHECK(s2 >= s1);
    CHECK_FALSE(s1 >= s2);
  }

  {
    int arr1[] = {1, 2};  // smaller
    int arr2[] = {2, 1};  // bigger

    span<int> s1 = arr1;
    span<int> s2 = arr2;

    CHECK(s1 != s2);
    CHECK(s2 != s1);
    CHECK_FALSE(s1 == s2);
    CHECK_FALSE(s2 == s1);
    CHECK(s1 < s2);
    CHECK_FALSE(s2 < s1);
    CHECK(s1 <= s2);
    CHECK_FALSE(s2 <= s1);
    CHECK(s2 > s1);
    CHECK_FALSE(s1 > s2);
    CHECK(s2 >= s1);
    CHECK_FALSE(s1 >= s2);
  }
}
