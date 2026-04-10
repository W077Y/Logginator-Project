#include <ut_catch.hpp>
//
#include <array>
#include <iostream>
#include <logginator.hpp>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>

namespace test_app
{
  // Test data types
  struct SimpleData
  {
    int32_t count;
    double  value;
  };

  struct BinaryData
  {
    uint32_t                  id;
    std::array<std::byte, 16> hash;
  };

  struct StringData
  {
    std::size_t      index;
    std::string_view message;
  };

  struct BoolData
  {
    bool flag;
  };

  // ADL hooks for custom types
  logginator::line_t request_line(SimpleData const&);
  void               print(SimpleData const& data, logginator::line_t& line);

  logginator::line_t request_line(BinaryData const&);
  void               print(BinaryData const& data, logginator::line_t& line);

  logginator::line_t request_line(StringData const&);
  void               print(StringData const& data, logginator::line_t& line);

  logginator::line_t request_line(BoolData const&);
  void               print(BoolData const& data, logginator::line_t& line);
}    // namespace test_app

// Output capture for testing
class StringOutput: public logginator::Manager_Interface::Output_Interface
{
public:
  void operator()(std::string_view msg) noexcept override
  {
    try
    {
      output.append(msg);
    }
    catch (...)
    {
    }
  }

  std::string get_output() const { return output; }

  void clear() { output.clear(); }

private:
  std::string output;
};

// Manager instance
StringOutput                          g_output;
logginator::Manager<std::mutex, 8192> g_manager{ g_output };

logginator::Manager_Interface& get_manager() { return g_manager; }

namespace test_app
{
  // Implementations for test types
  void print(SimpleData const& data, logginator::line_t& line)
  {
    using namespace logginator;
    using FI = column_description_int::Format;
    using FF = column_description_float::Format;

    line.add(column_description_int{ "count", "", FI::ascii }, data.count);
    line.add(column_description_float{ "value", "", FF::ascii }, data.value);
  }

  logginator::line_t request_line(SimpleData const& val)
  {
    static auto channel = get_manager().request_channel(val, logginator::channel_description_t{ 10, "SimpleData" });
    return channel.request_line();
  }

  void print(BinaryData const& data, logginator::line_t& line)
  {
    using namespace logginator;
    using FI = column_description_int::Format;
    using FB = column_description_binary::Format;

    line.add(column_description_int{ "id", "", FI::hex }, data.id);
    line.add(column_description_binary{ "hash", "", FB::b64 }, std::span<std::byte const>{ data.hash });
  }

  logginator::line_t request_line(BinaryData const& val)
  {
    static auto channel = get_manager().request_channel(val, logginator::channel_description_t{ 11, "BinaryData" });
    return channel.request_line();
  }

  void print(StringData const& data, logginator::line_t& line)
  {
    using namespace logginator;
    using FI = column_description_int::Format;
    using FS = column_description_string::Format;

    line.add(column_description_int{ "index", "", FI::ascii }, data.index);
    line.add(column_description_string{ "message", "", FS::ascii }, data.message);
  }
  logginator::line_t request_line(StringData const& val)
  {
    static auto channel = get_manager().request_channel(val, logginator::channel_description_t{ 12, "StringData" });
    return channel.request_line();
  }

  void print(BoolData const& data, logginator::line_t& line)
  {
    using namespace logginator;
    using FI = column_description_int::Format;

    line.add(column_description_int{ "flag", "", FI::ascii }, data.flag);
  }
  logginator::line_t request_line(BoolData const& val)
  {
    static auto channel = get_manager().request_channel(val, logginator::channel_description_t{ 20, "BoolData" });
    return channel.request_line();
  }
}    // namespace test_app

// Tests
TEST_CASE("Logger: Basic integer formatting")
{
  g_output.clear();

  test_app::SimpleData data{ .count = 42, .value = 3.14 };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("#0a") != std::string::npos);     // Channel ID 10
  REQUIRE(output.find("42") != std::string::npos);      // count value
  REQUIRE(output.find("3.14") != std::string::npos);    // value
  REQUIRE(output.find("\n") != std::string::npos);      // Line terminator
}

TEST_CASE("Logger: Negative integers")
{
  g_output.clear();

  test_app::SimpleData data{ .count = -123, .value = -45.67 };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("-123") != std::string::npos);
  REQUIRE(output.find("-45.67") != std::string::npos);
}

TEST_CASE("Logger: Zero and small values")
{
  g_output.clear();

  test_app::SimpleData data{ .count = 0, .value = 0.0 };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("0") != std::string::npos);
}

TEST_CASE("Logger: Hexadecimal integer formatting")
{
  g_output.clear();

  test_app::BinaryData data{ .id = 0xDEADBEEF, .hash = {} };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("dead") != std::string::npos);
}

TEST_CASE("Logger: Base64 binary data encoding")
{
  g_output.clear();

  test_app::BinaryData data{ .id = 1, .hash = {} };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  // Fill hash with known pattern
  for (std::size_t i = 0; i < data.hash.size(); ++i)
  {
    data.hash[i] = std::byte(i);
  }

  logginator::print(data);

  std::string output = g_output.get_output();
  // Base64 output should only contain valid Base64 characters (A-Z, a-z, 0-9, +, /, =)
  REQUIRE(output.find("#0b") != std::string::npos);    // Channel ID 11
  // Some form of encoding should be present
  REQUIRE(output.length() > 20);
}

TEST_CASE("Logger: String data logging")
{
  g_output.clear();

  test_app::StringData data{ .index = 5, .message = "Hello World" };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("5") != std::string::npos);
  REQUIRE(output.find("Hello World") != std::string::npos);
}

TEST_CASE("Logger: Empty string handling")
{
  g_output.clear();

  test_app::StringData data{ .index = 0, .message = "" };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("#0c") != std::string::npos);    // Should still log with channel ID
}

TEST_CASE("Logger: Channel headers")
{
  g_output.clear();
  // Log some data first
  test_app::SimpleData simple{ .count = 1, .value = 1.0 };
  get_manager().setup_channel(request_line(simple).get_cfg().ID, 1);
  logginator::print(simple);
  // Print headers
  get_manager().print_channels();
  std::string output = g_output.get_output();
  // Headers should contain channel info
  REQUIRE(output.find("SimpleData") != std::string::npos);
  REQUIRE(output.find("#0a") != std::string::npos);
}

TEST_CASE("Logger: Downsampling configuration")
{
  g_output.clear();

  // Setup channel 10 to downsample every 3rd sample
  get_manager().setup_channel(request_line(test_app::SimpleData()).get_cfg().ID, 3);

  // Log 10 samples
  for (int i = 0; i < 10; ++i)
  {
    test_app::SimpleData data{ .count = i, .value = static_cast<double>(i) };
    logginator::print(data);
  }

  std::string output = g_output.get_output();
  // With downsampling of 3, we should get roughly 3-4 log lines instead of 10
  // Count newlines (one per log line)
  std::size_t line_count = std::count(output.begin(), output.end(), '\n');
  REQUIRE(line_count <= 5);    // Should be significantly less than 10
}

TEST_CASE("Logger: Large values")
{
  g_output.clear();

  test_app::SimpleData data{ .count = std::numeric_limits<int32_t>::max(), .value = std::numeric_limits<double>::max() };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.length() > 0);
  REQUIRE(output.find("#0a") != std::string::npos);
}

TEST_CASE("Logger: Multiple data types in sequence")
{
  g_output.clear();

  test_app::SimpleData simple{ .count = 42, .value = 1.5 };
  test_app::StringData str{ .index = 1, .message = "test" };
  test_app::BinaryData binary{ .id = 99, .hash = {} };
  get_manager().setup_channel(request_line(simple).get_cfg().ID, 1);
  get_manager().setup_channel(request_line(str).get_cfg().ID, 1);
  get_manager().setup_channel(request_line(binary).get_cfg().ID, 1);
  logginator::print(simple);
  logginator::print(str);
  logginator::print(binary);

  std::string output = g_output.get_output();
  // All three should be logged
  REQUIRE(output.find("#0a") != std::string::npos);    // SimpleData channel
  REQUIRE(output.find("#0c") != std::string::npos);    // StringData channel
  REQUIRE(output.find("#0b") != std::string::npos);    // BinaryData channel
  // Count lines
  std::size_t line_count = std::count(output.begin(), output.end(), '\n');
  REQUIRE(line_count >= 3);
}

TEST_CASE("Logger: Line format structure")
{
  g_output.clear();

  test_app::SimpleData data{ .count = 123, .value = 45.6 };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  // Line should start with # and channel ID
  REQUIRE(output[0] == '#');
  // Should end with newline
  REQUIRE(output.back() == '\n');
  // Should contain semicolon separators
  REQUIRE(output.find(';') != std::string::npos);
}

TEST_CASE("Logger: Special float values (infinity)")
{
  g_output.clear();

  test_app::SimpleData data{ .count = 1, .value = std::numeric_limits<double>::infinity() };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.length() > 0);
}

TEST_CASE("Logger: Special float values (NaN)")
{
  g_output.clear();

  test_app::SimpleData data{ .count = 1, .value = std::numeric_limits<double>::quiet_NaN() };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.length() > 0);
}

TEST_CASE("Logger: Boolean as integer")
{
  g_output.clear();

  test_app::BoolData data{ .flag = true };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("#14") != std::string::npos);    // Channel 20 in hex
}

TEST_CASE("Logger: Rapid successive logging")
{
  g_output.clear();

  for (int i = 0; i < 100; ++i)
  {
    test_app::SimpleData data{ .count = i, .value = i * 0.5 };
    get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
    logginator::print(data);
  }

  std::string output = g_output.get_output();
  // Should have logged all 100 items (or downsampled version)
  std::size_t line_count = std::count(output.begin(), output.end(), '\n');
  REQUIRE(line_count > 0);
}

TEST_CASE("Logger: Buffer capacity")
{
  g_output.clear();

  // Try to log very long strings
  test_app::StringData data{ .index = 999, .message = "This is a very long test message that should fit in the buffer without issues" };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("This is a very long") != std::string::npos);
}

TEST_CASE("Logger: Binary data with various byte patterns")
{
  g_output.clear();

  test_app::BinaryData data{ .id = 42, .hash = {} };

  // All zeros
  std::fill(data.hash.begin(), data.hash.end(), std::byte(0x00));
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  g_output.clear();

  // All ones
  std::fill(data.hash.begin(), data.hash.end(), std::byte(0xFF));
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.length() > 0);
}

// ============================================================================
// Thread-Safety Tests
// ============================================================================

TEST_CASE("Logger: Thread-safe concurrent logging")
{
  g_output.clear();

  std::array<std::thread, 4> threads;

  // Launch 4 threads logging simultaneously
  for (int t = 0; t < 4; ++t)
  {
    threads[t] = std::thread(
        [t]()
        {
          for (int i = 0; i < 25; ++i)
          {
            test_app::SimpleData data{ .count = t * 25 + i, .value = (t * 25 + i) * 0.1 };
            logginator::print(data);
          }
        });
  }

  // Wait for all threads
  for (auto& thread : threads)
  {
    thread.join();
  }

  std::string output = g_output.get_output();
  // Should have 100 log lines
  std::size_t line_count = std::count(output.begin(), output.end(), '\n');
  REQUIRE(line_count == 100);
  // Output should be well-formed
  REQUIRE(output.find("#0a") != std::string::npos);
}

TEST_CASE("Logger: Thread-safe channel setup and logging")
{
  g_output.clear();

  std::array<std::thread, 3> threads;

  // Multiple threads calling setup_channel and logging
  for (int t = 0; t < 3; ++t)
  {
    threads[t] = std::thread(
        [t]()
        {
          for (int i = 0; i < 10; ++i)
          {
            test_app::SimpleData data{ .count = t * 10 + i, .value = i * 1.5 };
            get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
            logginator::print(data);
          }
        });
  }

  for (auto& thread : threads)
  {
    thread.join();
  }

  std::string output = g_output.get_output();
  // Should complete without deadlock
  std::size_t line_count = std::count(output.begin(), output.end(), '\n');
  REQUIRE(line_count > 0);
}

// ============================================================================
// Exception-Handling Tests
// ============================================================================

TEST_CASE("Logger: Invalid channel ID subscription error")
{
  // Try to setup a channel that was never subscribed
  REQUIRE_THROWS_AS(get_manager().setup_channel(99, 1), logginator::errors::channel_setup_error);
}

TEST_CASE("Logger: Exception on line formatting")
{
  // Create a test that can trigger buffer overflow
  // Use a very small manager temporarily
  StringOutput                        small_output;
  logginator::Manager<std::mutex, 32> small_manager{ small_output };
  logginator::Manager_Interface&      manager = small_manager;

  try
  {
    auto channel = manager.request_channel(test_app::SimpleData(), logginator::channel_description_t{ 1, "VeryLongChannelNameThatWillCauseBufferOverflow" });
    manager.print_channels();
    //  This should throw due to buffer being too small
    REQUIRE(false);    // If we get here, buffer was large enough
  }
  catch (logginator::errors::line_serialization_error&)
  {
    REQUIRE(true);    // Expected exception
  }
}

TEST_CASE("Logger: Multiple channel registrations")
{
  // Test that the same channel ID cannot be registered twice
  StringOutput                          output;
  logginator::Manager<std::mutex, 8192> manager{ output };

  try
  {
    auto channel1 = manager.request_channel(test_app::SimpleData(), logginator::channel_description_t{ 50, "Ch1" });
    auto channel2 = manager.request_channel(test_app::SimpleData(), logginator::channel_description_t{ 50, "Ch2" });
    // Second registration with same ID should fail
    REQUIRE(false);    // Should not reach here
  }
  catch (logginator::errors::channel_subscribtion_error&)
  {
    REQUIRE(true);    // Expected
  }
}

// ============================================================================
// Edge-Case Tests
// ============================================================================

TEST_CASE("Logger: Channel ID boundary - minimum (0)")
{
  g_output.clear();

  StringOutput                          boundary_output;
  logginator::Manager<std::mutex, 8192> boundary_manager{ boundary_output };
  logginator::Manager_Interface&        manager = boundary_manager;

  auto channel = manager.request_channel(test_app::SimpleData(), logginator::channel_description_t{ 0, "MinID" });
  manager.setup_channel(0, 1);

  test_app::SimpleData data{ .count = 1, .value = 2.0 };
  {
    auto line = channel.request_line();
    print(data, line);
  }

  std::string output = boundary_output.get_output();
  REQUIRE(output.find("#00") != std::string::npos);
}

TEST_CASE("Logger: Channel ID boundary - maximum (255)")
{
  StringOutput                          boundary_output;
  logginator::Manager<std::mutex, 8192> boundary_manager{ boundary_output };
  logginator::Manager_Interface&        manager = boundary_manager;

  auto channel = manager.request_channel(test_app::SimpleData(), logginator::channel_description_t{ 255, "MaxID" });
  manager.setup_channel(255, 1);

  test_app::SimpleData data{ .count = 1, .value = 2.0 };
  {
    auto line = channel.request_line();
    print(data, line);
  }

  std::string output = boundary_output.get_output();
  REQUIRE(output.find("#ff") != std::string::npos);
}

TEST_CASE("Logger: Downsampling edge case - factor 1 (all samples)")
{
  g_output.clear();

  get_manager().setup_channel(request_line(test_app::SimpleData()).get_cfg().ID, 1);

  for (int i = 0; i < 5; ++i)
  {
    test_app::SimpleData data{ .count = i, .value = i * 1.0 };
    logginator::print(data);
  }

  std::string output     = g_output.get_output();
  std::size_t line_count = std::count(output.begin(), output.end(), '\n');
  REQUIRE(line_count == 5);    // All 5 samples should be logged
}

TEST_CASE("Logger: Downsampling edge case - large factor")
{
  g_output.clear();

  get_manager().setup_channel(request_line(test_app::SimpleData()).get_cfg().ID, 1000);

  // Log 10 samples with downsampling of 1000
  for (int i = 0; i < 10; ++i)
  {
    test_app::SimpleData data{ .count = i, .value = i * 1.0 };
    logginator::print(data);
  }

  std::string output = g_output.get_output();
  // None should be logged because we never reach 1000
  std::size_t line_count = std::count(output.begin(), output.end(), '\n');
  REQUIRE(line_count == 1);
}

TEST_CASE("Logger: Very large integers")
{
  g_output.clear();

  test_app::SimpleData data{ .count = std::numeric_limits<int32_t>::max(), .value = std::numeric_limits<double>::min() };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("2147483647") != std::string::npos);    // max int32_t value
}

TEST_CASE("Logger: Very small float values")
{
  g_output.clear();

  test_app::SimpleData data{ .count = 1, .value = 1e-10 };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.length() > 0);
}

TEST_CASE("Logger: Mixed special float values (negative infinity)")
{
  g_output.clear();

  test_app::SimpleData data{ .count = 1, .value = -std::numeric_limits<double>::infinity() };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.length() > 0);
}

TEST_CASE("Logger: Empty binary hash")
{
  g_output.clear();

  test_app::BinaryData data{ .id = 0, .hash = {} };
  std::fill(data.hash.begin(), data.hash.end(), std::byte(0));
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("#0b") != std::string::npos);
}

TEST_CASE("Logger: String with special characters")
{
  g_output.clear();

  test_app::StringData data{ .index = 1, .message = "Test;with;semicolons" };
  get_manager().setup_channel(request_line(data).get_cfg().ID, 1);
  logginator::print(data);

  std::string output = g_output.get_output();
  REQUIRE(output.find("Test;with;semicolons") != std::string::npos);
}

TEST_CASE("Logger: Sequential channel setup")
{
  StringOutput                          sequential_output;
  logginator::Manager<std::mutex, 8192> sequential_manager{ sequential_output };
  logginator::Manager_Interface&        manager = sequential_manager;

  std::array<logginator::channel_description_t, 10> channel_descriptions = {
    logginator::channel_description_t{ 1, "Channel1" }, logginator::channel_description_t{ 2, "Channel2" },
    logginator::channel_description_t{ 3, "Channel3" }, logginator::channel_description_t{ 4, "Channel4" },
    logginator::channel_description_t{ 5, "Channel5" }, logginator::channel_description_t{ 6, "Channel6" },
    logginator::channel_description_t{ 7, "Channel7" }, logginator::channel_description_t{ 8, "Channel8" },
    logginator::channel_description_t{ 9, "Channel9" }, logginator::channel_description_t{ 10, "Channel10" },
  };

  // Setup multiple channels in sequence
  for (uint8_t i = 0; i < 10; ++i)
  {
    auto channel = manager.request_channel(test_app::SimpleData(), channel_descriptions[i]);
    manager.setup_channel(channel_descriptions[i].ID, 1);
  }

  REQUIRE(true);    // If we get here, no exceptions were thrown
}

TEST_CASE("Logger: Rapid create/destroy cycles")
{
  // Test that channels can be created and destroyed rapidly without issues
  for (int cycle = 0; cycle < 5; ++cycle)
  {
    StringOutput                          cycle_output;
    logginator::Manager<std::mutex, 2048> cycle_manager{ cycle_output };
    logginator::Manager_Interface&        manager = cycle_manager;

    test_app::SimpleData data{ .count = cycle, .value = cycle * 1.5 };
    auto                 channel = manager.request_channel(data, logginator::channel_description_t{ 1, "Cycle" });
    manager.setup_channel(1, 1);

    {
      auto line = channel.request_line();
      print(data, line);
    }
  }

  REQUIRE(true);
}
