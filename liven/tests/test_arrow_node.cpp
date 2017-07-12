#include "arrow_node.h"
#include <cassert>

using liven::arrow_node;

int main() {
	arrow_node an{arrow_node::direction::up,3};
	assert(an.get_length() == 3);
	assert(an.get_direction() == arrow_node::direction::up);
}