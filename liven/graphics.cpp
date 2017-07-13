#include "graphics.h"

namespace liven {

std::ostream &operator<<(std::ostream &os, const point<2> &p) {os << "(" << p.x << "," << p.y << ")"; return os;}
std::ostream &operator<<(std::ostream &os, const point<3> &p) {os << "(" << p.x << "," << p.y << "," << p.z << ")"; return os;}

point<3>::point(const point<2> &other) : x(other.x), y(other.y), z(0) {}
point<2>::point(const point<3> &other) : x(other.x), y(other.y) {}

template<class POINT>
int get_coordinate_(POINT &pnt,dimension dim) {
	switch (dim) {
		case dimension::x:
		return pnt.x;
		case dimension::y:
		return pnt.y;
	}
}

template<class POINT>
void set_coordinate_(POINT &pnt,dimension dim,int value) {
	switch (dim) {
		case dimension::x:
		pnt.x = value;
		break;
		case dimension::y:
		pnt.y = value;
		break;
	}
}

// explicit instantiations
template int get_coordinate_<point<3>>(point<3> &, dimension);
template void set_coordinate_<point<3>>(point<3> &,dimension,int);
template int get_coordinate_<point<2>>(point<2> &, dimension);
template void set_coordinate_<point<2>>(point<2> &,dimension,int);

int point<3>::get_coordinate(dimension dim) const {
	return get_coordinate_(*this,dim);
}

void point<3>::set_coordinate(dimension dim, int value) {
	return set_coordinate_(*this,dim,value);
}

int point<2>::get_coordinate(dimension dim) const {
	return get_coordinate_(*this,dim);
}

void point<2>::set_coordinate(dimension dim, int value) {
	return set_coordinate_(*this,dim,value);
}

// rect
int rect::get_size(dimension dim) {
	switch (dim) {
		case dimension::x:
		return width;
		case dimension::y:
		return height;
	}
}

} // liven
