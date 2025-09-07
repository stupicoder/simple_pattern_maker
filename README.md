# Basic Anti-Aliasing

This project is a simple command-line application that demonstrates various anti-aliasing techniques and pattern generation.

## Features

*   **Anti-Aliasing**: Supports SSAA, MSAA, and FXAA.
*   **Patterns**: Generates UV, checkerboard, circle, and Voronoi patterns.
*   **Output**: Exports images in the PPM format.

## How to Build

To build the project, you need to have CMake installed.

1.  **Configure the project**:
    ```bash
    cmake -S . -B build
    ```

2.  **Build the project**:
    ```bash
    cmake --build build
    ```

## How to Run

The executable will be located in the `build` directory.

```bash
./build/basicAA.exe [width] [height] [aa_type] [aa_level] [pattern_type] [output_file]
```

### Options

*   `width`: Output image width (default: 1920)
*   `height`: Output image height (default: 1080)
*   `aa_type`: `ssaa`, `msaa`, `fxaa` (default: `msaa`)
*   `aa_level`: 1-8 (default: 2)
*   `pattern_type`: `uv`, `checkerboard`, `circle`, `voronoi` (default: `voronoi`)
*   `output_file`: Optional output file name