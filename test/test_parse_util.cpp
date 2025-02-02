#include <datadog/error.h>
#include <datadog/parse_util.h>

#include <cassert>
#include <cstdint>
#include <limits>
#include <string>
#include <variant>

#include "test.h"

using namespace datadog::tracing;

TEST_CASE("parse_int") {
  struct TestCase {
    int line;
    std::string name;
    std::string argument;
    int base;
    std::variant<int, Error::Code> expected;
  };

  // clang-format off
  auto test_case = GENERATE(values<TestCase>({
      {__LINE__, "zero (dec)", "0", 10, 0},
      {__LINE__, "zeros (dec)", "000", 10, 0},
      {__LINE__, "zero (hex)", "0", 16, 0},
      {__LINE__, "zeros (hex)", "000", 16, 0},
      {__LINE__, "leading whitespace (dec 1)", " 42", 10, Error::INVALID_INTEGER},
      {__LINE__, "leading whitespace (dec 2)", "\t42", 10, Error::INVALID_INTEGER},
      {__LINE__, "leading whitespace (dec 3)", "\n42", 10, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (dec 1)", "42 ", 10, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (dec 2)", "42\t", 10, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (dec 3)", "42\n", 10, Error::INVALID_INTEGER},
      {__LINE__, "leading whitespace (hex 1)", " 42", 16, Error::INVALID_INTEGER},
      {__LINE__, "leading whitespace (hex 2)", "\t42", 16, Error::INVALID_INTEGER},
      {__LINE__, "leading whitespace (hex 3)", "\n42", 16, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (hex 1)", "42 ", 16, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (hex 2)", "42\t", 16, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (hex 3)", "42\n", 16, Error::INVALID_INTEGER},
      {__LINE__, "no hex prefix", "0xbeef", 16, Error::INVALID_INTEGER},
      {__LINE__, "dec rejects hex", "42beef", 10, Error::INVALID_INTEGER},
      {__LINE__, "hex accepts hex", "42beef", 16, 0x42beef},
      {__LINE__, "no trailing nonsense (dec)", "42xyz", 10, Error::INVALID_INTEGER},
      {__LINE__, "no trailing nonsense (hex)", "42xyz", 16, Error::INVALID_INTEGER},
      {__LINE__, "no leading nonsense (dec)", "xyz42", 10, Error::INVALID_INTEGER},
      {__LINE__, "no leading nonsense (hex)", "xyz42", 16, Error::INVALID_INTEGER},
      {__LINE__, "overflow", std::to_string(std::numeric_limits<int>::max()) + "0", 10, Error::OUT_OF_RANGE_INTEGER},
      {__LINE__, "negative (dec)", "-10", 10, -10},
      {__LINE__, "negative (hex)", "-a", 16, -10},
      {__LINE__, "lower case", "a", 16, 10},
      {__LINE__, "upper case", "A", 16, 10},
      {__LINE__, "underflow", std::to_string(std::numeric_limits<int>::min()) + "0", 10, Error::OUT_OF_RANGE_INTEGER},
  }));
  // clang-format on

  CAPTURE(test_case.line);
  CAPTURE(test_case.name);
  CAPTURE(test_case.argument);
  CAPTURE(test_case.base);

  const auto result = parse_int(test_case.argument, test_case.base);
  if (std::holds_alternative<int>(test_case.expected)) {
    const int& expected = std::get<int>(test_case.expected);
    REQUIRE(result);
    REQUIRE(*result == expected);
  } else {
    assert(std::holds_alternative<Error::Code>(test_case.expected));
    const Error::Code& expected = std::get<Error::Code>(test_case.expected);
    REQUIRE(!result);
    REQUIRE(result.error().code == expected);
  }
}

// This test case is similar to the one above, except that negative numbers are
// not supported, and the underflow and overflow values are different.
TEST_CASE("parse_uint64") {
  struct TestCase {
    int line;
    std::string name;
    std::string argument;
    int base;
    std::variant<std::uint64_t, Error::Code> expected;
  };

  // clang-format off
  auto test_case = GENERATE(values<TestCase>({
      {__LINE__, "zero (dec)", "0", 10, UINT64_C(0)},
      {__LINE__, "zeros (dec)", "000", 10, UINT64_C(0)},
      {__LINE__, "zero (hex)", "0", 16, UINT64_C(0)},
      {__LINE__, "zeros (hex)", "000", 16, UINT64_C(0)},
      {__LINE__, "leading whitespace (dec 1)", " 42", 10, Error::INVALID_INTEGER},
      {__LINE__, "leading whitespace (dec 2)", "\t42", 10, Error::INVALID_INTEGER},
      {__LINE__, "leading whitespace (dec 3)", "\n42", 10, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (dec 1)", "42 ", 10, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (dec 2)", "42\t", 10, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (dec 3)", "42\n", 10, Error::INVALID_INTEGER},
      {__LINE__, "leading whitespace (hex 1)", " 42", 16, Error::INVALID_INTEGER},
      {__LINE__, "leading whitespace (hex 2)", "\t42", 16, Error::INVALID_INTEGER},
      {__LINE__, "leading whitespace (hex 3)", "\n42", 16, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (hex 1)", "42 ", 16, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (hex 2)", "42\t", 16, Error::INVALID_INTEGER},
      {__LINE__, "trailing whitespace (hex 3)", "42\n", 16, Error::INVALID_INTEGER},
      {__LINE__, "no hex prefix", "0xbeef", 16, Error::INVALID_INTEGER},
      {__LINE__, "dec rejects hex", "42beef", 10, Error::INVALID_INTEGER},
      {__LINE__, "hex accepts hex", "42beef", 16, UINT64_C(0x42beef)},
      {__LINE__, "no trailing nonsense (dec)", "42xyz", 10, Error::INVALID_INTEGER},
      {__LINE__, "no trailing nonsense (hex)", "42xyz", 16, Error::INVALID_INTEGER},
      {__LINE__, "no leading nonsense (dec)", "xyz42", 10, Error::INVALID_INTEGER},
      {__LINE__, "no leading nonsense (hex)", "xyz42", 16, Error::INVALID_INTEGER},
      {__LINE__, "overflow", std::to_string(std::numeric_limits<std::uint64_t>::max()) + "0", 10, Error::OUT_OF_RANGE_INTEGER},
      {__LINE__, "negative (dec)", "-10", 10, Error::INVALID_INTEGER},
      {__LINE__, "negative (hex)", "-a", 16, Error::INVALID_INTEGER},
      {__LINE__, "lower case", "a", 16, UINT64_C(10)},
      {__LINE__, "upper case", "A", 16, UINT64_C(10)},
  }));
  // clang-format on

  CAPTURE(test_case.line);
  CAPTURE(test_case.name);
  CAPTURE(test_case.argument);
  CAPTURE(test_case.base);

  const auto result = parse_uint64(test_case.argument, test_case.base);
  if (std::holds_alternative<std::uint64_t>(test_case.expected)) {
    const std::uint64_t& expected = std::get<std::uint64_t>(test_case.expected);
    REQUIRE(result);
    REQUIRE(*result == expected);
  } else {
    assert(std::holds_alternative<Error::Code>(test_case.expected));
    const Error::Code& expected = std::get<Error::Code>(test_case.expected);
    REQUIRE(!result);
    REQUIRE(result.error().code == expected);
  }
}
