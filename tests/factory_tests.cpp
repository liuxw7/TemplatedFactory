#pragma warning(push)
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#define _CRT_SECURE_NO_WARNINGS 1
#include <gtest/gtest.h>
#undef _CRT_SECURE_NO_WARNINGS
#pragma warning(pop)
#include <factory/factory.h>

void tell_methods(std::string_view view) {
  constexpr bool be_noisy = true;
  if constexpr (be_noisy) {
    std::cout << view.data() << std::endl;
  }
}
enum class test_enum : uint_fast32_t {
  first = 1,
  second = 2,
  third = 3,
};

class noisy_base_abstract {
 public:
  virtual int do_stuff() = 0;
};
class noisy_base : public noisy_base_abstract {
 public:
  noisy_base() { tell_methods("noisy_base()"); }
  noisy_base(const noisy_base&) { tell_methods("noisy_base(const noisy_base&)"); }
  noisy_base(noisy_base&&) { tell_methods("noisy_base(noisy_base&&)"); }
  noisy_base& operator=(const noisy_base&) {
    tell_methods("operator=(const noisy_base&)");
    return *this;
  }
  noisy_base& operator=(noisy_base&&) {
    tell_methods("operator=(noisy_base&&)");
    return *this;
  }
  ~noisy_base() { tell_methods("~noisy_base()"); }
  int do_stuff() {
    tell_methods("noisy_base::do_stuff()");
    return 0;
  };
};
struct noisy1 : public noisy_base {
  int do_stuff() {
    tell_methods("noisy1::do_stuff()");
    return 1;
  }
  ~noisy1() { tell_methods("~noisy1()"); }
};
struct noisy2 : public noisy_base {
  virtual int do_stuff() {
    tell_methods("noisy2::do_stuff()");
    return 2;
  }
  virtual ~noisy2() { tell_methods("~noisy2()"); }
};
struct noisy3 : public noisy_base {
  virtual int do_stuff() {
    tell_methods("noisy3::do_stuff()");
    return 3;
  }
  virtual ~noisy3() { tell_methods("~noisy3()"); }
};

constexpr constexpr_factory<int, 2, test_enum> compile_time_factory = {std::make_pair<test_enum, int>(test_enum::first, 1),
                                                                       std::make_pair<test_enum, int>(test_enum::second, 2)};

TEST(factory, simple_key) {
  factory<int> test_factory{};
  auto key = std::string_view{"first_key"};
  test_factory.register_(key, 5);
  auto temp = test_factory.copy(key);
  ASSERT_EQ(temp, 5);
}

TEST(factory, noisy) {
  factory<noisy_base, test_enum> my_factory;
  my_factory.register_<noisy1>(test_enum::first);
  my_factory.register_<noisy2>(test_enum::second);
  my_factory.register_<noisy3>(test_enum::third);
  //Object Slicing, If you want you can static_cast it with the template Parameter of copy
  ASSERT_EQ(my_factory.copy(test_enum::first).do_stuff(), 0);
  ASSERT_EQ(my_factory.copy(test_enum::second).do_stuff(), 0);
  ASSERT_EQ(my_factory.copy(test_enum::third).do_stuff(), 0);
}

TEST(factory, noisy_abstract) {
  factory<noisy_base_abstract, test_enum> my_factory;
  my_factory.register_<noisy1>(test_enum::first);
  my_factory.register_<noisy2>(test_enum::second);
  my_factory.register_<noisy3>(test_enum::third);
  ASSERT_EQ(my_factory.copy<noisy1>(test_enum::first).do_stuff(), static_cast<int>(test_enum::first));
  ASSERT_EQ(my_factory.copy<noisy2>(test_enum::second).do_stuff(), static_cast<int>(test_enum::second));
  ASSERT_EQ(my_factory.copy<noisy3>(test_enum::third).do_stuff(), static_cast<int>(test_enum::third));
}

TEST(factory, compile_time_factory) {
  ASSERT_EQ(1, compile_time_factory.copy(test_enum::first));
}