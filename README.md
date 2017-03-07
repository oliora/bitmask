# bitmask
A generic implementation of the [BitmaskType](http://en.cppreference.com/w/cpp/concept/BitmaskType) C++ concept.

The library is a tiny single header without any dependencies except the standard library. And yes, it's pure C++11 and constexpr.

To use it just download [the latest version of `bitmask.hpp`](include/boost/bitmask.hpp) and put somewhere in your project.

**Please note that despite `bitmask` library uses the `boost` namespace it is not an official Boost library.**

## Warm up example

```cpp
#include <bitmask.hpp>

// Define possible flags
enum class open_mode {
    binary = 0x01,
    app    = 0x02,
    in     = 0x04,
    out    = 0x08,
    trunc  = 0x10,
    ate    = 0x20
};

// Define bitmask
BOOST_BITMASK_MAX_ELEMENT(open_mode, ate);

// Now the bitmask can be used:
File open_file(const char *filename, boost::bitmask<open_mode> mode);

auto f = open_file("test.txt", open_mode::out | open_mode::binary | open_mode::trunk);
```
## Intro

`boost::bitmask<T>` class template is an implementation of [BitmaskType](http://en.cppreference.com/w/cpp/concept/BitmaskType) concept.
In short, a bitmask is a finite set of distinct non-zero values (I'd call them "named bits"). It is usually used to implement a set of flags that can be passed to the function.

The implementation has one significant divergences from the concept: a bitmask value and the bitmask itself (i.e. a combination of bitmask values) are two different types. So far I can't see any disadvantage because of this. Also the class provides extra operations in addition to ones required by the concept.

## All the ways to define a bitmask

The bitmask can be defined in the ways described below. Any way of definition can be used with any type of enum: unscoped (classic), scoped (C++11), relaxed and strongly typed ones. The non-intrusive way of definition is specially designed to use exisiting flag enums without any modification.

### Contiguous bitmask values

Intrusive definition:

```cpp
enum class open_mode_1 {
  app = 0x01,
  in =  0x02,
  out = 0x04,
  
  _bitmask_max_element = out
};
BOOST_BITMASK(open_mode_1)
```

Non-intrusive definition:

```cpp
enum class open_mode_2 {
  app = 0x01,
  in =  0x02,
  out = 0x04,
};
BOOST_BITMASK_MAX_ELEMENT(open_mode_2, out)
```

### Noncontiguous bitmask values

Intrusive definition:

```cpp
enum class open_mode_3 {
  app = 0x02,
  in =  0x08,
  out = 0x10,
  
  _bitmask_value_mask = 0x1A // = 0x02 | 0x08 | 0x10
};
BOOST_BITMASK(open_mode_3)
```

Non-intrusive definition:

```cpp
enum class open_mode_4 {
  app = 0x02,
  in =  0x08,
  out = 0x10,
};
BOOST_BITMASK_VALUE_MASK(open_mode_4, 0x1A)
```

## Available operations

There is an overview of operations available for bitmask. Please check [`bitmask.hpp`](include/boost/bitmask.hpp) for whole picture.
Note that you can't instantiate and use `bitmask<X>` prior a bitmask for `X` is defined with one of the `BOOST_BITMASK` macros described above.

All bitmask operations are constexpr (Have I told this already?) and noexcept (OK, not all but most of them).

### Bitwise operations

Bitwise operations `|`, `&`, `^` and `~` are available for both bitmask value and `bitmask<T>` types.
Bitwise assignment operators `|=`, `&=` and `^=` are available for `bitmask<T>` only.

```cpp
enum class flags {
    binary = 0x01,
    app    = 0x02,
    in     = 0x04,
    out    = 0x08,
};
BOOST_BITMASK_MAX_ELEMENT(flags, out);

auto x = flags::binary | flags::app & (~flags::in ^ flags::out);
auto y = ~x ^ flags::binary;
// both x and y are of type bitmask<flags>

y &= flags::binary;
```

### Equality comparison

Operators `==` and `!=` are available to compare two `bitmask<T>`, `bitmask<T>` with its bitmask value `T`, and `bitmask<T>` with its underlying integral type.

```cpp
bitmask<flags> x = flags::binary | flags::app;
bitmask<flags> y = flags::binary | flags::in;

if (flags::binary == x) ...
if (y != flags::binary | flags::app) ...
if (x == y) ...
if (y == 0x12) ...
```
### Operator bool

`bitmask<T>` has an explicit operator bool. One returns true if bitmask is non-zero and false otherwise.

```cpp
bitmask<flags> y;
bitmask<flags> x = flags::binary | flags::app;

if (y) ...                     // false
if (!y) ...                    // true
if (x & flags::binary) ...     // true
if (!(x & flags::binary)) ...  // true
```

### Less than

There is operator `<` defined for `bitmask<T>` so its instances can be ordered.

### `std::hash<bitmask<T>>`

There is an `std::hash` specialization for `bitmask<T>` defined thus one can be used as an unordered container key.

### Bit acccess

Free function `bits()` is available for both bitmask value and `bitmask<T>` types.
`bitmask<T>` also has a member function `bits()`.

```cpp
auto b1 = bits(flags::binary);
auto b2 = bits(flags::binary | flags::in);
auto b3 = (flags::binary | flags::in).bits();
// All b1, b2 and b3 are of type std::underlying_type<flags>::type.
```

## How to build and run tests

It's easy: you only need to install CMake 3.1 or above.

```
cd <bitmask_dir>
mkdir build && cd build
cmake ..
make
make test
```

## Acknowledgement

- LLVM libc++ for inspiring `std::regex_constants` implementation.
- [Jonathan Müller](https://twitter.com/foonathan) for great post https://foonathan.github.io/blog/2016/09/09/cpp14-concepts.html that inspired me to add compile-time tests (yet in progress).

## Tested compilers

- MacOS: Apple Clang 7.3.0
- Linux: G++ 4.8.5 and G++ 5.4.0
- Windows: Visual C++ 2015 (14.0) Update 3.
  *Note that some tests and compile time checks are disabled because VC++ has a limited support for expression SFINAE.*

## License
This is free and unencumbered software released into the **public domain**.

Please check [LICENSE](LICENSE) if you need more details.
