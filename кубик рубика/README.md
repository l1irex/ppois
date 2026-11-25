# Rubik's Cube 3x3x3

A C++ console model of the Rubik's Cube puzzle. The project implements a single
`RubiksCube` class that stores the state of 54 stickers (6 faces of 9 stickers),
rotates faces together with their edge cycles, scrambles the cube with random
moves, checks whether it is solved, and reads/writes states through streams and
text files. It demonstrates dynamic memory management, the Rule of Three,
operator overloading, input validation, and separation of the domain model from
the console interface.

## Requirements

- Visual Studio 2022 with the MSVC v145 toolset (or another C++17 compiler)
- Windows API (`windows.h`) for UTF-8 console output in the demo
- No third-party libraries are required

## Build

Open `кубик рубика.slnx` in Visual Studio 2022, select `x64` and `Debug` or
`Release`, then build the solution (`Ctrl+Shift+B`). From a Developer Command
Prompt the same build can be started with:

```sh
msbuild "кубик рубика.slnx" /p:Configuration=Release /p:Platform=x64
```

This builds:

- `кубик рубика` — an interactive demonstration program (`main.cpp`)
- `RubiksCubeTests` — a native unit-test project included in the solution

## Run the demonstration

Run the `кубик рубика` executable. The program shows a menu with six commands:

```sh
x64\Release\кубик рубика.exe
```

| Command | Action |
|---:|---|
| 1 | Print the current cube state as a net of six faces |
| 2 | Check whether the cube is solved |
| 3 | Scramble the cube with the requested number of random moves |
| 4 | Rotate a face clockwise or counter-clockwise |
| 5 | Load a cube state from a text file |
| 0 | Exit |

Face indices are `0` Up, `1` Down, `2` Front, `3` Back, `4` Left, and `5` Right.
Invalid menu input and unreadable files are reported as `Ошибка: <message>`.

## Run the tests

The native test suite is `RubiksCubeTests/RubiksCubeTests.cpp`. Build the test
project and run the resulting DLL with VSTest:

```sh
msbuild "RubiksCubeTests\RubiksCubeTests.vcxproj" /p:Configuration=Release /p:Platform=x64
vstest.console.exe "x64\Debug\RubiksCubeTests.dll" /Platform:x64
```

If the test project was built in Release, point VSTest to the Release DLL:

```sh
vstest.console.exe "x64\Release\RubiksCubeTests.dll" /Platform:x64
```

On success VSTest reports that every discovered test passed. The tests cover
the default and copied states, sticker access, face rotations, scrambling,
stream operators, and file loading.

## Requirement coverage

| Item | Value |
|---|---:|
| Domain classes | 1 (`RubiksCube`) |
| Sticker slots | 54 (6 faces x 9 stickers) |
| Face indices | 0..5 |
| Color codes | 0..5 |
| Public operations | 12 |
| Unit tests | 14 |
| Custom exceptions | 0 (errors are reported with `bool`, `-1`, or stream failbit) |

The public operations include construction, copy construction, assignment,
destruction, `scramble`, `rotateFace`, `isSolved`, `getColor`, `loadFromFile`,
`operator==`, `operator!=`, and the stream insertion/extraction operators.

## OOP concepts demonstrated

| Concept | Where |
|---|---|
| Encapsulation | Sticker buffer is private; access goes through `getColor` and stream operators |
| Dynamic memory | `new[]` allocates the sticker array; `delete[]` releases it |
| Rule of Three | Copy constructor, copy assignment operator, and destructor all manage the same buffer |
| Operator overloading | `=`, `==`, `!=`, `<<`, and `>>` |
| Input validation | `getColor` bounds check, `rotateFace` face check, stream color-code check |
| Composition | `RubiksCube` owns its sticker buffer |
| Separation of concerns | `main.cpp` handles the UI; `RubiksCube` handles only cube state and behavior |

---

## Class documentation

### RubiksCube

- Private state: `int* colors`, `SIZE = 54`, `FACE_SIZE = 9`, `NUM_FACES = 6`
- Helpers: `rotateFaceData`, `shiftEdgesCycle`, `rotateEdges`, `isValidColor`
- Construction/copy: `RubiksCube()`, `RubiksCube(const RubiksCube&)`,
  `operator=(const RubiksCube&)`, `~RubiksCube()`
- Behavior: `scramble(int moves = 20)`, `rotateFace(int faceIndex, bool clockwise)`,
  `isSolved() const`, `getColor(int index) const`, `loadFromFile(const std::string&)`
- Comparison: `operator==(const RubiksCube&) const`, `operator!=(const RubiksCube&) const`
- Streams: `operator<<(std::ostream&, const RubiksCube&)`,
  `operator>>(std::istream&, RubiksCube&)`

The default constructor builds a solved cube: every sticker on face `i` has
color `i`. A face rotation rotates the 3x3 sticker block and cyclically shifts
the adjacent edge rows. Four clockwise turns of the same face restore the
original state; a clockwise turn followed by a counter-clockwise turn cancels
out. `scramble(n)` applies `n` random face turns and ignores non-positive `n`.

`isSolved()` compares every sticker of a face with its center sticker.
`getColor()` returns `-1` for an index outside `0..53`. `loadFromFile()` reads
54 integer color codes; invalid data sets the stream failbit and leaves the
cube unchanged.

### Face indices

| Index | Face |
|---:|---|
| 0 | Up (Верх) |
| 1 | Down (Низ) |
| 2 | Front (Перед) |
| 3 | Back (Зад) |
| 4 | Left (Лево) |
| 5 | Right (Право) |

### State file format

A state file contains 54 integers separated by spaces. The first 9 values
describe face 0, the next 9 describe face 1, and so on. Each value is a color
code in the range `0..5`.

---

## Project layout

- `кубик рубика/RubiksCube.h` — class declaration and public API
- `кубик рубика/RubiksCube.cpp` — rotation, scramble, validation, stream, and file logic
- `кубик рубика/main.cpp` — interactive demonstration program
- `кубик рубика/Doxyfile` — Doxygen configuration
- `кубик рубика/html/` — generated Doxygen documentation
- `RubiksCubeTests/RubiksCubeTests.cpp` — native unit-test suite
- `кубик рубика.slnx` — solution configuration (x64 and x86)
