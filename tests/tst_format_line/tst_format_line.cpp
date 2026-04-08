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
    signed long long int      time    = 0;
    double                    value_1 = std::numeric_limits<double>::quiet_NaN();
    std::array<std::byte, 13> value_2 = {};
    const char*               value_3 = "msg";
  };

  logginator::line_t request_line(type_a const&);

  void print(type_a const& value, logginator::line_t& line);
}    // namespace my_app

logginator::Manager_Interface& request_manager();
#include <iostream>

TEST_CASE()
{
  //
  logginator::request_line(my_app::type_a{});

  //
  {
    auto& man = request_manager();
    man.setup_channel(1, 1);
    man.print_channels();
  }

  //
  std::size_t i = 0;
  for (; i < 2; i++)
  {
    my_app::type_a a{ .time = std::numeric_limits<signed long long int>::min(), .value_1 = i * 2.0 / 3.0 };
    logginator::print(a);
  }
  {
    std::cout << "---" << std::endl;
    request_manager().print_channels();
  }
  for (; i < 4; i++)
  {
    my_app::type_a a{ .time = std::numeric_limits<signed long long int>::max(), .value_1 = i * 2.0 / 3.0};
    logginator::print(a);
  }
}

//
void my_app::print(type_a const& value, logginator::line_t& line)
{
  using namespace logginator;
  using FI = column_description_int::Format;
  using FF = column_description_float::Format;
  using FB = column_description_binary::Format;
  using FS = column_description_string::Format;
  line.add(column_description_int{ "Time", "s", FI::ascii }, value.time);
  line.add(column_description_int{ "Time", "s", FI::hex }, value.time);
  line.add(column_description_int{ "Time", "s", FI::b64 }, value.time);
  line.add(column_description_int{ "Time", "s", FI::default_fmt }, value.time);
  line.add(column_description_float{ "value_1", "degC", FF::ascii }, value.value_1);
  line.add(column_description_float{ "value_1", "degC", FF::ascii_fixed }, value.value_1);
  line.add(column_description_float{ "value_1", "degC", FF::ascii_scientific }, value.value_1);
  line.add(column_description_float{ "value_1", "degC", FF::hex }, value.value_1);
  line.add(column_description_float{ "value_1", "degC", FF::b64 }, value.value_1);
  line.add(column_description_float{ "value_1", "degC", FF::default_fmt }, value.value_1);
  line.add(column_description_binary{ "value_2", "degC", FB::b64 }, value.value_2);
  line.add(column_description_binary{ "value_2", "degC", FB::default_fmt }, value.value_2);
  line.add(column_description_string{ "value_3", "degC", FS::ascii }, value.value_3);
  line.add(column_description_string{ "value_3", "degC", FS::default_fmt }, value.value_3);
}

//
#include <iostream>
#include <logginator-manager.hpp>
#include <logginator-printer.hpp>
#include <mutex>

logginator::Manager_Interface& request_manager()
{
  static class wrapper_t final: public logginator::Manager::Output_Interface
  {
    void operator()(std::string_view msg) override { std::cout << msg; }
  } wrapper;
  static logginator::Manager obj{ wrapper };
  return obj;
}

logginator::line_t my_app::request_line(type_a const&)
{
  static logginator::Printer<std::mutex, type_a, 1024> obj{ request_manager(), logginator::channel_description_t{ 1, "my little printer 1" } };
  return obj.request_line();
}
