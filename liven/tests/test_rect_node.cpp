#include "rect_node.h"
#include <cassert>

using namespace liven;

int main() {
	auto rn = rect_node::make(0,0);
	assert(rn->get_children().size() == 1);
	assert(rn->get_children()[0] == rn->get_grid());
	assert(rn->get_width() == 0);
	assert(rn->get_height() == 0);
	assert(rn->get_grid()->width == 0);
	assert(rn->get_grid()->height == 0);
	assert(rn->get_grid()->v_partitions == std::vector<int>({0,0}));
	assert(rn->get_grid()->h_partitions == std::vector<int>({0,0}));
}