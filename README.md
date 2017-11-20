# bitmask
A generic implementation of the [BitmaskType](http://en.cppreference.com/w/cpp/concept/BitmaskType) C++ concept.

The library is a tiny single header without any dependencies except the standard library. And yes, it's pure C++11 and constexpr.

To start using it just download [the latest version of `bitmask.hpp`](include/bitmask/bitmask.hpp) and put it somewhere in your project. (Check for other options [below](#how-to-use-bitmask-library-in-your-project)).

## Warm up example

```cpp
#include <bitmask/bitmask.hpp>

// 1. Define possible flags
enum class open_mode {
    binary = 0x01,
    app    = 0x02,
    in     = 0x04,
    out    = 0x08,
    trunc  = 0x10,
    ate    = 0x20,
    
    // 2. Define max bitmask value
    _bitmask_max_element = ate
};

// 3. Enable bitmask features for the enum
BITMASK_DEFINE(open_mode);

// 4. Now you can use defined bitmask:
File open_file(const char *filename, bitmask::bitmask<open_mode> mode);

auto f = open_file("test.txt", open_mode::out | open_mode::binary | open_mode::trunk);
```
## Intro

`bitmask::bitmask<T>` class template is an implementation of [BitmaskType](http://en.cppreference.com/w/cpp/concept/BitmaskType) concept.
In short, a bitmask is a finite set of distinct non-zero constant values (you can think of them as "named bits"). It is usually used to implement a set of flags that can be passed to the function.

The implementation has one significant divergences from the concept: a bitmask value and the bitmask itself (i.e. a combination of bitmask values) are two different types.
So far I can't see any disadvantage because of this. Also the class provides extra operations in addition to ones required by the concept.

Unlike `std::bitset<N>`, `bitmask::bitmask<T>` is "tied" to enum `T` and `bitmask<T>` ensures the type and value safety in respect to `T` as following:

1) Only values of `T` (or convertible to) can be used to set bitmask bits:
```cpp
bitmask::bitmask<open_mode> mode = read_flags::optimized; // Compilation error
```
2) Bit operations (binary ones complement in particular) keep the bitmask value in the domain of the bitmask.
```cpp
// Non-contigious bitmask values (details are below)
enum class open_mode: unsigned char {
    binary = 0x01,
    out    = 0x02,
    app    = 0x08,
    
    _bitmask_value_mask = 0x0B // = 0x01 | 0x02 | 0x08
};

BITMASK_DEFINE(open_mode);

auto open_mode_flags = ~open_mode::app | open_mode::out;
    // open_mode_flags set to 0x03 (i.e. open_mode::binary | open_mode::out) rather than 0xF7 which would be
    // outside of the domain of bitmask<open_mode>
```

`bitmask::bitmask<T>` is simple to use thanks to implicit conversion from `T` to `bitmask<T>` and overloaded operators provided (more details are below).

`bitmask<T>` is the same size as `T` itself. It has a single field of type `std::underlying_type<T>::type`.

## All the ways to define a bitmask

The bitmask can be defined in the ways described below.
Any way of definition can be used with any kind of [enumeration](http://en.cppreference.com/w/cpp/language/enum): unscoped, scoped, relaxed and strongly typed ones.
The non-intrusive way of bitmask definition is specially designed to be used with an exisiting enum type when its definition may not be altered.

### Contiguous bitmask values

If bitmask values are contigious bits that starts from the first bit i.e. `0x1`, `0x2`, `0x4`, `0x8` etc then defining the bitmask is as simple as specifying
its max value (let's call it *max element*).

Intrusive definition:

```cpp
enum class open_mode_1 {
  app = 0x01,
  in =  0x02,
  out = 0x04,
  
  // Specify maximum bitmask value
  _bitmask_max_element = out
};

// Enable bitmask features for the enum
BITMASK_DEFINE(open_mode_1)
```

Non-intrusive definition:

```cpp
// Note that enum definition has nothing special to support bitmask thus any pre-existing enum type can be used
enum class open_mode_2 {
  app = 0x01,
  in =  0x02,
  out = 0x04,
};

// Specify maximum bitmask value and enable bitmask features for the enum
BITMASK_DEFINE_MAX_ELEMENT(open_mode_2, out)
```

### Noncontiguous bitmask values

If bitmask values are non-contigious bits then you have to provide a bit-mask of all possible values.

Intrusive definition:

```cpp
enum class open_mode_3 {
  app = 0x02,
  in =  0x08,
  out = 0x10,
  
  // Specify bitmask values mask
  _bitmask_value_mask = 0x1A // = 0x02 | 0x08 | 0x10
};

// Enable bitmask features for the enum
BITMASK_DEFINE(open_mode_3)
```

Non-intrusive definition:

```cpp
// Note that enum definition has nothing special to support bitmask thus any pre-existing enum type can be used
enum class open_mode_4 {
  app = 0x02,
  in =  0x08,
  out = 0x10,
};

// Specify bitmask values mask and enable bitmask features for the enum
BITMASK_DEFINE_VALUE_MASK(open_mode_4, 0x1A)  // 0x1A == 0x02 | 0x08 | 0x10
```

### Non-distinct enum values

Note that `bitmask` doesn't require that all enum values are distinct bits. The enum can contain bit combinations as well.
It works fine as long as `bitmask` is defined properly (i.e. all distinct bits are provided).

```cpp
// Note that enum definition has nothing special to support bitmask thus any pre-existing enum type can be used
enum open_mode {
  open_mode_app = 0x01,
  open_mode_in =  0x02,
  open_mode_out = 0x04,
  open_mode_binary = 0x08,
  open_mode_binary_out = open_mode_binary | open_mode_out // Not a distinct bit but a combination of other bits
};

// Note that we use `open_mode_binary` rather than `open_mode_binary_out` because
// former is the maximum distinct bit (i.e. a bitmask value) while latter is not.
BITMASK_DEFINE_MAX_ELEMENT(open_mode, open_mode_binary)

auto f = open_file("test.txt", open_mode_binary_out | open_mode_app);
```

## Available operations

There is an overview of operations available for bitmask. Please check [`bitmask.hpp`](include/bitmask/bitmask.hpp) for all the details.
Note that you can't instantiate and use `bitmask<T>` prior defining a bitmask for `T` with one of the `BITMASK_DEFINE...` macros described above.

All bitmask operations are `constexpr` (Have I told this already?) and most of them are `noexcept`.

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
BITMASK_DEFINE_MAX_ELEMENT(flags, out);

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

It's easy: you only need to install CMake 3.1 and download the library sources.

```
git clone https://github.com/oliora/bitmask.git
cd bitmask
mkdir build
cd build
cmake ..
make
make test
```

## How to use Bitmask library in your project

The simplest way is to download [the lastest version of `bitmask.hpp`](include/bitmask/bitmask.hpp) and place into your project source tree, preferable under `bitmask` directory.

If you want to keep the library separated, you can build and install the library:

```
git clone https://github.com/oliora/bitmask.git
cd bitmask
mkdir build
cd build
cmake .. [-DCMAKE_INSTALL_PREFIX=<prefix>]
make
make install
```

When installed, the library can be simply used in any CMake project as following:

```
find_package(bitmask)
add_executable(<your_app> ...)
target_link_libraries(<your_app> bitmask)
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
The library released under [Boost Software License v1.0](http://www.boost.org/LICENSE_1_0.txt).
