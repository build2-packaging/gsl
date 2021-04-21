// #include <cstdlib>  // for std::exit
// #include <gsl/span> // for span

// int operator_subscript_no_throw() noexcept
// {
//     int arr[10];
//     const gsl::span<int> sp{arr};
//     return sp[11];
// }

// [[noreturn]] void test_terminate() { std::exit(0); }

// void setup_termination_handler() noexcept
// {
// #if defined(GSL_MSVC_USE_STL_NOEXCEPTION_WORKAROUND)

//     auto& handler = gsl::details::get_terminate_handler();
//     handler = &test_terminate;

// #else

//     std::set_terminate(test_terminate);

// #endif
// }

// int main() noexcept
// {
//     setup_termination_handler();
//     operator_subscript_no_throw();
//     return -1;
// }
