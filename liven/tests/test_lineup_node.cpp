#include "lineup_node.h"
#include "text_node.h"
#include <cassert>

using namespace liven;

int main() {
	auto ln = std::make_shared<lineup_node>(dimension::x);
	auto hi = std::make_shared<text_node>("hi");
	auto there = std::make_shared<text_node>("there");
	ln->push_back(hi);
	assert(ln->get_children().size() == 1);
	ln->push_back(there);
	assert(ln->get_children().size() == 2);
	assert(hi->get_location() == point<3>(0,0,0));
	auto x = hi->own_bounding_rect().width; 
	assert(there->get_location() == point<3>(x,0,0));

	auto ln2 = std::make_shared<lineup_node>(dimension::y);
	ln2->splice(ln2->end(),ln);
	auto y = hi->own_bounding_rect().height;
	assert(hi->get_parent().lock() == ln2);
	assert(there->get_parent().lock() == ln2);
	assert(hi->get_location() == point<3>(0,0,0));
	assert(there->get_location() == point<3>(0,y,0));

	ln2->remove(hi);
	assert(ln2->get_children().size() == 1);
	assert(there->get_location() == point<3>(0,0,0));

	auto root = std::make_shared<node>();
	ln2->set_parent(root);
	assert(ln2->get_parent().lock() == root);
	ln2->push_back(hi);
	auto ln3 = ln2->split(--ln2->end());
	assert(ln3->get_parent().lock() == ln2->get_parent().lock());
	assert(ln3->dim == ln2->dim);
	assert(ln3->get_location().get_coordinate(ln3->dim) == ln2->bounding_rect().get_size(ln2->dim));
	assert(*ln3->begin() == hi);
	assert(ln3->get_lineup().size() == 1);
	assert(*ln2->begin() == there);
	assert(ln2->get_lineup().size() == 1);
}