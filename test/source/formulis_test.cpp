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
    REQUIRE(x.unwrap() == 2);
    x.set(3);
    REQUIRE(x.unwrap() == 3);
  }
  SECTION("increment test")
  {
    term x(2);
    REQUIRE(x.unwrap() == 2);
    ++x;
    REQUIRE(x.unwrap() == 3);
    int y = x++;
    REQUIRE(y == 3);
    REQUIRE(x.unwrap() == 4);
  }
  SECTION("decrement test")
  {
    term x(4);
    REQUIRE(x.unwrap() == 4);
    --x;
    REQUIRE(x.unwrap() == 3);
    int y = x--;
    REQUIRE(y == 3);
    REQUIRE(x.unwrap() == 2);
  }
  SECTION("+= test")
  {
    term x(2);
    REQUIRE(x.unwrap() == 2);
    x += 2;
    REQUIRE(x.unwrap() == 4);
  }
  SECTION("-= test")
  {
    term x(4);
    REQUIRE(x.unwrap() == 4);
    x -= 2;
    REQUIRE(x.unwrap() == 2);
  }
}

TEST_CASE("2 term formulas", "[simple]")
{
  SECTION("basic formula [+]")
  {
    term x(2);
    term y(2);
    auto z = x + y;
    REQUIRE(z.eval() == 4);
    x.set(3);
    REQUIRE(z.eval() == 5);
    y.set(3);
    REQUIRE(z.eval() == 6);
  }

  SECTION("basic formula [+] and const")
  {
    term x(2);
    auto z = x + 1;
    REQUIRE(z.eval() == 3);
    x.set(3);
    REQUIRE(z.eval() == 4);
    auto w = 1 + x;
    REQUIRE(w.eval() == 4);
    x.set(4);
    REQUIRE(w.eval() == 5);
  }

  SECTION("basic formula [*]")
  {
    term x(2);
    term y(2);
    auto z = x * y;
    REQUIRE(z.eval() == 4);
    x.set(3);
    REQUIRE(z.eval() == 6);
    y.set(3);
    REQUIRE(z.eval() == 9);
  }

  SECTION("basic formula [/]")
  {
    term x(2);
    term y(2);
    auto z = x / y;
    REQUIRE(z.eval() == 1);
    x.set(3);
    REQUIRE(z.eval() == 1);
    y.set(4);
    REQUIRE(z.eval() == 0);
  }

  SECTION("basic formula [%]")
  {
    term x(2);
    term y(2);
    auto z = x % y;
    REQUIRE(z.eval() == 0);
    x.set(3);
    REQUIRE(z.eval() == 1);
    y.set(4);
    REQUIRE(z.eval() == 3);
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
    REQUIRE(w.eval() == 6);
    x.set(3);
    REQUIRE(w.eval() == 7);
    y.set(3);
    REQUIRE(w.eval() == 8);
    z.set(3);
    REQUIRE(w.eval() == 9);
  }
  SECTION("test formula creation and updating (different parenthesis)")
  {
    term x(2);
    term y(2);
    term z(2);
    auto w = x + (y + z);
    REQUIRE(w.eval() == 6);
    x.set(3);
    REQUIRE(w.eval() == 7);
    y.set(3);
    REQUIRE(w.eval() == 8);
    z.set(3);
    REQUIRE(w.eval() == 9);
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
    REQUIRE(o.eval() == 8);
    x.set(3);
    REQUIRE(o.eval() == 9);
    y.set(3);
    REQUIRE(o.eval() == 10);
    z.set(3);
    REQUIRE(o.eval() == 11);
    w.set(3);
    REQUIRE(o.eval() == 12);
  }
  SECTION("test formula creation and updating (different parenthesis)")
  {
    term x(2);
    term y(2);
    term z(2);
    term w(2);
    auto o = (x + y) + (z + w);
    REQUIRE(o.eval() == 8);
    x.set(3);
    REQUIRE(o.eval() == 9);
    y.set(3);
    REQUIRE(o.eval() == 10);
    z.set(3);
    REQUIRE(o.eval() == 11);
    w.set(3);
    REQUIRE(o.eval() == 12);
  }
}

TEST_CASE("custom operator", "[simple]")
{
  SECTION("test creation of custom unary op")
  {
    term x(3);
    auto z = my_func(x);
    REQUIRE(z.eval() == 10);
    x.set(4);
    REQUIRE(z.eval() == 13);
  }
}
