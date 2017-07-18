#include "lineup_node.h"

using namespace liven;

void lineup_node::set_lineup(unique_sequence<std::shared_ptr<node>> lineup_) {
	for (auto node : lineup) {remove_child(node);}
	lineup = lineup_;
	for (auto node : lineup) {add_child(node);}
	update_locations();
}

void lineup_node::insert(lineup_node::iterator pos,std::shared_ptr<node> n, bool update) {
	lineup.insert(pos,n);
	add_child(n);
	if (update) {update_locations();};
}

void lineup_node::erase(lineup_node::iterator pos, bool update) {
	remove_child(*pos);
	lineup.erase(pos);
	if (update) {update_locations();};
}

void lineup_node::erase(iterator first, iterator last, bool update) {
	for (auto it = first; it != last; ++it) {
		remove_child(*it);
		lineup.erase(it);
	}
	if (update) {update_locations();};
}

void lineup_node::remove(std::shared_ptr<node> n, bool update) {
	auto it = std::find(lineup.begin(),lineup.end(),n);
	if (it != lineup.end()) {
		erase(it,update);
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
		int new_coord = current_location.get_coordinate(dim) + (*it)->bounding_rect().get_size(dim);
		current_location.set_coordinate(dim, new_coord);
		++it;
	}
}

void lineup_node::splice(iterator pos, std::shared_ptr<lineup_node> other, bool update) {
	for (auto item : other->get_lineup()) {
		this->add_child(item);
	}
	lineup.splice(pos, other->lineup);
	if (update) {update_locations();}
}

std::shared_ptr<lineup_node> lineup_node::split(iterator pos) {
	if (pos == end()) {return std::make_shared<lineup_node>(dim);}
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
