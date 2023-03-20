# Visual Maze Solver

Solves mazes using image processing techniques. This program expects its input
to be images of mazes with the following properties:

- corridors are pure white and have an odd width
- width and height of corridors are equal
- walls are pure black and 1 px wide
- the entire maze is surrounded by a wall
- there is exactly one "entrance"/beginning and it is defined to be in the top left corner
- there is exactly one "exit"/end and it is defined to be in the bottom right corner

It produces images that have the solution drawn in as a red line.

# Compilation and usage

This code requires `libvips` to be installed on the system. It can be compiled and run as follows:

```sh
clang++ src/main.cpp -o visual-maze-solver --std=c++20 -Wall -O3 `pkg-config vips-cpp --cflags --libs`
# read `blank_maze.png` and write the solution to `solved.png`
./visual-maze-solver blank_maze.png solved.png
```

# License

While this program was written on my own time, it was inspired by code I wrote as part of my activity at Müller BBM VibroAkustik Systeme (MBBM VAS). MBBM VAS has agreed to me publishing this code, however, to avoid issues regarding the intellectual property, this work **MUST NOT** be reproduced without written permission.
