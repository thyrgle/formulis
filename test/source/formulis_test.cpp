#include <iostream>
#include <memory>
#include <sstream>
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
  std::shared_ptr<term<int>> x = std::make_shared<term<int>>(2);
  std::shared_ptr<term<int>> y = std::make_shared<term<int>>(3);
  std::shared_ptr<formula<int>> z = x + y;
  return z->eval();
}

static auto simple_form_change() -> int
{
  std::shared_ptr<term<int>> x = std::make_shared<term<int>>(2);
  std::shared_ptr<term<int>> y = std::make_shared<term<int>>(3);
  std::shared_ptr<formula<int>> z = x + y;
  x->set(3);
  return z->eval();
}

static auto listen_z(int old_val, int new_val)
{
  std::cout << "z changed from " << old_val << " to " << new_val;
}

static auto simple_on_change() -> void
{
  std::shared_ptr<term<int>> x = std::make_shared<term<int>>(2);
  std::shared_ptr<term<int>> y = std::make_shared<term<int>>(3);
  std::shared_ptr<formula<int>> z = x + y;
  z->on_change(listen_z);
  x->set(3);
}

static auto simple_on_change_sub() -> void
{
  std::shared_ptr<term<int>> x = std::make_shared<term<int>>(4);
  std::shared_ptr<term<int>> y = std::make_shared<term<int>>(3);
  std::shared_ptr<formula<int>> z = x - y;
  z->on_change(listen_z);
  x->set(3);
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

TEST_CASE("Simple on_change for formula.", "[simple_on_change_sub]")
{
  simple_on_change();
  std::streambuf* old_buffer = std::cout.rdbuf();

  std::stringstream capture_output;
  std::cout.rdbuf(capture_output.rdbuf());

  simple_on_change_sub();

  std::cout.rdbuf(old_buffer);
  REQUIRE(capture_output.str() == "z changed from 1 to 0");
}
