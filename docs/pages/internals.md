# Internals: A High Level Overview

## The Structure

There are really two major components to the library:

- The fundamental types.
  - From a user perspective: `formula` and `term` types.
  - From a contributor perspective, there are two flavors of `formula`. `formula` wrap around an expression (either a `unary_expr` or `bin_expr`).
- Macros for building a large combination of operator overloads.

## An Aside About Macros

We remark to not let the extreme macro use scare you! This is because there are so many operator overloads we use macros to help assist the generation of them all! To see why there are so many operators to overload, consider the expression:

```cpp
auto z = x + y + w + 2;
```

Thus, we must be able to add `formula` objects with each other, with terms, with constant, and `term` objects with `formula`, and constants, etc. The macros expand out to be operator overloading. Disregarding the macro use, we will make some notes about operator overloading later.

## The Types

First let us look at `term` type. A `term`, is, in essence, a wrapper around a value. A naive approach would be:

```cpp
template<typename T>
struct term
{
  T value;
}
```

But we have to remember that when `value` is updated for some term, say `t`, then `t` must also signal every `formula` object that uses it. A `formula` that uses it is a "parent". Thus, `term` also has a `std::vector` field called `m_parents` that keep track of this. The user *does not* specify parents ever. Instead, they are added when an operator is called. Thus, if we have something like:

```cpp
term x(3);
term y(2);
auto z = x + y;
```
`operator+` is called, and, breaking C++ conventions, `x` and `y` are modified so that the object returned to `z` is added as a parent to both of them. Thus, `term` more closely resembles:

```cpp
template<typename T>
class term
{
  T value;
  collection of parents;
  // ...
}
```

more precisely:

```cpp
template<typename T>
class term
{
  T m_value;
  std::vector<formula<T>*> m_parents;
  // ...
}
```

A similar structure is used for `formula` with some nuances. Now, we consider an slightly more complicated expression to exhibit this:

```cpp
term x(2);
term y(3);
term z(4);
auto w = x + y + z;
```

`w` is a formula that also (implicitly) evaluates to `w = (x + y) + z`. Thus, in addition to `w`, `x`, `y` and `z`, we also have `(x + y)` which is a formula that is never directly used by the user. It *is*, however, generated (and in fact placed on the heap). It has a parent, which is `w`. So we have a "parent" field for `formula` type too.

```cpp
template<typename T>
class formula
{
  std::vector<formula<T>*> m_parents;
}
```

In fact, we even have a `m_value` field of sorts in `formula`, but with a different name than `m_value`. This field is called `m_cached_val` because it does exactly that: It caches the value, but if a `term` child is updated, the value of the `formula` object must be recomputed and then stored again in `m_cached_val`.

If we have to recompute, how is this done? The secret is `unary_expr` and `bin_expr` structs mentioned above. `term` is essentially a value. A mathematical expression is roughly `term` objects combined by operators (either `unary` or `binary` in our case). Therefore, recursively speaking, a mathematical formula is a mathematical formula that has operations that either (for the base case) take terms, or take (a smaller) mathematical expression(s). Thus, we want to write something like:

```cpp
struct bin_op
{
  term or expression lhs;
  term or expression rhs;
  a binary operation;
}
```

To do so, we use the `std::variant` type `stmt`:

```cpp
template<typename T>
using stmt = std::variant<term<T>*, formula<T>*>;
```

and so we get precisely the definition of `bin_op`:

```cpp
struct bin_op
{
  stmt<T> lhs;
  stmt<T> rhs;
  std::function<T(T, T)> op;
}
```

Hence, to calculate the value it roughly becomes `eval(f) = f.op(eval(f.lhs), eval(f.rhs))` or if the operator is unary: `eval(f) = f.op(f.rhs)`. With the base case being a `term` is simply `unwrap` the internal value. To make this fast, of course, caching is used.

Lastly, we have yet to talk about the field `on_change` which is called when ever either a `term` or `formula` is changed. It is simply a collection of function objects that are supplied two paremeters (an old/previous and new/current val) and called one by one if a change is detected.

# The Destructor and Memory Management

Consider again the expression:

```cpp
term x(2);
term y(3);
term z(4);
auto w = x + y + z;
```
This makes two formula (`w` and `x + y`) and has 3 `term`. With how it is written, the memory allocation is a bit spooky. `x`, `y`, and `z` are as expected placed on the stack. However, `w` and `x + y` are both placed on the heap! Do not worry: When `x`, `y`, and `z` are cleaned up, so is `w` and `x + y`.

Suppose that `x` is removed from the stack first. The destructor first calls the destructor of the parent formula(s), which, in this case, is `w` and `x + y`. *But* before `w` and `x + y` deallocate, they also send a message to their immediate children `x`, `y`, and `z` that they should remove `w` and `x + y` from their parent list, respectively. *Then* they deallocate. Thus, memory is managed for the user!
