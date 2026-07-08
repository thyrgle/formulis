# What is it?

## Reactive Programming Made Simple

`formulis` allows you to make event listeners using formulae. What does that mean? First, consider this small example:

```cpp
#include <memory>
#include "formulis/formulis.hpp"

auto main() -> int
{
  auto x = std::make_shared<term<int>>(3); // Not actually an int, but a term that holds an int value.
  auto y = std::make_shared<term<int>>(5); // Same as above.
  auto z = x + y; // z = x + y -> 3 + 5 = 8.
  x->set(4); // Change the value of the term x.
  // z = x + y -> 4 + 5 = 9 now. Everything is updated automatically.
}
```

Formulae update to reflect changes in their terms. Furthermore, we can listen for changes:

```cpp
#include <iostream>
#include <memory>
#include "formulis/formulis.hpp"

auto listen_to_z(int old_val, int new_val) -> void
{
  std::cout << "z changed from " << old_val << " to " << new_val << endl;
}

auto main() -> int
{
  auto x = std::make_shared<term<int>>(3);
  auto y = std::make_shared<term<int>>(5);
  auto z = x + y; // z = x + y -> 3 + 5 = 8.
  z->on_change(listen_to_z); 
  x->set(4); // Change the value of the term x.
  // Prints "z changed from 8 to 9".
}
```

This provides powerful ways of organizing the execution flow of programs. Interested in using the library? Checkout the next section [Using the Library](\ref md_using].
