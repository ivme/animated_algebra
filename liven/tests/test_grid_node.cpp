#include "grid_node.h"
#include <cassert>

using namespace liven;

int main() {
	grid_node gn{3,5,{0,1,3},{0,5}};
	assert(gn.width == 3);
	assert(gn.height == 5);
}