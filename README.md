# StringsForMicrocontrollers

A lightweight C++ string utility library focused on constrained environments (especially microcontrollers), while still being usable on desktop C++ compilers for development and testing.

## Why this project exists

Many embedded projects need safer and more expressive string handling than raw `char*`, but cannot always afford full STL usage or dynamic-heavy abstractions.

This project provides:
- a non-owning `string_view` type for efficient read-only operations,
- a fixed-capacity string (`FixedString<N>`) for predictable memory usage,
- a resizable string (`DynamicString`) for cases where growth is needed,
- optional interoperability with STL strings and Arduino `String`.

## Core components

### `string_view`
Non-owning string reference with:
- size tracking (`size()`),
- indexed access (`operator[]`, `at()`),
- comparisons (`==`, `!=`, relational operators or `<=>` on C++20),
- search helpers (`find`, `indexOf`, `startsWith`),
- lightweight printing support.

Best used for read-only parameters to avoid unnecessary copying.

### `string` (base owning type)
Owns a mutable character buffer and provides:
- C-string access (`c_str()`, `data()`),
- concatenation (`concat(...)` overloads),
- replacement (`replace(old, new)`),
- copy and move semantics.

This class is the base for fixed and dynamic capacity variants.

### `FixedString<N>`
Compile-time fixed-capacity string:
- stores data in an internal stack/static buffer,
- never reallocates,
- truncates when operations exceed capacity.

Use this when deterministic memory behavior is required.

### `DynamicString`
Heap-allocated string that can grow:
- starts with a caller-provided initial capacity (minimum enforced),
- auto-resizes during concatenation/replacement,
- useful when input size is variable.

Use this when flexibility is needed and heap usage is acceptable.

## Platform support

The header (`mystring.hpp`) is written to work in:
- standard C++ environments (desktop compilers),
- Arduino builds (`ARDUINO` path with `Serial`-based warnings/printing).

It conditionally enables interoperability with:
- `std::string` / `std::string_view` when available,
- Arduino `String` when compiling for Arduino.

## Repository layout

- `/mystring.hpp` – main library header
- `/test_string.cpp` – primary test suite for library behavior
- `/test.cpp` – additional prototype/test code
- `/main.cpp` – sample/demo-style code

## Quick usage example

```cpp
#include "mystring.hpp"

FixedString<32> name = "Sensor";
name.concat(" Node");

DynamicString message(8);
message = "Temp:";
message.concat(" 24.5C");

if (string_view(message.data()).startsWith("Temp")) {
    // handle message
}
```

## Building and running tests (desktop)

Use any C++ compiler (examples with `g++`):

```bash
g++ -std=c++17 test_string.cpp -o test_string
./test_string
```

You can also compile individual demo files:

```bash
g++ -std=c++17 main.cpp -o demo
./demo
```

## Behavioral notes

- `FixedString<N>` has a hard limit of `N` characters (excluding null terminator).
- Some operations intentionally truncate or fail when capacity is insufficient.
- `DynamicString` grows capacity to accommodate writes.
- Bounds checks in `at()` print warnings and return safe fallback values.

## Current status

This is an actively evolving implementation intended for learning, experimentation, and embedded-oriented utility development. Expect APIs and internals to evolve as the project is refined.
