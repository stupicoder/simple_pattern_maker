# AGENTS.md

This file provides guidance to agents when working with code in this repository.

## Build/Lint/Test Commands

*   **Configure**: `cmake -S . -B build`
*   **Build**: `cmake --build build`
*   **Run**: `./build/basicAA.exe [width] [height] [aa_type] [aa_level] [pattern_type] [output_file]`

## Code Style

*   **Naming Conventions**:
    *   `Enums` and `enum` classes are prefixed with `E`.
    *   `Structs` and `classes` are in `camelCase`.
    *   `Variables` are in `camelCase`.
    *   `Constants` are in `UPPER_CASE`.
*   **Formatting**:
    *   Braces are on the same line as the statement.
    *   Indentation is 4 spaces.

## Custom Utilities/Patterns

*   The project uses custom math utilities defined in [`sources/math.h`](sources/math.h:1) for vector and matrix operations.
*   Pattern generation and anti-aliasing logic are implemented in [`sources/pattern.h`](sources/pattern.h:1).
*   Image export is handled by the `ExportPPM` function in [`sources/ppm.h`](sources/ppm.h:1).

## Testing

*   There is no dedicated testing framework. Testing is done manually by running the executable with different arguments and inspecting the output images.