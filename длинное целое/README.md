# Signed Long Integer

A C++ console model of an arbitrary-precision signed decimal integer. The
project implements a single `SignedLong` class that stores decimal digits in a
dynamically allocated array, tracks a separate sign, and supports construction
from `long long` and strings, the Rule of Three, conversion back to `long long`,
arithmetic with both `SignedLong` and `long long`, prefix/postfix increment and
decrement, all relational operators, and stream input/output. It demonstrates
dynamic memory management, operator overloading, validation through exceptions,
and separation of the domain class from the console interface.

## Requirements

- Visual Studio 2022 with the MSVC v145 toolset (or another C++17 compiler)
- Windows API (`Windows.h`) for UTF-8 console output in the demo
- No third-party libraries are required

## Build

Open `длинное целое.slnx` in Visual Studio 2022, select `x64` and `Debug` or
`Release`, then build the solution (`Ctrl+Shift+B`). From a Developer Command
Prompt the same build can be started with:

```sh
msbuild "длинное целое.slnx" /p:Configuration=Release /p:Platform=x64
```

This builds:

- `длинное целое` — an interactive demonstration program (`main.cpp`)
- `SignedLongTests` — a native unit-test project included in the solution

## Run the demonstration

Run the `длинное целое` executable. The program shows a menu with eight commands:

```sh
x64\Release\длинное целое.exe
```

| Command | Action |
|---:|---|
| 1 | Enter values `A` and `B` |
| 2 | Show `A`, `B`, a copy of `A`, and the result of assigning `B` |
| 3 | Arithmetic with another `SignedLong` (`+`, `-`, `*`, `/`, `+=`, `-=`, `*=`, `/=`) |
| 4 | Arithmetic with a built-in `long long` |
| 5 | Prefix and postfix increment/decrement |
| 6 | Compare `A` with `B` and with a `long long` value |
| 7 | Convert `A` to built-in `long long` |
| 0 | Exit |

Values may be longer than `long long`; enter them as ordinary decimal text,
for example `-12345678901234567890`. Invalid input and operations such as
division by zero are reported with an `Ошибка:` message.

## Run the tests

The native test suite is `SignedLongTests/SignedLongTests.cpp`. Build the test
project and run the resulting DLL with VSTest:

```sh
msbuild "SignedLongTests\SignedLongTests.vcxproj" /p:Configuration=Release /p:Platform=x64
vstest.console.exe "x64\Debug\SignedLongTests.dll" /Platform:x64
```

If the test project was built in Release, point VSTest to the Release DLL:

```sh
vstest.console.exe "x64\Release\SignedLongTests.dll" /Platform:x64
```

On success VSTest reports that every discovered test passed. The tests cover
constructors and the Rule of Three, conversion to `long long`, all arithmetic
operations, increment/decrement, comparisons, and stream operators.

## Requirement coverage

| Item | Value |
|---|---:|
| Domain classes | 1 (`SignedLong`) |
| Stored fields | 3 (`digits`, `length`, `sign`) |
| Sign values | `-1`, `0`, `1` |
| Digit base | 10 |
| Public operations | 31 |
| Unit tests | 18 |
| Custom exceptions | 0 (standard `invalid_argument`, `overflow_error`, and `domain_error` are used) |

The public operations include two converting constructors, copy construction,
copy assignment, destruction, unary minus, arithmetic with `SignedLong` and
`long long` in both compound and non-compound forms, four increment/decrement
forms, twelve comparisons, and stream insertion/extraction.

## OOP concepts demonstrated

| Concept | Where |
|---|---|
| Encapsulation | Digits, length, and sign are private; state is accessed through operators |
| Dynamic memory | `new[]` and `delete[]` manage the digit buffer |
| Rule of Three | Copy constructor, copy assignment operator, and destructor |
| Operator overloading | Arithmetic, compound assignment, increment/decrement, comparison, conversion, `<<`, and `>>` |
| Static helpers | `copyDigits`, `compareAbs`, `addAbs`, and `subAbs` |
| Input validation | String constructor and stream extraction reject invalid tokens; conversion detects overflow |
| Error signaling | Exceptions for invalid strings, overflow, and division by zero |
| Separation of concerns | `main.cpp` handles the UI; `SignedLong` handles only number representation and operations |

---

## Class documentation

### SignedLong

- Private state: `int* digits`, `size_t length`, `int sign`
- Helpers: `normalize()`, `copyDigits()`, `compareAbs()`, `addAbs()`, `subAbs()`
- Construction/copy: `SignedLong(long long val = 0)`,
  `explicit SignedLong(const std::string& str)`, `SignedLong(const SignedLong&)`,
  `operator=(const SignedLong&)`, `~SignedLong()`
- Conversion: `explicit operator long long() const`
- Unary: `operator-() const`
- Arithmetic with `SignedLong`: `+`, `-`, `*`, `/` and their compound forms
- Arithmetic with `long long`: `+`, `-`, `*`, `/` and their compound forms
- Increment/decrement: `operator++()`, `operator++(int)`, `operator--()`, `operator--(int)`
- Comparison with `SignedLong` and `long long`: `==`, `!=`, `<`, `<=`, `>`, `>=`
- Streams: `operator<<(std::ostream&, const SignedLong&)`,
  `operator>>(std::istream&, SignedLong&)`

Digits are stored from least significant to most significant. Zero is stored as
one digit with sign `0`; leading zeros are removed by `normalize()`. String
construction accepts optional surrounding whitespace and an optional `+` or
`-` sign; invalid characters or an empty string throw `std::invalid_argument`.

Addition and subtraction handle opposite signs by comparing absolute values.
Multiplication uses grade-school long multiplication. Division uses repeated
subtraction of the divisor from the current prefix and truncates toward zero;
division by zero throws `std::domain_error`. Conversion to `long long` throws
`std::overflow_error` when the value does not fit, including the `LLONG_MIN`
boundary.

### Error handling

| Operation | Error | Result |
|---|---|---|
| `SignedLong(const std::string&)` | empty or malformed string | `std::invalid_argument` |
| `operator long long()` | value outside `long long` range | `std::overflow_error` |
| `operator/` and `operator/=` | zero divisor | `std::domain_error` |
| `operator>>` | malformed token | stream failbit; target is unchanged |

### Sample value file

`длинное целое/signed_long_value.txt` contains `-123` and can be used as a
simple example of decimal text accepted by the string constructor and stream
extraction operator.

---

## Project layout

- `длинное целое/SignedLong.h` — class declaration and public API
- `длинное целое/SignedLong.cpp` — arithmetic, comparison, conversion, and stream logic
- `длинное целое/main.cpp` — interactive demonstration program
- `длинное целое/signed_long_value.txt` — sample decimal value
- `длинное целое/Doxyfile` — Doxygen configuration (root level)
- `длинное целое/html/` and `длинное целое/latex/` — generated documentation
- `SignedLongTests/SignedLongTests.cpp` — native unit-test suite
- `длинное целое.slnx` — solution configuration (x64 and x86)
