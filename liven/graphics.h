#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <array>
#include <algorithm>
#include <iostream>

namespace liven {

class node;

enum class dimension : int {x = 0, y = 1};

template<size_t DIM> 
struct point {
	point() : coord(std::array<int,DIM>()) {}

	std::array<int, DIM> coord;

	int operator+=(const point<DIM> &other) {
		for (int i = 0; i < DIM; ++i) {
			coord[i] += other.coord[i];
		} 
	}
	int operator-=(const point<DIM> &other) {
		for (int i = 0; i < DIM; ++i) {
			coord[i] -= other.coord[i];
		} 
	}
	
	bool operator==(const point<DIM> &other) {
		for (int i = 0; i < DIM; ++i) {
			if (coord[i] != other.coord[i]) {return false;}
		}
		return true;
	}
};

template<>
struct point<2> {
	point() : x(0), y(0) {}
	point(int x_, int y_) : x(x_), y(y_) {}
	point(const point<3> &other);// : x(other.x), y(other.y) {}
	
	point<2> &operator+=( const point<2> &other) {
		x += other.x;
		y += other.y;
		return *this;
	}
	point<2> &operator-=(const point<2> &other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}

	bool operator==(const point<2> &other) {return x == other.x && y == other.y;}

	int x;
	int y;
};

template<>
struct point<3> {
	point() : x(0), y(0), z(0) {}
	point(int x_, int y_, int z_) : x(x_), y(y_), z(z_) {}
	point(const point<2> &other);// : x(other.x), y(other.y), z(0) {}

	point<3> &operator+=( const point<3> &other) {
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}
	point<3> &operator-=(const point<3> &other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}
	bool operator==(const point<3> &other) {return x == other.x && y == other.y && z == other.z;}

	int x;
	int y;
	int z;
};

std::ostream &operator<<(std::ostream &os, const point<2> &p);
std::ostream &operator<<(std::ostream &os, const point<3> &p);

template <size_t DIM>
point<DIM> operator+(const point<DIM> &lhs, const point<DIM> &rhs) {
	point<DIM> out = lhs;
	out += rhs;
	return out;
}

template <size_t DIM>
point<DIM> operator-(const point<DIM> &lhs, const point<DIM> &rhs) {
	point<DIM> out = lhs;
	out -= rhs;
	return out;
}

struct rect {
	rect() : width(0), height(0) {}
	rect(int width_, int height_) : width(width_), height(height_) {}
	int width = 0;
	int height = 0;
	bool operator==(const rect &other) const {return width == other.width && height == other.height;}
	bool has_no_area() const {return width == 0 || height == 0;}
};

template <int DIM>
struct located_base {
	located_base() : location(point<DIM>()) {}
	located_base(point<DIM> location_) : location(location_) {}
	point<DIM> location;
};

template <class T, int DIM>
struct located : public T, public located_base<DIM> {
	located() : T(), located_base<DIM>() {}
	located(T t, point<DIM> location_) : located_base<DIM>(location_), T(t) {}
	
	template <int DIM2>
	located(const located<T,DIM2> &other) : located_base<DIM>(other.location), T(other) {}

	bool operator==(const located<T,DIM> &rhs) {return T::operator==(rhs) && this->location == rhs.location;}
	bool operator!=(const located<T,DIM> &rhs) {return !(*this == rhs);}
};


struct compare_z {
	bool operator()(const located_base<3> &a, const located_base<3> &b) {
		return a.location.z < b.location.z;
	}
	bool operator()(const std::shared_ptr<located_base<3>> &a, const std::shared_ptr<located_base<3>> &b) {
		return a->location.z < b->location.z;
	}
	bool operator()(const std::shared_ptr<node> &a, const std::shared_ptr<node> &b);  // defined in scene.cpp
};



// return the intersection of the projections of the inputs onto the xy plane
template <int DIM>
located<rect,2> intersection(const located<rect,DIM> &a,const located<rect,DIM> &b) {
	// [a_left,a_right) and [a_low,a_high) are a half-open intervals
	int a_left = a.location.x;
	int a_right = a.location.x + a.width;
	int a_low = a.location.y;
	int a_high = a.location.y + a.height;

	int b_left = b.location.x;
	int b_right = b.location.x + b.width;
	int b_low = b.location.y;
	int b_high = b.location.y + b.height;

	bool intersect = true;
	located<rect,2> empty_rect{rect(0,0),point<2>(0,0)};
	if (a_left >= b_right || b_left >= a_right || a_low >= b_high || b_low >= a_high) {return empty_rect;}
	
	int out_left = std::max(a_left,b_left);
	int out_right = std::min(a_right,b_right);
	int out_low = std::max(a_low,b_low);
	int out_high = std::min(a_high,b_high);
	return located<rect,2>(rect(out_right - out_left, out_high - out_low),point<2>(out_left,out_low));
}

template <int DIM>
located<rect,2> union_bounding_rect(const located<rect,DIM> &a, const located<rect,DIM> &b) {
	if (a.has_no_area()) {return b;}
	if (b.has_no_area()) {return a;}

	int a_left = a.location.x;
	int a_right = a.location.x + a.width;
	int a_low = a.location.y;
	int a_high = a.location.y + a.height;

	int b_left = b.location.x;
	int b_right = b.location.x + b.width;
	int b_low = b.location.y;
	int b_high = b.location.y + b.height;

	int min_x = std::min(a_left,b_left);
	int max_x = std::max(a_right,b_right);
	int min_y = std::min(a_low,b_low);
	int max_y = std::max(a_high,b_high);

	return located<rect,2>(rect(max_x - min_x, max_y - min_y),point<2>(min_x,min_y));
}

} // liven

#endif