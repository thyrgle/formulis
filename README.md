# formulis

`formulis` allows you to make event listeners using formulae. What does that mean? First, consider this small example:

```cpp
#include "formulis/formulis.hpp"

auto main() -> int
{
    term x = 3; // Not actually an int, but a term that holds an int value.
    term y = 5; // Same as above.
    formula z = x + y; // z = x + y -> 3 + 5 = 8.
    x.set(4); // Change the value of the term x.
    // z = x + y -> 4 + 5 = 9 now. Everything is updated automatically.
}
```

Formulae update to reflect changes in their terms. Furthermore, we can listen for changes:

```cpp
#include <iostream>
#include "formulis/formulis.hpp"

auto listen_to_z(int old_val, int new_val) -> void
{
    std::cout << "z changed from " << old_val << " to " << new_val << endl;
}

auto main() -> int
{
    term x = 3;
    term y = 5;
    formula z = x + y; // z = x + y -> 3 + 5 = 8.
    z.on_change(listen_to_z); 
    x.set(4); // Change the value of the term x.
    // Prints "z changed from 8 to 9".
}
```

This provides powerful ways of organizing the execution flow of programs.

# Building and installing

The library was created using modified [cmake-init](https://github.com/friendlyanon/cmake-init) scaffolding for [conan](https://conan.io/) projects. First, ensure that [`cmake`](https://cmake.org/) and [`conan`](https://conan.io/) are installed. Next, ensure a "Conan Profile" is established for your computer with:

```bash
conan profile detect --name defualt
```

This needs to be only ran once. Then run:

```bash
conan install . -S build_type=Debug -b missing
```

This will have `conan` install the third party dependencies ([Catch2](https://github.com/catchorg/Catch2) for testing). Finally, to build everything:

```bash
cmake --preset=dev
cmake --build --preset=dev
```

and to test:

```bash
ctest --preset=dev
```
