#include "p_rect.h"
#include <stdexcept>

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
