#include "lineup_node.h"

using namespace liven;

void lineup_node::set_lineup(unique_sequence<std::shared_ptr<node>> lineup_) {
	for (auto node : lineup) {remove_child(node);}
	lineup = lineup_;
	for (auto node : lineup) {add_child(node);}
	update_locations();
}

void lineup_node::insert(lineup_node::iterator pos,std::shared_ptr<node> n) {
	lineup.insert(pos,n);
	add_child(n);
	update_locations();
}

void lineup_node::erase(lineup_node::iterator pos) {
	remove_child(*pos);
	lineup.erase(pos);
	update_locations();
}

void lineup_node::erase(iterator first, iterator last) {
	for (auto it = first; it != last; ++it) {
		remove_child(*it);
		lineup.erase(it);
	}
	update_locations();
}

void lineup_node::remove(std::shared_ptr<node> n) {
	auto it = std::find(lineup.begin(),lineup.end(),n);
	if (it != lineup.end()) {
		erase(it);
		remove_child(*it);
	}
}

void lineup_node::push_back(std::shared_ptr<node> n) {
	insert(end(),n);
}

void lineup_node::pop_back() {
	lineup.pop_back();
}

void lineup_node::update_locations() {
	auto it = lineup.begin();
	auto end = lineup.end();
	point<3> current_location = point<3>(0,0,0);
	while (it != end) {
		(*it)->set_location(current_location);
		int new_coord = current_location.get_coordinate(dim) + (*it)->own_bounding_rect().get_size(dim);
		current_location.set_coordinate(dim, new_coord);
		++it;
	}
}

void lineup_node::splice(iterator pos, std::shared_ptr<lineup_node> other) {
	for (auto item : other->get_lineup()) {
		this->add_child(item);
	}
	lineup.splice(pos, other->lineup);
	update_locations();
}

std::shared_ptr<lineup_node> lineup_node::split(iterator pos) {
	update_locations();
	point<3> loc = (*pos)->get_location();
	auto out = std::make_shared<lineup_node>(dim);
	auto out_seq = unique_sequence<std::shared_ptr<node>>{};
	std::copy(pos,lineup.end(),std::back_inserter(out_seq));
	lineup.erase(pos,lineup.end());
	out->set_parent(this->get_parent());
	out->set_location(this->get_location()+loc);
	out->set_lineup(out_seq);
	return out;
}

std::shared_ptr<lineup_node> lineup_node::split(std::shared_ptr<node> n) {
	return this->split(std::find(begin(),end(),n));
}
