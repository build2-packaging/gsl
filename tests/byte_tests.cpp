///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#include <doctest/doctest.h>

#include <gsl/byte> // for to_byte, to_integer, byte, operator&, ope...

using namespace std;
using namespace gsl;

namespace
{
int modify_both(gsl::byte& b, int& i)
{
    i = 10;
    b = to_byte<5>();
    return i;
}

TEST_CASE("byte_tests, construction")
{
    {
        const byte b = static_cast<byte>(4);
        CHECK(static_cast<unsigned char>(b) == 4);
    }

    // clang-format off
    GSL_SUPPRESS(es.49)
    // clang-format on
    {
        const byte b = byte(12);
        CHECK(static_cast<unsigned char>(b) == 12);
    }

    {
        const byte b = to_byte<12>();
        CHECK(static_cast<unsigned char>(b) == 12);
    }
    {
        const unsigned char uc = 12;
        const byte b = to_byte(uc);
        CHECK(static_cast<unsigned char>(b) == 12);
    }

#if defined(__cplusplus) && (__cplusplus >= 201703L)
    {
        const byte b{14};
        CHECK(static_cast<unsigned char>(b) == 14);
    }
#endif

#ifdef CONFIRM_COMPILATION_ERRORS
    to_byte(char{});
    to_byte(3);
    to_byte(3u);
#endif
}

TEST_CASE("byte_tests, bitwise_operations")
{
    const byte b = to_byte<0xFF>();

    byte a = to_byte<0x00>();
    CHECK((b | a) == to_byte<0xFF>());
    CHECK(a == to_byte<0x00>());

    a |= b;
    CHECK(a == to_byte<0xFF>());

    a = to_byte<0x01>();
    CHECK((b & a) == to_byte<0x01>());

    a &= b;
    CHECK(a == to_byte<0x01>());

    CHECK((b ^ a) == to_byte<0xFE>());

    CHECK(a == to_byte<0x01>());
    a ^= b;
    CHECK(a == to_byte<0xFE>());

    a = to_byte<0x01>();
    CHECK(~a == to_byte<0xFE>());

    a = to_byte<0xFF>();
    CHECK((a << 4) == to_byte<0xF0>());
    CHECK((a >> 4) == to_byte<0x0F>());

    a <<= 4;
    CHECK(a == to_byte<0xF0>());
    a >>= 4;
    CHECK(a == to_byte<0x0F>());
}

TEST_CASE("byte_tests, to_integer")
{
    const byte b = to_byte<0x12>();

    CHECK(0x12 == gsl::to_integer<char>(b));
    CHECK(0x12 == gsl::to_integer<short>(b));
    CHECK(0x12 == gsl::to_integer<long>(b));
    CHECK(0x12 == gsl::to_integer<long long>(b));

    CHECK(0x12 == gsl::to_integer<unsigned char>(b));
    CHECK(0x12 == gsl::to_integer<unsigned short>(b));
    CHECK(0x12 == gsl::to_integer<unsigned long>(b));
    CHECK(0x12 == gsl::to_integer<unsigned long long>(b));

    //      CHECK(0x12 == gsl::to_integer<float>(b));   // expect compile-time error
    //      CHECK(0x12 == gsl::to_integer<double>(b));  // expect compile-time error
}

TEST_CASE("byte_tests, aliasing")
{
    int i{0};
    const int res = modify_both(reinterpret_cast<byte&>(i), i);
    CHECK(res == i);
}

} // namespace

#ifdef CONFIRM_COMPILATION_ERRORS
copy(src_span_static, dst_span_static);
#endif
