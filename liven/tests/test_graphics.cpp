#include "graphics.h"
#include <cassert>

using namespace liven;

int main() {
	using located_rect_2 = located<rect,2>;
	located<rect,2> rect0{rect(3,5),point<2>(-1,-4)};
	located<rect,2> rect1{rect(3,5),point<2>(-1,-4)};
	located<rect,2> rect2{rect(6,7),point<2>(-5,-2)};
	assert(rect0 == rect1);
	assert(rect1 != rect2);
	assert(intersection(rect1,rect2) == located_rect_2(rect(2,3),point<2>(-1,-2)));
	assert(union_bounding_rect(rect1,rect2) == located_rect_2(rect(7,9),point<2>(-5,-4)));
	
	located<rect,3> rect4{rect(0,0),point<3>(0,0,0)};
	located<rect,3> rect5{rect(0,0),point<3>(0,0,1)};
	compare_z cmp_z = compare_z{};
	assert(cmp_z(rect4,rect5));
	assert(!cmp_z(rect5,rect4));

	assert(point<3>(1,2,3) + point<3>(4,5,6) == point<3>(5,7,9));
	assert(point<3>(1,2,-1) - point<3>(3,5,-7) == point<3>(-2,-3,6));

	point<2> p2{4,5};
	point<3> p3{6,7,8};
	p2.set_coordinate(dimension::x, 1);
	assert(p2.x == 1);
	assert(p2.y == 5);
	assert(p2 == point<2>(1,5));
	p3.set_coordinate(dimension::y, 1);
	assert(p3 == point<3>(6,1,8));

}