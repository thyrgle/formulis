## Testing from Source

The library was created using modified [cmake-init](https://github.com/friendlyanon/cmake-init) scaffolding for [conan](https://conan.io/) projects. First, ensure that [`cmake`](https://cmake.org/) and [`conan`](https://conan.io/) are installed. Next, ensure a "Conan Profile" is established for your computer with:

```bash
conan profile detect --name defualt
```

This needs to be only ran once. Then run:

```bash
conan install . -S build_type=Debug -b missing
```

This will have `conan` install the third party dependencies ([Catch2](https://github.com/catchorg/Catch2) for testing). Set up the "presets" once:

```bash
cmake --preset=dev
```

Finally, to build:

```bash
cmake --build --preset=dev
```

and to test:

```bash
ctest --preset=dev
```
