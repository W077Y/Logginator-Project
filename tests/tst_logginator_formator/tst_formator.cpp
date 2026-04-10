#include <ut_catch.hpp>
//
#include <logginator-format.hpp>

TEST_CASE("append_base64")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append_base64(buffer, buffer + sizeof(buffer), std::span<std::byte const>{ reinterpret_cast<std::byte const*>("foobar"), 6 });
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "Zm9vYmFy");
}

TEST_CASE("append_base64_empty")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append_base64(buffer, buffer + sizeof(buffer), std::span<std::byte const>{ reinterpret_cast<std::byte const*>(""), 0 });
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "");
}

TEST_CASE("append_base64_buffer_too_small")
{
  using namespace logginator::format;

  char buffer[5];
  auto res = append_base64(buffer, buffer + sizeof(buffer), std::span<std::byte const>{ reinterpret_cast<std::byte const*>("foobar"), 6 });
  REQUIRE(res.ec == std::errc::no_buffer_space);
}

TEST_CASE("append_string")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append_string(buffer, buffer + sizeof(buffer), "foobar");
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "foobar");
}

TEST_CASE("append_string_empty")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append_string(buffer, buffer + sizeof(buffer), "");
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "");
}

TEST_CASE("append_string_buffer_too_small")
{
  using namespace logginator::format;

  char buffer[5];
  auto res = append_string(buffer, buffer + sizeof(buffer), "foobar");
  REQUIRE(res.ec == std::errc::no_buffer_space);
}

TEST_CASE("append_n_chars")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append_n_chars(buffer, buffer + sizeof(buffer), 'x', 6);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "xxxxxx");
}

TEST_CASE("append_n_chars_empty")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append_n_chars(buffer, buffer + sizeof(buffer), 'x', 0);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "");
}

TEST_CASE("append_n_chars_buffer_too_small")
{
  using namespace logginator::format;

  char buffer[5];
  auto res = append_n_chars(buffer, buffer + sizeof(buffer), 'x', 6);
  REQUIRE(res.ec == std::errc::no_buffer_space);
}

TEST_CASE("append")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append(buffer, buffer + sizeof(buffer), -42, IntegerFormat::ascii);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "-42");
}

TEST_CASE("append_hex integer")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append(buffer, buffer + sizeof(buffer), -42, IntegerFormat::hex);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "-2a");
}

TEST_CASE("append_b64 integer")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append(buffer, buffer + sizeof(buffer), -42, IntegerFormat::b64);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "1v///w==");
}

TEST_CASE("append_ascii")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append(buffer, buffer + sizeof(buffer), 3.14, FloatFormat::ascii);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "3.14");
}

TEST_CASE("append_ascii_fixed")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append(buffer, buffer + sizeof(buffer), 3.14, FloatFormat::ascii_fixed);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "3.14");
}

TEST_CASE("append_ascii_scientific")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append(buffer, buffer + sizeof(buffer), 3.14, FloatFormat::ascii_scientific);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "3.14e+00");
}

TEST_CASE("append_hex float")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append(buffer, buffer + sizeof(buffer), 3.14, FloatFormat::hex);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "1.91eb851eb851fp+1");
}

TEST_CASE("append_b64 float")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append(buffer, buffer + sizeof(buffer), 3.14, FloatFormat::b64);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "H4XrUbgeCUA=");
}

TEST_CASE("append_ascii string")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append(buffer, buffer + sizeof(buffer), "foobar", StringFormat::ascii);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "foobar");
}

TEST_CASE("append_b64 string")
{
  using namespace logginator::format;

  char buffer[128];
  auto res = append(buffer, buffer + sizeof(buffer), "foobar", StringFormat::b64);
  REQUIRE(res.ec == std::errc());
  REQUIRE(std::string_view(buffer, res.ptr - buffer) == "Zm9vYmFy");
}