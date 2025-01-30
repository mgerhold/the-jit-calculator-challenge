# The JIT Calculator Challenge

This is strongly inspired by [The JIT Calculator Challenge](https://ochagavia.nl/blog/the-jit-calculator-challenge/) by [Oscar Chagavia](https://ochagavia.nl/). This program can parse simple arithmetic expressions like `4-(x+2)-y*y+z`. The program generates a JIT-compiled function that can evaluate the expression for given values of `x`, `y`, and `z`.

The resulting assembly is far from optimal, but it's a fun exercise in code generation. For now, it only works on x86-64 Linux (following the System V calling convention). However, the architecture should allow for rather easy porting to other platforms.

This project is by no means complete, but besides demonstrating the principle of JIT compilation, it also shows how to parse expressions using a Pratt parser.
