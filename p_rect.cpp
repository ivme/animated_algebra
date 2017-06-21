#include "p_rect.h"
#include <stdexcept>
#include <typeinfo>
#include <algorithm>
#include <iterator>

bool operator==(const length &lhs, const length &rhs) {return lhs.name == rhs.name && lhs.val == rhs.val;}

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

	for (auto child : parent->get_children()) {
		if (!(typeid(*child)==typeid(p_rect))) {
			throw std::runtime_error("non p_rect child found in p_rect::merge");
		}
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

	for (auto child : parent->get_children()) {
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
		case dimension::y:
		merged_rect = std::make_shared<p_rect>(other_lengths,lengths);
	}
	
	merged_rect->set_location(parent->get_location());
	auto grandparent = parent->get_parent();
	if (auto gp = grandparent.lock()) {
		gp->remove_child(parent);
		merged_rect->set_parent(gp,true);
	}
	
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
