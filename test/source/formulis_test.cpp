#include <iostream>

#include "formulis/formulis.hpp"

#include <catch2/catch_test_macros.hpp>

static auto listen_z(int old_val, int new_val)
{
  std::cout << "z changed from " << old_val << " to " << new_val;
}

TEST_CASE("simple terms and formulas", "[simple]")
{
  SECTION("change from 2 to 3")
  {
    term x(2);
    REQUIRE(x.unwrap() == 2);
    x.set(3);
    REQUIRE(x.unwrap() == 3);
  }

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
