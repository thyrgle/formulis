#include <iostream>
#include <utility>

#include "formulis/formulis.hpp"

#include <catch2/catch_test_macros.hpp>

static auto simple_term_change() -> int
{
  term test = term(2);
  test.set(3);
  return test.unwrap();
}

static auto simple_form_construct() -> int
{
  term x = term(2);
  term y = term(3);
  formula z = x + y;
  return z.eval();
}

static auto simple_form_change() -> int
{
  term x = term(2);
  term y = term(3);
  formula z = x + y;
  x.set(3);
  return z.eval();
}

int old_value = 0;
int new_value = 0;

static auto listen_z(int old_val, int new_val)
{
  std::cout << "z changed from " << old_val << " to " << new_val;
}

static auto simple_on_change() -> void
{
  term x = term(2);
  term y = term(3);
  formula z = x + y;
  z.on_change(listen_z);
  x.set(3);
}

TEST_CASE("Simple term change", "[simple_term_change]")
{
  REQUIRE(simple_term_change() == 3);
}

TEST_CASE("Simple formula construct", "[simple_form_construct]")
{
  REQUIRE(simple_form_construct() == 5);
}

TEST_CASE("Simple formula change", "[simple_form_change]")
{
  REQUIRE(simple_form_change() == 6);
}

TEST_CASE("Simple on_change for formula.", "[simple_on_change]")
{
  simple_on_change();
  std::streambuf* old_buffer = std::cout.rdbuf();

  std::stringstream capture_output;
  std::cout.rdbuf(capture_output.rdbuf());

  simple_on_change();

  std::cout.rdbuf(old_buffer);
  REQUIRE(capture_output.str() == "z changed from 5 to 6");
}
