# 2D Graphics Editor Mini-Project

An interactive command-line terminal application that acts as a 2D Vector Graphic layout tool utilizing text-based ASCII canvas rendering arrays.

## Features Developed
- **Primitive Drawing Engine:** Programmed functions in pure C to accurately structure Lines, Rectangles, Circles, and Triangles.
- **Dynamic Object Matrix:** Handled inside an item array buffer to easily implement Add, Delete, and Wipe commands.
- **Matrix Buffer:** Uses a global `char` 2D array grid to cache shapes before printing the final layout onto the terminal interface.

## Execution
Compile the `main.c` file using any basic standard C compiler:
```bash
gcc main.c -o graphics_editor -lm
./graphics_editor
