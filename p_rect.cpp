#include "p_rect.h"
#include "algebra.h"
#include <stdexcept>
#include <typeinfo>
#include <algorithm>
#include <iterator>

using liven::scene;
using liven::view;
using liven::viewer;
using liven::node;
using liven::located;
using liven::dimension;
using liven::ascii_image;
using liven::ascii_viewer;
using liven::point;
using liven::rect;
using liven::grid_node;

bool operator==(const length &lhs, const length &rhs) {return !(lhs < rhs) && !(rhs < lhs);}
bool operator< (const length &lhs, const length &rhs) {
	if (lhs.val < rhs.val) {return true;}
	else if (lhs.val > rhs.val) {return false;}
	else {
		// lhs.val == rhs.val
		return lhs.name < rhs.name;
	}
}

located<rect,2> p_rect::own_bounding_rect() const {
	return located<rect,2>(rect(width()+1,height()+1),get_location());
}

int p_rect::width_in_units() const {
	int out = 0;
	for (auto it = x_lengths.begin(); it != x_lengths.end(); ++it) {out += it->val;}
	return out;
}

int p_rect::height_in_units() const {
	int out = 0;
	for (auto it = y_lengths.begin(); it != y_lengths.end(); ++it) {out += it->val;}
	return out;
}

int p_rect::width() const {
	return width_in_units() * unit_size;
}

int p_rect::height() const {
	return height_in_units() * unit_size;
}

std::map<length,int> p_rect::get_length_frequency_map(dimension dim) const {
	std::map<length,int> out{};
	for (auto len : get_lengths(dim)) {++out[len];}
	return out;
}

std::map<std::string,int> p_rect::get_var_coeff_map(dimension dim) const {
	std::map<std::string,int> out{};
	auto lengths = get_lengths(dim);
	for (auto len : lengths) {
		std::string name{};
		if (len.name == "1") {name = "";}
		else (name = len.name);
		++out[name];
	}
	return out;
}

const std::vector<length> &p_rect::get_lengths(dimension dim) const {
	switch (dim) {
		case dimension::x:
		return x_lengths;
		case dimension::y:
		return y_lengths;
	}
}

void p_rect::swap_x_y() {
	x_lengths.swap(y_lengths);
}

std::string p_rect::x_label_text() const {
	return p_rect::label_text(dimension::x);
}

std::string p_rect::y_label_text() const {
	return p_rect::label_text(dimension::y);
}

std::string p_rect::get_factored_string() const {
	return algebra::enclose(x_label_text()) + algebra::enclose(y_label_text());
}

std::string p_rect::get_expanded_string() const {
	return algebra::sum_to_string(algebra::expand(get_var_coeff_map(dimension::x),get_var_coeff_map(dimension::y)));
}

std::string p_rect::label_text(dimension dim) const {
	return algebra::sum_to_string(get_var_coeff_map(dim));
}

void p_rect::set_display_style(display_style_type ds) {
	display_style = ds;
	switch (ds) {
		case display_style_type::empty:
		show_interior_lines = false;
		show_sub_rect_labels = false;
		show_center_factored = false;
		show_center_expanded = false;
		break;
		case display_style_type::labels_and_lines:
		show_interior_lines = true;
		show_sub_rect_labels = true;
		show_center_factored = false;
		show_center_expanded = false;
		break;
		case display_style_type::center_factored:
		show_interior_lines = false;
		show_sub_rect_labels = false;
		show_center_factored = true;
		show_center_expanded = false;
		break;
		case display_style_type::center_expanded:
		show_interior_lines = false;
		show_sub_rect_labels = false;
		show_center_factored = false;
		show_center_expanded = true;
	}
}

p_rect::display_style_type p_rect::get_display_style() {
	return display_style;
}

void p_rect::set_children_display_style(std::shared_ptr<node> n, display_style_type ds) {
	for (auto child : n->get_children()) {
		std::dynamic_pointer_cast<p_rect>(child)->set_display_style(ds);
	}
}

std::shared_ptr<node> p_rect::split(dimension dim,std::set<int> split_points) {
	auto out = std::make_shared<node>();
	std::vector<length> split_lengths;
	std::vector<length> other_lengths;
	switch(dim) {
		case dimension::x:
		split_lengths = x_lengths;
		other_lengths = y_lengths;
		break;
		case dimension::y:
		split_lengths = y_lengths;
		other_lengths = x_lengths;
		break;
	}

	int low = 0;
	int low_units = 0;
	int low_coordinates = 0;
	std::vector<std::vector<length>> out_split_lengths = {};
	std::vector<int> location_offsets = {};
	split_points.insert(split_lengths.size());

	for (auto split_point : split_points) {
		if (split_point < 1) {throw std::out_of_range("split_point less than 1");}
		if (split_point > split_lengths.size()) {throw std::out_of_range("split_point past the end");}
		out_split_lengths.push_back(std::vector<length>(split_lengths.begin() + low,split_lengths.begin() + split_point));
		
		location_offsets.push_back(low_coordinates);
		for (int i = low; i < split_point; ++i) {
			low_units += split_lengths[i].val;
		}
		low_coordinates = low_units * unit_size;
		low = split_point;
	}
	
	std::shared_ptr<p_rect> sub_rect;
	for (int i = 0; i < out_split_lengths.size(); ++i) {
		auto lengths = out_split_lengths[i];
		switch(dim) {
			case dimension::x:
			sub_rect = std::make_shared<p_rect>(lengths,other_lengths);
			sub_rect->set_location(point<3>(location_offsets[i],0,0));
			break;
			case dimension::y:
			sub_rect = std::make_shared<p_rect>(other_lengths,lengths);
			sub_rect->set_location(point<3>(0,location_offsets[i],0));
			break;
		}
		out->add_child(sub_rect);
		sub_rect->set_display_style(this->display_style);
	}
	
	out->set_location(this->get_location());

	auto p = get_parent().lock();
	if (p == nullptr) {
		auto scn_ = get_scene().lock();
		if (scn_) {
			// this node is the root of a scene; replace it with the new node out
			scn_->set_root(out);
		} // TODO: check on possible memory leak from this?
	} else {
		// remove this node from parent and replace it with out.
		p->remove_child(shared_from_this());
		p->add_child(out);
	}
	return out;
}

std::shared_ptr<p_rect> p_rect::merge(std::shared_ptr<node> parent, dimension dim) {
	if (parent->get_children().empty()) {
		throw std::runtime_error("children empty in p_rect::merge");
	}

	auto zero_child = std::dynamic_pointer_cast<p_rect>(parent->get_children()[0]);
	const std::vector<length> &lengths = zero_child->get_lengths(dim);
	std::vector<length> other_lengths{};

	// sort parent->get_children() by appropriate coordinate, x or y,
	// depending on the value of dim
	std::vector<std::shared_ptr<node>> sorted_children = parent->get_children();
	std::sort(sorted_children.begin(),sorted_children.end(),
		[dim](std::shared_ptr<node> a, std::shared_ptr<node> b) {
			switch (dim) {
				case dimension::x:
				return a->get_location().x < b->get_location().x;
				case dimension::y:
				return a->get_location().y < b->get_location().y;
			}
		}
	);

	dimension other_dim = dim == dimension::x ? dimension::y : dimension::x;

	for (auto child : sorted_children) {
		auto p_rect_child = std::dynamic_pointer_cast<p_rect>(child);
		if (p_rect_child->get_lengths(dim) != lengths) {
			throw std::runtime_error("merging p_rects with different lengths");
		}

		// append p_rect_child's other lengths to other_lengths
		auto child_other_lengths = p_rect_child->get_lengths(other_dim);
		std::copy(child_other_lengths.cbegin(), child_other_lengths.cend(),std::back_inserter(other_lengths));
	}

	std::shared_ptr<p_rect> merged_rect;
	switch (dim) {
		case dimension::x:
		merged_rect = std::make_shared<p_rect>(lengths,other_lengths);
		break;
		case dimension::y:
		merged_rect = std::make_shared<p_rect>(other_lengths,lengths);
		break;
	}
	
	merged_rect->set_location(parent->get_location());
	auto grandparent = parent->get_parent();
	if (auto gp = grandparent.lock()) {
		gp->remove_child(parent);
		merged_rect->set_parent(gp,true);
	}
	
	merged_rect->set_display_style(zero_child->display_style);
	return merged_rect;
}

std::set<int> p_rect::get_split_points(dimension dim, unsigned int sub_rect_count) const {
	int partitions;
	switch (dim) {
		case dimension::x:
		partitions = x_lengths.size(); break;
		case dimension::y:
		partitions = y_lengths.size(); break;
	}
	if (sub_rect_count > partitions) {throw std::runtime_error("p_rect has too few partitions for requested number of sub-rects");}
	std::set<int> split_points{};
	unsigned int sub_rect_height = partitions / sub_rect_count;
	if (sub_rect_height * sub_rect_count != partitions) {throw std::runtime_error("Height of p_rect is not a multiple of sub_rect_count");}
	unsigned int current_split = sub_rect_height;
	while (current_split < partitions) {
		split_points.insert(current_split);
		current_split += sub_rect_height;
	}
	return split_points;
}

std::shared_ptr<node> p_rect::split(dimension dim, int sub_rect_count) {
	return split(dim,get_split_points(dim,sub_rect_count));
}

grid_node p_rect::get_grid_node() const {
	std::vector<std::vector<int>> partition_vectors{{0},{0}};
	std::vector<std::vector<length>> length_vectors{x_lengths,y_lengths};

	int position;
	for (int i = 0; i < 2; ++i) {
		position = 0;
		for (auto l : length_vectors[i]) {
			position += l.val * unit_size;
			partition_vectors[i].push_back(position);
		}
	}

	return grid_node{width(),height(),partition_vectors[0], partition_vectors[1]};
}

/********              p_rect actions               ********/
/*
bool flash_display_style::own_act() {
	if (current_frame > end_frame) {return false;}
	if (pause_during == 0) {return false;}
	if (current_frame == change_frame) {
			pr->set_display_style(ds);
			return true;
	}
	if (current_frame == revert_frame) {
		pr->set_display_style(original_ds);
		if (pause_after > 0) {return true;}
		else {return false;} 
	}
	return true;
}*/