#include "text_node.h"

using namespace liven;

int text_node::get_offset(horizontal_align_type align) {
	switch (align) {
		case horizontal_align_type::left:
		return 0;
		case horizontal_align_type::center:
		return (text.size() * char_width) / 2;
		case horizontal_align_type::right:
		return (text.size() * char_width);
	}
}

// n is the node in which this textnode is wrapped
point<2> text_node::get_anchor(const node &n) {
	auto loc = n.get_location();
	return point<2>(loc.x + get_offset(h_align),loc.y);
}

void text_node::set_anchor(node &n, point<2> new_anchor) {
	point<3> delta = point<3>(new_anchor - get_anchor(n));
	n.shift(delta);
}

void text_node::align_left(node &n) {
	set_h_align(n, horizontal_align_type::left);
}

void text_node::align_center(node &n) {
	set_h_align(n, horizontal_align_type::center);
}

void text_node::align_right(node &n) {
	set_h_align(n, horizontal_align_type::right);
}

void text_node::set_h_align(node &n, horizontal_align_type align) {
	auto loc = n.get_location();
	n.set_location(get_anchor(n).x - get_offset(align), loc.y, loc.z);
}

std::string text_node::get_text() const {
	return text;
}

void text_node::set_text(std::string text_) {
	text = text_;
}

