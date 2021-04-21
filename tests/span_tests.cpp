#include <doctest/doctest.h>
//
#include <gsl/gsl_byte>  // for byte
#include <gsl/gsl_util>  // for narrow_cast, at
#include <gsl/span>      // for span, span_iterator, operator==, operator!=
//
#include <array>        // for array
#include <cstddef>      // for ptrdiff_t
#include <iostream>     // for ptrdiff_t
#include <iterator>     // for reverse_iterator, operator-, operator==
#include <memory>       // for unique_ptr, shared_ptr, make_unique, allo...
#include <regex>        // for match_results, sub_match, match_results<>...
#include <string>       // for string
#include <type_traits>  // for integral_constant<>::value, is_default_co...
#include <utility>
#include <vector>  // for vector

// the string_view include and macro are used in the deduction guide
// verification
#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))
#ifdef __has_include
#if __has_include(<string_view>)
#include <string_view>
#define HAS_STRING_VIEW
#endif  // __has_include(<string_view>)
#endif  // __has_include
#endif  // (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >=
        // 201611L))

using namespace std;
using namespace gsl;

namespace {
static constexpr char deathstring[] = "Expected Death";

struct BaseClass {};
struct DerivedClass : BaseClass {};
struct AddressOverloaded {
#if (__cplusplus > 201402L)
  [[maybe_unused]]
#endif
  AddressOverloaded
  operator&() const {
    return {};
  }
};
}  // namespace

TEST_CASE("span_test, constructors") {
  span<int> s;
  CHECK(s.size() == 0);
  CHECK(s.data() == nullptr);

  span<const int> cs;
  CHECK(cs.size() == 0);
  CHECK(cs.data() == nullptr);
}

TEST_CASE("span_test, constructors_with_extent") {
  span<int, 0> s;
  CHECK(s.size() == 0);
  CHECK(s.data() == nullptr);

  span<const int, 0> cs;
  CHECK(cs.size() == 0);
  CHECK(cs.data() == nullptr);
}

TEST_CASE("span_test, constructors_with_bracket_init") {
  span<int> s{};
  CHECK(s.size() == 0);
  CHECK(s.data() == nullptr);

  span<const int> cs{};
  CHECK(cs.size() == 0);
  CHECK(cs.data() == nullptr);
}

TEST_CASE("span_test, size_optimization") {
  span<int> s;
  CHECK(sizeof(s) == sizeof(int*) + sizeof(ptrdiff_t));

  span<int, 0> se;
  CHECK(sizeof(se) == sizeof(int*));
}

TEST_CASE("span_test, from_nullptr_size_constructor") {
  std::set_terminate([] {
    std::cerr << "Expected Death. from_nullptr_size_constructor";
    std::abort();
  });
  {
    span<int> s{nullptr, narrow_cast<span<int>::size_type>(0)};
    CHECK(s.size() == 0);
    CHECK(s.data() == nullptr);

    span<int> cs{nullptr, narrow_cast<span<int>::size_type>(0)};
    CHECK(cs.size() == 0);
    CHECK(cs.data() == nullptr);
  }
  {
    auto workaround_macro = []() {
      const span<int, 1> s{nullptr, narrow_cast<span<int>::size_type>(0)};
    };
    // EXPECT_DEATH(workaround_macro(), deathstring);
  }
  {
    auto workaround_macro = []() { const span<int> s{nullptr, 1}; };
    // EXPECT_DEATH(workaround_macro(), deathstring);

    auto const_workaround_macro = []() { const span<const int> s{nullptr, 1}; };
    // EXPECT_DEATH(const_workaround_macro(), deathstring);
  }
  {
    auto workaround_macro = []() { const span<int, 0> s{nullptr, 1}; };
    // EXPECT_DEATH(workaround_macro(), deathstring);

    auto const_workaround_macro = []() {
      const span<const int, 0> s{nullptr, 1};
    };
    // EXPECT_DEATH(const_workaround_macro(), deathstring);
  }
  {
    span<int*> s{nullptr, narrow_cast<span<int>::size_type>(0)};
    CHECK(s.size() == 0);
    CHECK(s.data() == nullptr);

    span<const int*> cs{nullptr, narrow_cast<span<int>::size_type>(0)};
    CHECK(cs.size() == 0);
    CHECK(cs.data() == nullptr);
  }
}

TEST_CASE("span_test, from_pointer_length_constructor") {
  std::set_terminate([] {
    std::cerr << "Expected Death. from_pointer_length_constructor";
    std::abort();
  });
  int arr[4] = {1, 2, 3, 4};

  {
    for (int i = 0; i < 4; ++i) {
      {
        span<int> s = {&arr[0], narrow_cast<std::size_t>(i)};
        CHECK(s.size() == narrow_cast<std::size_t>(i));
        CHECK(s.data() == &arr[0]);
        CHECK(s.empty() == (i == 0));
        for (int j = 0; j < i; ++j)
          CHECK(arr[j] == s[narrow_cast<std::size_t>(j)]);
      }
      {
        span<int> s = {&arr[i], 4 - narrow_cast<std::size_t>(i)};
        CHECK(s.size() == 4 - narrow_cast<std::size_t>(i));
        CHECK(s.data() == &arr[i]);
        CHECK(s.empty() == ((4 - i) == 0));

        for (int j = 0; j < 4 - i; ++j)
          CHECK(arr[j + i] == s[narrow_cast<std::size_t>(j)]);
      }
    }
  }

  {
    span<int, 2> s{&arr[0], 2};
    CHECK(s.size() == 2);
    CHECK(s.data() == &arr[0]);
    CHECK(s[0] == 1);
    CHECK(s[1] == 2);
  }

  {
    int* p = nullptr;
    span<int> s{p, narrow_cast<span<int>::size_type>(0)};
    CHECK(s.size() == 0);
    CHECK(s.data() == nullptr);
  }

  {
    int* p = nullptr;
    auto workaround_macro = [=]() { const span<int> s{p, 2}; };
    // EXPECT_DEATH(workaround_macro(), deathstring);
  }
}

TEST_CASE("span_test, from_pointer_pointer_construction") {
  int arr[4] = {1, 2, 3, 4};

  {
    span<int> s{&arr[0], &arr[2]};
    CHECK(s.size() == 2);
    CHECK(s.data() == &arr[0]);
    CHECK(s[0] == 1);
    CHECK(s[1] == 2);
  }
  {
    span<int, 2> s{&arr[0], &arr[2]};
    CHECK(s.size() == 2);
    CHECK(s.data() == &arr[0]);
    CHECK(s[0] == 1);
    CHECK(s[1] == 2);
  }

  {
    span<int> s{&arr[0], &arr[0]};
    CHECK(s.size() == 0);
    CHECK(s.data() == &arr[0]);
  }

  {
    span<int, 0> s{&arr[0], &arr[0]};
    CHECK(s.size() == 0);
    CHECK(s.data() == &arr[0]);
  }

  // this will fail the std::distance() precondition, which asserts on MSVC
  // debug builds
  //{
  //    auto workaround_macro = [&]() { span<int> s{&arr[1], &arr[0]}; };
  // EXPECT_DEATH(workaround_macro(), deathstring);
  //}

  // this will fail the std::distance() precondition, which asserts on MSVC
  // debug builds
  //{
  //    int* p = nullptr;
  //    auto workaround_macro = [&]() { span<int> s{&arr[0], p}; };
  // EXPECT_DEATH(workaround_macro(), deathstring);
  //}

  {
    int* p = nullptr;
    span<int> s{p, p};
    CHECK(s.size() == 0);
    CHECK(s.data() == nullptr);
  }

  {
    int* p = nullptr;
    span<int, 0> s{p, p};
    CHECK(s.size() == 0);
    CHECK(s.data() == nullptr);
  }

  // this will fail the std::distance() precondition, which asserts on MSVC
  // debug builds
  //{
  //    int* p = nullptr;
  //    auto workaround_macro = [&]() { span<int> s{&arr[0], p}; };
  // EXPECT_DEATH(workaround_macro(), deathstring);
  //}
}

TEST_CASE("span_test, from_array_constructor") {
  int arr[5] = {1, 2, 3, 4, 5};

  {
    const span<int> s{arr};
    CHECK(s.size() == 5);
    CHECK(s.data() == &arr[0]);
  }

  {
    const span<int, 5> s{arr};
    CHECK(s.size() == 5);
    CHECK(s.data() == &arr[0]);
  }

  int arr2d[2][3] = {1, 2, 3, 4, 5, 6};

#ifdef CONFIRM_COMPILATION_ERRORS
  { span<int, 6> s{arr}; }

  {
    span<int, 0> s{arr};
    CHECK(s.size() == 0);
    CHECK(s.data() == &arr[0]);
  }

  {
    span<int> s{arr2d};
    CHECK(s.size() == 6);
    CHECK(s.data() == &arr2d[0][0]);
    CHECK(s[0] == 1);
    CHECK(s[5] == 6);
  }

  {
    span<int, 0> s{arr2d};
    CHECK(s.size() == 0);
    CHECK(s.data() == &arr2d[0][0]);
  }

  { span<int, 6> s{arr2d}; }
#endif
  {
    const span<int[3]> s{std::addressof(arr2d[0]), 1};
    CHECK(s.size() == 1);
    CHECK(s.data() == std::addressof(arr2d[0]));
  }

  int arr3d[2][3][2] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

#ifdef CONFIRM_COMPILATION_ERRORS
  {
    span<int> s{arr3d};
    CHECK(s.size() == 12);
    CHECK(s.data() == &arr3d[0][0][0]);
    CHECK(s[0] == 1);
    CHECK(s[11] == 12);
  }

  {
    span<int, 0> s{arr3d};
    CHECK(s.size() == 0);
    CHECK(s.data() == &arr3d[0][0][0]);
  }

  { span<int, 11> s{arr3d}; }

  {
    span<int, 12> s{arr3d};
    CHECK(s.size() == 12);
    CHECK(s.data() == &arr3d[0][0][0]);
    CHECK(s[0] == 1);
    CHECK(s[5] == 6);
  }
#endif
  {
    const span<int[3][2]> s{std::addressof(arr3d[0]), 1};
    CHECK(s.size() == 1);
  }

  AddressOverloaded ao_arr[5] = {};

  {
    const span<AddressOverloaded, 5> s{ao_arr};
    CHECK(s.size() == 5);
    CHECK(s.data() == std::addressof(ao_arr[0]));
  }
}

TEST_CASE("span_test, from_dynamic_array_constructor") {
  double(*arr)[3][4] = new double[100][3][4];

  {
    span<double> s(&arr[0][0][0], 10);
    CHECK(s.size() == 10);
    CHECK(s.data() == &arr[0][0][0]);
  }

  delete[] arr;
}

TEST_CASE("span_test, from_std_array_constructor") {
  std::array<int, 4> arr = {1, 2, 3, 4};

  {
    span<int> s{arr};
    CHECK(s.size() == arr.size());
    CHECK(s.data() == arr.data());

    span<const int> cs{arr};
    CHECK(cs.size() == arr.size());
    CHECK(cs.data() == arr.data());
  }

  {
    span<int, 4> s{arr};
    CHECK(s.size() == arr.size());
    CHECK(s.data() == arr.data());

    span<const int, 4> cs{arr};
    CHECK(cs.size() == arr.size());
    CHECK(cs.data() == arr.data());
  }

  {
    std::array<int, 0> empty_arr{};
    span<int> s{empty_arr};
    CHECK(s.size() == 0);
    CHECK(s.empty());
  }

  std::array<AddressOverloaded, 4> ao_arr{};

  {
    span<AddressOverloaded, 4> fs{ao_arr};
    CHECK(fs.size() == ao_arr.size());
    CHECK(ao_arr.data() == fs.data());
  }

#ifdef CONFIRM_COMPILATION_ERRORS
  {
    span<int, 2> s{arr};
    CHECK(s.size() == 2);
    CHECK(s.data() == arr.data());

    span<const int, 2> cs{arr};
    CHECK(cs.size() == 2);
    CHECK(cs.data() == arr.data());
  }

  {
    span<int, 0> s{arr};
    CHECK(s.size() == 0);
    CHECK(s.data() == arr.data());

    span<const int, 0> cs{arr};
    CHECK(cs.size() == 0);
    CHECK(cs.data() == arr.data());
  }

  { span<int, 5> s{arr}; }

  {
    auto get_an_array = []() -> std::array<int, 4> { return {1, 2, 3, 4}; };
    auto take_a_span = [](span<int> s) { static_cast<void>(s); };
    // try to take a temporary std::array
    take_a_span(get_an_array());
  }
#endif

  {
    auto get_an_array = []() -> std::array<int, 4> { return {1, 2, 3, 4}; };
    auto take_a_span = [](span<const int> s) { static_cast<void>(s); };
    // try to take a temporary std::array
    take_a_span(get_an_array());
  }
}

TEST_CASE("span_test, from_const_std_array_constructor") {
  const std::array<int, 4> arr = {1, 2, 3, 4};

  {
    span<const int> s{arr};
    CHECK(s.size() == arr.size());
    CHECK(s.data() == arr.data());
  }

  {
    span<const int, 4> s{arr};
    CHECK(s.size() == arr.size());
    CHECK(s.data() == arr.data());
  }

  const std::array<AddressOverloaded, 4> ao_arr{};

  {
    span<const AddressOverloaded, 4> s{ao_arr};
    CHECK(s.size() == ao_arr.size());
    CHECK(s.data() == ao_arr.data());
  }

#ifdef CONFIRM_COMPILATION_ERRORS
  {
    span<const int, 2> s{arr};
    CHECK(s.size() == 2);
    CHECK(s.data() == arr.data());
  }

  {
    span<const int, 0> s{arr};
    CHECK(s.size() == 0);
    CHECK(s.data() == arr.data());
  }

  { span<const int, 5> s{arr}; }
#endif

  {
    auto get_an_array = []() -> const std::array<int, 4> {
      return {1, 2, 3, 4};
    };
    auto take_a_span = [](span<const int> s) { static_cast<void>(s); };
    // try to take a temporary std::array
    take_a_span(get_an_array());
  }
}

TEST_CASE("span_test, from_std_array_const_constructor") {
  std::array<const int, 4> arr = {1, 2, 3, 4};

  {
    span<const int> s{arr};
    CHECK(s.size() == arr.size());
    CHECK(s.data() == arr.data());
  }

  {
    span<const int, 4> s{arr};
    CHECK(s.size() == arr.size());
    CHECK(s.data() == arr.data());
  }

#ifdef CONFIRM_COMPILATION_ERRORS
  {
    span<const int, 2> s{arr};
    CHECK(s.size() == 2);
    CHECK(s.data() == arr.data());
  }

  {
    span<const int, 0> s{arr};
    CHECK(s.size() == 0);
    CHECK(s.data() == arr.data());
  }

  { span<const int, 5> s{arr}; }

  { span<int, 4> s{arr}; }
#endif
}

TEST_CASE("span_test, from_container_constructor") {
  std::vector<int> v = {1, 2, 3};
  const std::vector<int> cv = v;

  {
    span<int> s{v};
    CHECK(s.size() == v.size());
    CHECK(s.data() == v.data());

    span<const int> cs{v};
    CHECK(cs.size() == v.size());
    CHECK(cs.data() == v.data());
  }

  std::string str = "hello";
  const std::string cstr = "hello";

  {
#ifdef CONFIRM_COMPILATION_ERRORS
    span<char> s{str};
    CHECK(s.size() == str.size());
         CHECK(s.data() == str.data()));
#endif
         span<const char> cs{str};
         CHECK(cs.size() == str.size());
         CHECK(cs.data() == str.data());
  }

  {
#ifdef CONFIRM_COMPILATION_ERRORS
    span<char> s{cstr};
#endif
    span<const char> cs{cstr};
    CHECK(cs.size() == cstr.size());
    CHECK(cs.data() == cstr.data());
  }

  {
#ifdef CONFIRM_COMPILATION_ERRORS
    auto get_temp_vector = []() -> std::vector<int> { return {}; };
    auto use_span = [](span<int> s) { static_cast<void>(s); };
    use_span(get_temp_vector());
#endif
  }

  {
    auto get_temp_vector = []() -> std::vector<int> { return {}; };
    auto use_span = [](span<const int> s) { static_cast<void>(s); };
    use_span(get_temp_vector());
  }

  {
#ifdef CONFIRM_COMPILATION_ERRORS
    auto get_temp_string = []() -> std::string { return {}; };
    auto use_span = [](span<char> s) { static_cast<void>(s); };
    use_span(get_temp_string());
#endif
  }

  {
    auto get_temp_string = []() -> std::string { return {}; };
    auto use_span = [](span<const char> s) { static_cast<void>(s); };
    use_span(get_temp_string());
  }

  {
#ifdef CONFIRM_COMPILATION_ERRORS
    auto get_temp_vector = []() -> const std::vector<int> { return {}; };
    auto use_span = [](span<const char> s) { static_cast<void>(s); };
    use_span(get_temp_vector());
#endif
  }

  {
    auto get_temp_string = []() -> const std::string { return {}; };
    auto use_span = [](span<const char> s) { static_cast<void>(s); };
    use_span(get_temp_string());
  }

  {
#ifdef CONFIRM_COMPILATION_ERRORS
    std::map<int, int> m;
    span<int> s{m};
#endif
  }
}

TEST_CASE("span_test, from_convertible_span_constructor"){
    {span<DerivedClass> avd;
span<const DerivedClass> avcd = avd;
static_cast<void>(avcd);
}

{
#ifdef CONFIRM_COMPILATION_ERRORS
  span<DerivedClass> avd;
  span<BaseClass> avb = avd;
  static_cast<void>(avb);
#endif
}

#ifdef CONFIRM_COMPILATION_ERRORS
{
  span<int> s;
  span<unsigned int> s2 = s;
  static_cast<void>(s2);
}

{
  span<int> s;
  span<const unsigned int> s2 = s;
  static_cast<void>(s2);
}

{
  span<int> s;
  span<short> s2 = s;
  static_cast<void>(s2);
}
#endif
}

TEST_CASE("span_test, copy_move_and_assignment") {
  span<int> s1;
  CHECK(s1.empty());

  int arr[] = {3, 4, 5};

  span<const int> s2 = arr;
  CHECK(s2.size() == 3);
  CHECK(s2.data() == &arr[0]);

  s2 = s1;
  CHECK(s2.empty());

  auto get_temp_span = [&]() -> span<int> { return {&arr[1], 2}; };
  auto use_span = [&](span<const int> s) {
    CHECK(s.size() == 2);
    CHECK(s.data() == &arr[1]);
  };
  use_span(get_temp_span());

  s1 = get_temp_span();
  CHECK(s1.size() == 2);
  CHECK(s1.data() == &arr[1]);
}

TEST_CASE("span_test, first") {
  std::set_terminate([] {
    std::cerr << "Expected Death. first";
    std::abort();
  });
  int arr[5] = {1, 2, 3, 4, 5};

  {
    span<int, 5> av = arr;
    CHECK(av.first<2>().size() == 2);
    CHECK(av.first(2).size() == 2);
  }

  {
    span<int, 5> av = arr;
    CHECK(av.first<0>().size() == 0);
    CHECK(av.first(0).size() == 0);
  }

  {
    span<int, 5> av = arr;
    CHECK(av.first<5>().size() == 5);
    CHECK(av.first(5).size() == 5);
  }

  {
    span<int, 5> av = arr;
#ifdef CONFIRM_COMPILATION_ERRORS
    CHECK(av.first<6>().size() == 6);
    CHECK(av.first<-1>().size() == -1);
#endif
    // EXPECT_DEATH(av.first(6).size(), deathstring);
  }

  {
    span<int> av;
    CHECK(av.first<0>().size() == 0);
    CHECK(av.first(0).size() == 0);
  }
}

TEST_CASE("span_test, last") {
  std::set_terminate([] {
    std::cerr << "Expected Death. last";
    std::abort();
  });
  int arr[5] = {1, 2, 3, 4, 5};

  {
    span<int, 5> av = arr;
    CHECK(av.last<2>().size() == 2);
    CHECK(av.last(2).size() == 2);
  }

  {
    span<int, 5> av = arr;
    CHECK(av.last<0>().size() == 0);
    CHECK(av.last(0).size() == 0);
  }

  {
    span<int, 5> av = arr;
    CHECK(av.last<5>().size() == 5);
    CHECK(av.last(5).size() == 5);
  }

  {
    span<int, 5> av = arr;
#ifdef CONFIRM_COMPILATION_ERRORS
    CHECK(av.last<6>().size() == 6);
#endif
    // EXPECT_DEATH(av.last(6).size(), deathstring);
  }

  {
    span<int> av;
    CHECK(av.last<0>().size() == 0);
    CHECK(av.last(0).size() == 0);
  }
}

TEST_CASE("span_test, subspan") {
  std::set_terminate([] {
    std::cerr << "Expected Death. subspan";
    std::abort();
  });
  int arr[5] = {1, 2, 3, 4, 5};

  {
    span<int, 5> av = arr;
    CHECK((av.subspan<2, 2>().size()) == 2);
    CHECK(decltype(av.subspan<2, 2>())::extent == 2);
    CHECK(av.subspan(2, 2).size() == 2);
    CHECK(av.subspan(2, 3).size() == 3);
  }

  {
    span<int, 5> av = arr;
    CHECK((av.subspan<0, 0>().size()) == 0);
    CHECK(decltype(av.subspan<0, 0>())::extent == 0);
    CHECK(av.subspan(0, 0).size() == 0);
  }

  {
    span<int, 5> av = arr;
    CHECK((av.subspan<0, 5>().size()) == 5);
    CHECK(decltype(av.subspan<0, 5>())::extent == 5);
    CHECK(av.subspan(0, 5).size() == 5);

    // EXPECT_DEATH(av.subspan(0, 6).size(), deathstring);
    // EXPECT_DEATH(av.subspan(1, 5).size(), deathstring);
  }

  {
    span<int, 5> av = arr;
    CHECK((av.subspan<4, 0>().size()) == 0);
    CHECK(decltype(av.subspan<4, 0>())::extent == 0);
    CHECK(av.subspan(4, 0).size() == 0);
    CHECK(av.subspan(5, 0).size() == 0);
    // EXPECT_DEATH(av.subspan(6, 0).size(), deathstring);
  }

  {
    span<int, 5> av = arr;
    CHECK(av.subspan<1>().size() == 4);
    CHECK(decltype(av.subspan<1>())::extent == 4);
  }

  {
    span<int> av;
    CHECK((av.subspan<0, 0>().size()) == 0);
    CHECK(decltype(av.subspan<0, 0>())::extent == 0);
    CHECK(av.subspan(0, 0).size() == 0);
    // EXPECT_DEATH((av.subspan<1, 0>().size()), deathstring);
  }

  {
    span<int> av;
    CHECK(av.subspan(0).size() == 0);
    // EXPECT_DEATH(av.subspan(1).size(), deathstring);
  }

  {
    span<int> av = arr;
    CHECK(av.subspan(0).size() == 5);
    CHECK(av.subspan(1).size() == 4);
    CHECK(av.subspan(4).size() == 1);
    CHECK(av.subspan(5).size() == 0);
    // EXPECT_DEATH(av.subspan(6).size(), deathstring);
    const auto av2 = av.subspan(1);
    for (std::size_t i = 0; i < 4; ++i)
      CHECK(av2[i] == static_cast<int>(i) + 2);
  }

  {
    span<int, 5> av = arr;
    CHECK(av.subspan(0).size() == 5);
    CHECK(av.subspan(1).size() == 4);
    CHECK(av.subspan(4).size() == 1);
    CHECK(av.subspan(5).size() == 0);
    // EXPECT_DEATH(av.subspan(6).size(), deathstring);
    const auto av2 = av.subspan(1);
    for (std::size_t i = 0; i < 4; ++i)
      CHECK(av2[i] == static_cast<int>(i) + 2);
  }
}

TEST_CASE("span_test, iterator_default_init") {
  span<int>::iterator it1;
  span<int>::iterator it2;
  CHECK(it1 == it2);
}

TEST_CASE("span_test, iterator_comparisons") {
  int a[] = {1, 2, 3, 4};
  {
    span<int> s = a;
    span<int>::iterator it = s.begin();
    auto it2 = it + 1;

    CHECK(it == it);
    CHECK(it == s.begin());
    CHECK(s.begin() == it);

    CHECK(it != it2);
    CHECK(it2 != it);
    CHECK(it != s.end());
    CHECK(it2 != s.end());
    CHECK(s.end() != it);

    CHECK(it < it2);
    CHECK(it <= it2);
    CHECK(it2 <= s.end());
    CHECK(it < s.end());

    CHECK(it2 > it);
    CHECK(it2 >= it);
    CHECK(s.end() > it2);
    CHECK(s.end() >= it2);
  }
}

TEST_CASE("span_test, incomparable_iterators") {
  std::set_terminate([] {
    std::cerr << "Expected Death. incomparable_iterators";
    std::abort();
  });

  int a[] = {1, 2, 3, 4};
  int b[] = {1, 2, 3, 4};
  {
    span<int> s = a;
    span<int> s2 = b;
#if (__cplusplus > 201402L)
    // EXPECT_DEATH([[maybe_unused]] bool _ = (s.begin() == s2.begin()),
    // deathstring);
    // EXPECT_DEATH([[maybe_unused]] bool _ = (s.begin() <=
    // s2.begin()),
    // deathstring);
#else
    // EXPECT_DEATH(bool _ = (s.begin() == s2.begin()), deathstring);
    // EXPECT_DEATH(bool _ = (s.begin() <= s2.begin()), deathstring);
#endif
  }
}

TEST_CASE("span_test, begin_end") {
  std::set_terminate([] {
    std::cerr << "Expected Death. begin_end";
    std::abort();
  });
  {
    int a[] = {1, 2, 3, 4};
    span<int> s = a;

    span<int>::iterator it = s.begin();
    span<int>::iterator it2 = std::begin(s);
    CHECK(it == it2);

    it = s.end();
    it2 = std::end(s);
    CHECK(it == it2);
  }

  {
    int a[] = {1, 2, 3, 4};
    span<int> s = a;

    auto it = s.begin();
    auto first = it;
    CHECK(it == first);
    CHECK(*it == 1);

    auto beyond = s.end();
    CHECK(it != beyond);
    // EXPECT_DEATH(*beyond, deathstring);

    CHECK(beyond - first == 4);
    CHECK(first - first == 0);
    CHECK(beyond - beyond == 0);

    ++it;
    CHECK(it - first == 1);
    CHECK(*it == 2);
    *it = 22;
    CHECK(*it == 22);
    CHECK(beyond - it == 3);

    it = first;
    CHECK(it == first);
    while (it != s.end()) {
      *it = 5;
      ++it;
    }

    CHECK(it == beyond);
    CHECK(it - beyond == 0);

    for (const auto& n : s) {
      CHECK(n == 5);
    }
  }
}

TEST_CASE("span_test, rbegin_rend") {
  std::set_terminate([] {
    std::cerr << "Expected Death. rbegin_rend";
    std::abort();
  });
  {
    int a[] = {1, 2, 3, 4};
    span<int> s = a;

    auto it = s.rbegin();
    auto first = it;
    CHECK(it == first);
    CHECK(*it == 4);

    auto beyond = s.rend();
    CHECK(it != beyond);
#if (__cplusplus > 201402L)
    // EXPECT_DEATH([[maybe_unused]] auto _ = *beyond, deathstring);
#else
    // EXPECT_DEATH(auto _ = *beyond, deathstring);
#endif

    CHECK(beyond - first == 4);
    CHECK(first - first == 0);
    CHECK(beyond - beyond == 0);

    ++it;
    CHECK(it - s.rbegin() == 1);
    CHECK(*it == 3);
    *it = 22;
    CHECK(*it == 22);
    CHECK(beyond - it == 3);

    it = first;
    CHECK(it == first);
    while (it != s.rend()) {
      *it = 5;
      ++it;
    }

    CHECK(it == beyond);
    CHECK(it - beyond == 0);

    for (const auto& n : s) {
      CHECK(n == 5);
    }
  }
}

TEST_CASE("span_test, as_bytes") {
  std::set_terminate([] {
    std::cerr << "Expected Death. as_bytes";
    std::abort();
  });

  int a[] = {1, 2, 3, 4};
  {
    const span<const int> s = a;
    CHECK(s.size() == 4);
    const span<const byte> bs = as_bytes(s);
    CHECK(static_cast<const void*>(bs.data()) ==
          static_cast<const void*>(s.data()));
    CHECK(bs.size() == s.size_bytes());
  }

  {
    span<int> s;
    const auto bs = as_bytes(s);
    CHECK(bs.size() == s.size());
    CHECK(bs.size() == 0);
    CHECK(bs.size_bytes() == 0);
    CHECK(static_cast<const void*>(bs.data()) ==
          static_cast<const void*>(s.data()));
    CHECK(bs.data() == nullptr);
  }

  {
    span<int> s = a;
    const auto bs = as_bytes(s);
    CHECK(static_cast<const void*>(bs.data()) ==
          static_cast<const void*>(s.data()));
    CHECK(bs.size() == s.size_bytes());
  }

  int b[5] = {1, 2, 3, 4, 5};
  {
    span<int> sp(begin(b), static_cast<size_t>(-2));
    // EXPECT_DEATH((void)sp.size_bytes(), deathstring);
  }
}

TEST_CASE("span_test, as_writable_bytes") {
  int a[] = {1, 2, 3, 4};

  {
#ifdef CONFIRM_COMPILATION_ERRORS
    // you should not be able to get writeable bytes for const objects
    span<const int> s = a;
    CHECK(s.size() == 4);
    span<const byte> bs = as_writable_bytes(s);
    CHECK(static_cast<void*>(bs.data()) == static_cast<void*>(s.data()));
    CHECK(bs.size() == s.size_bytes());
#endif
  }

  {
    span<int> s;
    const auto bs = as_writable_bytes(s);
    CHECK(bs.size() == s.size());
    CHECK(bs.size() == 0);
    CHECK(bs.size_bytes() == 0);
    CHECK(static_cast<void*>(bs.data()) == static_cast<void*>(s.data()));
    CHECK(bs.data() == nullptr);
  }

  {
    span<int> s = a;
    const auto bs = as_writable_bytes(s);
    CHECK(static_cast<void*>(bs.data()) == static_cast<void*>(s.data()));
    CHECK(bs.size() == s.size_bytes());
  }
}

TEST_CASE("span_test, fixed_size_conversions") {
  std::set_terminate([] {
    std::cerr << "Expected Death. fixed_size_conversions";
    std::abort();
  });
  int arr[] = {1, 2, 3, 4};

  // converting to an span from an equal size array is ok
  span<int, 4> s4 = arr;
  CHECK(s4.size() == 4);

  // converting to dynamic_range is always ok
  {
    span<int> s = s4;
    CHECK(s.size() == s4.size());
    static_cast<void>(s);
  }

// initialization or assignment to static span that REDUCES size is NOT ok
#ifdef CONFIRM_COMPILATION_ERRORS
  { span<int, 2> s = arr; }
  {
    span<int, 2> s2 = s4;
    static_cast<void>(s2);
  }
#endif

  // even when done dynamically
  {
    // this now results in a compile-time error, rather than runtime.
    // There is no suitable conversion from dynamic span to fixed span.
#ifdef CONFIRM_COMPILATION_ERRORS
    span<int> s = arr;
    auto f = [&]() {
      const span<int, 2> s2 = s;
      static_cast<void>(s2);
    };
    // EXPECT_DEATH(f(), deathstring);
#endif
  }

  // but doing so explicitly is ok

  // you can convert statically
  {
    const span<int, 2> s2{&arr[0], 2};
    static_cast<void>(s2);
  }
  {
    const span<int, 1> s1 = s4.first<1>();
    static_cast<void>(s1);
  }

  /*
   // this is not a legal operation in std::span, so we are no longer supporting
   it
   // conversion from span<int, 4> to span<int, dynamic_extent> via call to
   `first`
   // then convert from span<int, dynamic_extent> to span<int, 1>
   // The dynamic to fixed extents are not supported in the standard
   // to make this work, span<int, 1> would need to be span<int>.
   {

       // NB: implicit conversion to span<int,1> from span<int>
       span<int, 1> s1 = s4.first(1);
       static_cast<void>(s1);
   }
   */

  // initialization or assignment to static span that requires size INCREASE is
  // not ok.
  int arr2[2] = {1, 2};

#ifdef CONFIRM_COMPILATION_ERRORS
  { span<int, 4> s3 = arr2; }
  {
    span<int, 2> s2 = arr2;
    span<int, 4> s4a = s2;
  }
#endif
  {
    auto f = [&]() {
      const span<int, 4> _s4{arr2, 2};
      static_cast<void>(_s4);
    };
    // EXPECT_DEATH(f(), deathstring);
  }

  // This no longer compiles. There is no suitable conversion from dynamic span
  // to a fixed size span.
  // this should fail - we are trying to assign a small dynamic span to a
  // fixed_size larger one span<int>
#ifdef CONFIRM_COMPILATION_ERRORS
  av = arr2;
  auto f = [&]() {
    const span<int, 4> _s4 = av;
    static_cast<void>(_s4);
  };
  // EXPECT_DEATH(f(), deathstring);
#endif
}

TEST_CASE("span_test, interop_with_std_regex") {
  char lat[] = {'1', '2', '3', '4', '5', '6', 'E', 'F', 'G'};
  span<char> s = lat;
  const auto f_it = s.begin() + 7;

  std::match_results<span<char>::iterator> match;

  std::regex_match(s.begin(), s.end(), match, std::regex(".*"));
  CHECK(match.ready());
  CHECK_FALSE(match.empty());
  CHECK(match[0].matched);
  CHECK(match[0].first == s.begin());
  CHECK(match[0].second == s.end());

  std::regex_search(s.begin(), s.end(), match, std::regex("F"));
  CHECK(match.ready());
  CHECK_FALSE(match.empty());
  CHECK(match[0].matched);
  CHECK(match[0].first == f_it);
  CHECK(match[0].second == (f_it + 1));
}

TEST_CASE("span_test, default_constructible") {
  CHECK((std::is_default_constructible<span<int>>::value));
  CHECK((std::is_default_constructible<span<int, 0>>::value));
  CHECK_FALSE((std::is_default_constructible<span<int, 42>>::value));
}

TEST_CASE("span_test, std_container_ctad") {
#if (defined(__cpp_deduction_guides) && (__cpp_deduction_guides >= 201611L))
  // this TEST_CASE "is just to" verify that these compile
  {
    std::vector<int> v{1, 2, 3, 4};
    gsl::span sp{v};
    static_assert(std::is_same<decltype(sp), gsl::span<int>>::value);
  }
  {
    std::string str{"foo"};
    gsl::span sp{str};
    static_assert(std::is_same<decltype(sp), gsl::span<char>>::value);
  }
#ifdef HAS_STRING_VIEW
  {
    std::string_view sv{"foo"};
    gsl::span sp{sv};
    static_assert(std::is_same<decltype(sp), gsl::span<const char>>::value);
  }
#endif
#endif
}

TEST_CASE("span_test, front_back") {
  int arr[5] = {1, 2, 3, 4, 5};
  span<int> s{arr};
  CHECK(s.front() == 1);
  CHECK(s.back() == 5);

  std::set_terminate([] {
    std::cerr << "Expected Death. front_back";
    std::abort();
  });
  span<int> s2;
  // EXPECT_DEATH(s2.front(), deathstring);
  // EXPECT_DEATH(s2.back(), deathstring);
}
