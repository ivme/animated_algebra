#include "scene.h"
#include <cassert>

using namespace liven;

int main() {
	auto node1 = std::make_shared<node>();
	auto node2 = std::make_shared<node>();

	view v1{};
	v1.scn = std::shared_ptr<scene>(new scene);
	v1.scn->set_root(node1);
	assert(v1.scn->get_root() == node1);
	node1->add_child(node2);
	assert(node1->get_children().size() == 1);
	node2->set_location(point<3>(1,2,3));
	assert(node2->get_location() == point<3>(1,2,3));
	node2->shift(point<3>(1,1,0));
	assert(node2->get_location() == point<3>(2,3,3));
}