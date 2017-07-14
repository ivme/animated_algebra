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
		int new_coord = current_location.get_coordinate(dim) + (*it)->bounding_rect().get_size(dim);
		current_location.set_coordinate(dim, new_coord);
		++it;
	}
}