#include "arrow_node.h"

using namespace liven;

point<2> arrow_node::get_offset(anchor_style_type anchor_style_) const {
	dimension long_dim;
	dimension other_dim;
	if (direction_is_horizontal()) {long_dim = dimension::x;}
	else {long_dim = dimension::y;}
	int long_dim_offset;
	int other_offset;
	if (anchor_is_at_positive(anchor_style_)) {
		long_dim_offset = length;
	} else {
		long_dim_offset = 0;
	}
	if (long_dim == dimension::x) {
		other_offset = h_thick / 2;
	} else {
		other_offset = v_thick / 2;
	}
	point<2> out;
	out.set_coordinate(long_dim,long_dim_offset);
	out.set_coordinate(other_dim,other_offset);
	return out;
}

point<2> arrow_node::get_anchor() {
	return point<2>(get_location()) + get_offset(anchor_style); 
}

void arrow_node::set_anchor(point<2> new_anchor) {
	point<2> delta = new_anchor - get_anchor();
	shift(delta);
}

void arrow_node::anchor_tail() {
	set_anchor_style(anchor_style_type::tail);
}

void arrow_node::anchor_tip() {
	set_anchor_style(anchor_style_type::tip);
}

arrow_node::anchor_style_type arrow_node::get_anchor_style() {
	return anchor_style;
}

void arrow_node::set_anchor_style(anchor_style_type style) {
	auto loc_z = get_location().z;
	point<2> loc_2d = get_anchor() - get_offset(style);
	set_location(loc_2d.x,loc_2d.y,loc_z);
	anchor_style = style;
}

located<rect,2> arrow_node::own_bounding_rect() const {
	rect rect_;
	if (direction_is_horizontal()) {
		rect_ = rect(length,h_thick);
	} else {
		rect_ = rect(v_thick,length);
	}
	return located<rect,2>(rect_,get_scene_location());
}

bool arrow_node::anchor_is_at_positive(anchor_style_type anchor_style_) const {
	if (d == direction::right || d == direction::up) {
		return anchor_style_ == anchor_style_type::tip;
	} else {
		return anchor_style_ == anchor_style_type::tail;
	}
}

bool arrow_node::direction_is_horizontal() const {
	if (d == direction::right || d == direction::left) {
		return true;
	} else {
		return false;
	}
}