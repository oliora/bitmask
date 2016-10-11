# bitmask
A generic implementation of the [BitmaskType](http://en.cppreference.com/w/cpp/concept/BitmaskType) C++ concept.

The library is a tiny single header without any dependencies except the standard library. And yes, it's pure C++11.

To use it just download [the latest version of `bitmask.hpp`](include/boost/bitmask.hpp) and put somewhere in your project.

Please note that despite `bitmask` library uses the `boost` namespace it is not an official [Boost](www.boost.org) library.

## Inspiring Example

```cpp
enum class open_mode {
    binary,
    app,
    in,
    out,
    trunc,
    ate
};

BOOST_BITMASK_MAX_ELEMENT(open_mode, ate);

File open_file(const char *filename, boost::bitmask<open_mode> mode);

auto f = open_file("test.txt", open_mode::out | open_mode::binary | open_mode::trunk);
```
## Intro

## 4 ways to define a bitmask

## Available operations

## How to build and run tests

It's easy: you only need to install CMake 3.1 or above.

```
cd <bitmask_dir>
mkdir ws && cd ws
cmake ..
make
make test
```

## Acknowledgement

- LLVM libc++ for inspiring {{std::regex_constants}} implementation.
- [Jonathan Müller](https://twitter.com/foonathan) for great post https://foonathan.github.io/blog/2016/09/09/cpp14-concepts.html that inspired me to add compile-time tests (yet in progress).

## Tested compilers

- MacOS: Apple Clang 7.3.0
- Linux: G++ 4.8.5 and G++ 5.4.0
- Windows: Visual C++ 2015 (14.0) Update 3.
  *Note that some tests and compile time checks are disabled because of limited support for expression SFINAE.*

## License
This is free and unencumbered software released into the public domain.

Please check [LICENSE](LICENSE) if you need more details.
