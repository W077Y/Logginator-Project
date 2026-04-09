#include "catch.hpp"

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

  man.setup_channel(request_line(my_app::type_a{}).get_cfg().ID, 1);
  man.setup_channel(request_line(my_app::type_b{}).get_cfg().ID, 2);
  man.setup_channel(request_line(my_app::type_c{}).get_cfg().ID, 3);
  man.setup_channel(request_line(my_app::type_d{}).get_cfg().ID, 4);
  man.print_channels();
}

#include <iostream>

//
void my_app::print(type_a const& value, logginator::line_t& line)
{
  using namespace logginator;
  using FI = column_description_int::Format;
  using FF = column_description_float::Format;
  line.add(column_description_int{ "Time", "s", FI::ascii }, value.time);
  line.add(column_description_float{ "value_1", "degC", FF::ascii }, value.value_1);
  line.add(column_description_float{ "value_2", "degC", FF::ascii }, value.value_2);
}

void my_app::print(type_b const& value, logginator::line_t& line)
{
  using namespace logginator;
  using FI = column_description_int::Format;
  using FF = column_description_float::Format;
  line.add(column_description_int{ "Time", "s", FI::ascii }, value.time);
  line.add(column_description_float{ "value_2", "degC", FF::ascii }, value.value_1);
  line.add(column_description_float{ "value_3", "degC", FF::ascii }, value.value_2);
  line.add(column_description_float{ "value_4", "degC", FF::ascii }, value.value_3);
  line.add(column_description_float{ "value_5", "degC", FF::ascii }, value.value_4);
}

void my_app::print(type_c const& value, logginator::line_t& line)
{
  using namespace logginator;
  using FI = column_description_int::Format;
  using FS = column_description_string::Format;
  line.add(column_description_int{ "Time", "s", FI::ascii }, value.time);
  line.add(column_description_string{ "txt", "", FS::ascii }, value.txt);
}

void my_app::print(type_d const& value, logginator::line_t& line)
{
  using namespace logginator;
  using FI = column_description_int::Format;
  using FB = column_description_binary::Format;
  line.add(column_description_int{ "Time", "s", FI::ascii }, value.time);
  line.add(column_description_binary{ "hash1", "", FB::b64 }, std::span<std::byte const>{ value.hash1 });
  line.add(column_description_binary{ "hash2", "", FB::b64 }, std::span<std::byte const>{ value.hash2 });
  line.add(column_description_binary{ "hash3", "", FB::b64 }, std::span<std::byte const>{ value.hash3 });
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
  static auto obj = request_manager().request_channel(value, channel_description_t{ 1, "Channel 1" });
  return obj.request_line();
}

logginator::line_t my_app::request_line(type_b const& value)
{
  using namespace logginator;
  static auto obj = request_manager().request_channel(value, channel_description_t{ 2, "Channel 2" });
  return obj.request_line();
}

logginator::line_t my_app::request_line(type_c const& value)
{
  using namespace logginator;
  static auto obj = request_manager().request_channel(value, channel_description_t{ 3, "Channel 3" });
  return obj.request_line();
}
logginator::line_t my_app::request_line(type_d const& value)
{
  using namespace logginator;
  static auto obj = request_manager().request_channel(value, channel_description_t{ 4, "Channel 4" });
  return obj.request_line();
}
