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

point<2> text_node::get_anchor() {
	auto loc = get_location();
	return point<2>(loc.x + get_offset(h_align),loc.y);
}

void text_node::set_anchor(point<2> new_anchor) {
	point<3> delta = point<3>(new_anchor - get_anchor());
	shift(delta);
}

void text_node::align_left() {
	set_h_align(horizontal_align_type::left);
}

void text_node::align_center() {
	set_h_align(horizontal_align_type::center);
}

void text_node::align_right() {
	set_h_align(horizontal_align_type::right);
}

void text_node::set_h_align(horizontal_align_type align) {
	auto loc = get_location();
	set_location(get_anchor().x - get_offset(align), loc.y, loc.z);
	h_align = align;
}

std::string text_node::get_text() const {
	return text;
}

void text_node::set_text(std::string text_) {
	auto loc = get_location();
	auto current_anchor = get_anchor();
	text = text_;
	set_location(current_anchor.x - get_offset(h_align), loc.y, loc.z);
}

located<rect,2> text_node::own_bounding_rect() const {
	return located<rect,2>(rect(text.size() * char_width, char_height),get_location());
}

