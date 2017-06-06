#include "graphics.h"

point<3>::point(const point<2> &other) : x(other.x), y(other.y), z(0) {}
point<2>::point(const point<3> &other) : x(other.x), y(other.y) {}
