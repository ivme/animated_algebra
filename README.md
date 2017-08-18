Animated algebra is a command-line tool for animating standard algorithms from high school algebra.  The only currently supported algorithm is quadratic polynomial factorization (with positive integer coefficients).

Factorization is the process of rewriting an expression as a product.  Here's a common approach to factoring quadratic expressions:
```
6x² + 17x + 12 = 6x² + 9x + 8x + 12 = 3x(2x + 3) + 4(2x + 3) = (3x + 4)(2x + 3)
```
Animated algebra illustrates this procedure for any small, factorable quadratic expression.  It does so by representing any product of two factors as a rectangle, the length of which is the first factor and the height of which is the second factor.

Animations are generated in ascii format and displayed in the terminal.

## Build
Both [CMake](https://cmake.org/) (the command line version) and [GNU make](https://www.gnu.org/software/make/) are prerequisites for building the project.  CMake is available via package managers such as homebrew (`brew install cmake`).  GNU make is preinstalled on many systems.  If you don't have cmake and GNU make installed, please install them before continuing.

After installing CMake and GNU make, clone this repository locally.  Then, from the root project directory,

```
cmake .
make
```

## Run
*Please maximize your terminal window before running, or the animations will be distorted*.  The terminal window should be at least 150 characters wide and 100 characters high.  Then execute `main`:

```
./main
```

You will be prompted for coefficients.  To keep things in frame, it's best to keep them quite small, say a < 10, b < 20, c < 25.  The set of coefficients must represent a factorable quadratic expression.  One working example is the following:
```
a = 
6
b = 
17
c = 
12
```

