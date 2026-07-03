#include "formulis/formulis.hpp"
#include <catch2/catch_test_macros.hpp>

static auto simple_term_change() -> int {
    term test = term(2);
    test.set(3);
    return test.unwrap();
}

static auto simple_form_construct() -> int {
    term x = term(2);
    term y = term(3);
    formula z = x + y;
    return z.eval();
}


static auto simple_form_change() -> int {
    term x = term(2);
    term y = term(3);
    formula z = x + y;
    x.set(3);
    return z.eval();
}


TEST_CASE("Simple term chnge", "[simple_term_change]")
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


