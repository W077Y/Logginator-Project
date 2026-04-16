#include <cstddef>
#include <string_view>
#include <ut_catch.hpp>
//
#include <array>
#include <logginator.hpp>

namespace my_app
{
  struct type_a
  {
    signed long long int time    = 0;
    double               value_1 = std::numeric_limits<double>::quiet_NaN();
    double               value_2 = std::numeric_limits<double>::quiet_NaN();
  };

  logginator::line_t request_line(type_a const&);

  void print(type_a const& value, logginator::line_t& line);

  struct type_b
  {
    unsigned long long int time    = 0;
    double                 value_1 = std::numeric_limits<double>::quiet_NaN();
    double                 value_2 = std::numeric_limits<double>::quiet_NaN();
    double                 value_3 = std::numeric_limits<double>::quiet_NaN();
    double                 value_4 = std::numeric_limits<double>::quiet_NaN();
  };

  logginator::line_t request_line(type_b const&);

  void print(type_b const& value, logginator::line_t& line);

  struct type_c
  {
    std::size_t      time = 0;
    std::string_view txt  = "";
  };

  logginator::line_t request_line(type_c const&);

  void print(type_c const& value, logginator::line_t& line);

  struct type_d
  {
    std::size_t               time  = 0;
    std::array<std::byte, 32> hash1 = {};
    std::array<std::byte, 32> hash2 = {};
    std::array<std::byte, 3>  hash3 = {
      std::byte(0xF0),
      std::byte(0xFF),
      std::byte(0xFF),
    };
  };

  logginator::line_t request_line(type_d const&);

  void print(type_d const& value, logginator::line_t& line);

}    // namespace my_app

logginator::Manager_Interface& request_manager();
void                           init_logginator();

TEST_CASE()
{
  init_logginator();
  //
  std::size_t i = 0;
  for (; i < 5; i++)
  {
    my_app::type_a a{ .time = std::numeric_limits<signed long long int>::min(), .value_1 = i * 2.0 / 3.0, .value_2 = i * 3.0 };
    my_app::type_b b{ .time = i, .value_1 = i * 1.5, .value_2 = i * 3.1, .value_3 = i * 1.0 / 6.0, .value_4 = i * 1.0 / 12.0 };
    my_app::type_c c{ .time = i, .txt = "Hallo" };
    my_app::type_d d{ .time = i, .hash1 = {}, .hash2 = {} };
    for (std::size_t j = 0; j < d.hash2.size(); j++)
      d.hash2[j] = std::byte(0xFF);
    logginator::print(a);
    logginator::print(b);
    logginator::print(c);
    logginator::print(d);
  }
  {
    request_manager().setup_channel(2, 3);
    request_manager().print_channels();
  }
  for (; i < 10; i++)
  {
    my_app::type_a a{ .time = std::numeric_limits<signed long long int>::max(), .value_1 = i * 2.0 / 3.0, .value_2 = i * 3.0 };
    my_app::type_b b{ .time = i, .value_1 = i * 1.5, .value_2 = i * 3.1, .value_3 = i * -1.0 / 6.0, .value_4 = i * 1.0 / 12.0 };
    my_app::type_c c{ .time = i, .txt = "aa" };
    if (i % 2 == 1)
      c = { .time = i, .txt = "bb" };
    my_app::type_d d{ .time = i, .hash1 = {}, .hash2 = {} };
    for (std::size_t j = 0; j < d.hash2.size(); j++)
      d.hash2[j] = std::byte(j);

    logginator::print(a);
    logginator::print(b);
    logginator::print(c);
    logginator::print(d);
  }
}

void init_logginator()
{
  auto& man = request_manager();
  request_line(my_app::type_a{});
  request_line(my_app::type_b{});
  request_line(my_app::type_c{});
  request_line(my_app::type_d{});
  man.print_channels();
}

#include <iostream>

//
void my_app::print(type_a const& value, logginator::line_t& line)
{
  using namespace logginator;
  using FI = ColumnDescriptionInt::Format;
  using FF = ColumnDescriptionFloat::Format;
  line.add(ColumnDescriptionInt{ "Time", "s", FI::ascii }, value.time);
  line.add(ColumnDescriptionFloat{ "value_1", "degC", FF::ascii }, value.value_1);
  line.add(ColumnDescriptionFloat{ "value_2", "degC", FF::ascii }, value.value_2);
}

void my_app::print(type_b const& value, logginator::line_t& line)
{
  using namespace logginator;
  using FI = ColumnDescriptionInt::Format;
  using FF = ColumnDescriptionFloat::Format;
  line.add(ColumnDescriptionInt{ "Time", "s", FI::ascii }, value.time);
  line.add(ColumnDescriptionFloat{ "value_2", "degC", FF::ascii }, value.value_1);
  line.add(ColumnDescriptionFloat{ "value_3", "degC", FF::ascii }, value.value_2);
  line.add(ColumnDescriptionFloat{ "value_4", "degC", FF::ascii }, value.value_3);
  line.add(ColumnDescriptionFloat{ "value_5", "degC", FF::ascii }, value.value_4);
}

void my_app::print(type_c const& value, logginator::line_t& line)
{
  using namespace logginator;
  using FI = ColumnDescriptionInt::Format;
  using FS = ColumnDescriptionString::Format;
  line.add(ColumnDescriptionInt{ "Time", "s", FI::ascii }, value.time);
  line.add(ColumnDescriptionString{ "txt", "", FS::ascii }, value.txt);
}

void my_app::print(type_d const& value, logginator::line_t& line)
{
  using namespace logginator;
  using FI = ColumnDescriptionInt::Format;
  using FB = ColumnDescriptionBinary::Format;
  line.add(ColumnDescriptionInt{ "Time", "s", FI::ascii }, value.time);
  line.add(ColumnDescriptionBinary{ "hash1", "", FB::b64 }, std::span<std::byte const>{ value.hash1 });
  line.add(ColumnDescriptionBinary{ "hash2", "", FB::b64 }, std::span<std::byte const>{ value.hash2 });
  line.add(ColumnDescriptionBinary{ "hash3", "", FB::b64 }, std::span<std::byte const>{ value.hash3 });
}

//
#include <iostream>
#include <mutex>

logginator::Manager_Interface& request_manager()
{
  static class wrapper_t final: public logginator::Manager_Interface::Output_Interface
  {
    void operator()(std::string_view msg) noexcept override
    {
      try
      {
        std::cout << msg;
      }
      catch (std::exception const& e)
      {
        std::cout << e.what() << std::endl;
      }
    }
  } wrapper;
  static logginator::Manager<std::mutex, 2048> obj{ wrapper };
  return obj;
}

logginator::line_t my_app::request_line(type_a const& value)
{
  using namespace logginator;
  static auto obj = request_manager().request_channel(value, ChannelDescription{ 1, "Channel 1" }, 1);
  return obj.request_line();
}

logginator::line_t my_app::request_line(type_b const& value)
{
  using namespace logginator;
  static auto obj = request_manager().request_channel(value, ChannelDescription{ 2, "Channel 2" }, 1);
  return obj.request_line();
}

logginator::line_t my_app::request_line(type_c const& value)
{
  using namespace logginator;
  static auto obj = request_manager().request_channel(value, ChannelDescription{ 3, "Channel 3" }, 1);
  return obj.request_line();
}
logginator::line_t my_app::request_line(type_d const& value)
{
  using namespace logginator;
  static auto obj = request_manager().request_channel(value, ChannelDescription{ 4, "Channel 4" }, 1);
  return obj.request_line();
}
