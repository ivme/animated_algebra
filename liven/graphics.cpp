#include "graphics.h"

using namespace liven;

std::ostream &operator<<(std::ostream &os, const point<2> &p) {os << "(" << p.x << "," << p.y << ")"; return os;}
std::ostream &operator<<(std::ostream &os, const point<3> &p) {os << "(" << p.x << "," << p.y << "," << p.z << ")"; return os;}

point<3>::point(const point<2> &other) : x(other.x), y(other.y), z(0) {}
point<2>::point(const point<3> &other) : x(other.x), y(other.y) {}
