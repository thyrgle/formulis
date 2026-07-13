#include "formulis/formulis.hpp"

#include <catch2/catch_test_macros.hpp>

auto my_func(const int& x) -> int
{
  return 3 * x + 1;
}
REGISTER_UNARY_PROC(my_func, int)

TEST_CASE("simple terms", "[simple]")
{
  SECTION("change from 2 to 3")
  {
    term x(2);
    REQUIRE(x() == 2);
    x.set(3);
    REQUIRE(x() == 3);
  }
  SECTION("increment test")
  {
    term x(2);
    REQUIRE(x() == 2);
    ++x;
    REQUIRE(x() == 3);
    int y = x++;
    REQUIRE(y == 3);
    REQUIRE(x() == 4);
  }
  SECTION("decrement test")
  {
    term x(4);
    REQUIRE(x() == 4);
    --x;
    REQUIRE(x() == 3);
    int y = x--;
    REQUIRE(y == 3);
    REQUIRE(x() == 2);
  }
  SECTION("+= test")
  {
    term x(2);
    REQUIRE(x() == 2);
    x += 2;
    REQUIRE(x() == 4);
  }
  SECTION("-= test")
  {
    term x(4);
    REQUIRE(x() == 4);
    x -= 2;
    REQUIRE(x() == 2);
  }
}

TEST_CASE("2 term formulas", "[simple]")
{
  SECTION("basic formula [+]")
  {
    term x(2);
    term y(2);
    auto z = x + y;
    REQUIRE(z() == 4);
    x.set(3);
    REQUIRE(z() == 5);
    y.set(3);
    REQUIRE(z() == 6);
  }

  SECTION("basic formula [+] and const")
  {
    term x(2);
    auto z = x + 1;
    REQUIRE(z() == 3);
    x.set(3);
    REQUIRE(z() == 4);
    auto w = 1 + x;
    REQUIRE(w() == 4);
    x.set(4);
    REQUIRE(w() == 5);
  }

  SECTION("basic formula [*]")
  {
    term x(2);
    term y(2);
    auto z = x * y;
    REQUIRE(z() == 4);
    x.set(3);
    REQUIRE(z() == 6);
    y.set(3);
    REQUIRE(z() == 9);
  }

  SECTION("basic formula [/]")
  {
    term x(2);
    term y(2);
    auto z = x / y;
    REQUIRE(z() == 1);
    x.set(3);
    REQUIRE(z() == 1);
    y.set(4);
    REQUIRE(z() == 0);
  }

  SECTION("basic formula [%]")
  {
    term x(2);
    term y(2);
    auto z = x % y;
    REQUIRE(z() == 0);
    x.set(3);
    REQUIRE(z() == 1);
    y.set(4);
    REQUIRE(z() == 3);
  }
}

TEST_CASE("3 term formulas", "[simple]")
{
  SECTION("test formula creation and updating")
  {
    term x(2);
    term y(2);
    term z(2);
    auto w = x + y + z;
    REQUIRE(w() == 6);
    x.set(3);
    REQUIRE(w() == 7);
    y.set(3);
    REQUIRE(w() == 8);
    z.set(3);
    REQUIRE(w.eval() == 9);
  }
  SECTION("test formula creation and updating (different parenthesis)")
  {
    term x(2);
    term y(2);
    term z(2);
    auto w = x + (y + z);
    REQUIRE(w() == 6);
    x.set(3);
    REQUIRE(w() == 7);
    y.set(3);
    REQUIRE(w() == 8);
    z.set(3);
    REQUIRE(w() == 9);
  }
}

TEST_CASE("4 term formulas", "[simple]")
{
  SECTION("test formula creation and updating")
  {
    term x(2);
    term y(2);
    term z(2);
    term w(2);
    auto o = x + y + z + w;
    REQUIRE(o() == 8);
    x.set(3);
    REQUIRE(o() == 9);
    y.set(3);
    REQUIRE(o() == 10);
    z.set(3);
    REQUIRE(o() == 11);
    w.set(3);
    REQUIRE(o() == 12);
  }
  SECTION("test formula creation and updating (different parenthesis)")
  {
    term x(2);
    term y(2);
    term z(2);
    term w(2);
    auto o = (x + y) + (z + w);
    REQUIRE(o() == 8);
    x.set(3);
    REQUIRE(o() == 9);
    y.set(3);
    REQUIRE(o() == 10);
    z.set(3);
    REQUIRE(o() == 11);
    w.set(3);
    REQUIRE(o() == 12);
  }
}

TEST_CASE("custom operator", "[simple]")
{
  SECTION("test creation of custom unary op")
  {
    term x(3);
    auto z = my_func(x);
    REQUIRE(z() == 10);
    x.set(4);
    REQUIRE(z() == 13);
  }
}
