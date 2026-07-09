#include <iostream>

#include "formulis/formulis.hpp"

#include <catch2/catch_test_macros.hpp>

static auto listen_z(int old_val, int new_val)
{
  std::cout << "z changed from " << old_val << " to " << new_val;
}

TEST_CASE("simple terms and formulas", "[simple]")
{
  auto x = term<int>(2);
  SECTION("change from 2 to 3")
  {
    REQUIRE(x.unwrap() == 2);
    x.set(3);
    REQUIRE(x.unwrap() == 3);
  }
  auto y = term<int>(2);
  auto z = x + y;
  SECTION("basic formula")
  {
    REQUIRE(z.eval() == 4);
    x.set(3);
    REQUIRE(z.eval() == 5);
    y.set(3);
    REQUIRE(z.eval() == 6);
  }
}

TEST_CASE("3 term formulas", "[simple]")
{
  auto x = term<int>(2);
  auto y = term<int>(2);
  auto z = term<int>(2);
  auto w = x + y + z;
  SECTION("test formula creation and updating")
  {
    REQUIRE(w.eval() == 6);
    x.set(3);
    REQUIRE(w.eval() == 7);
    y.set(3);
    REQUIRE(w.eval() == 8);
    z.set(3);
    REQUIRE(w.eval() == 9);
  }
}
