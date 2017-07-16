#ifndef NODE_LINEUP
#define NODE_LINEUP

#include "node.h"
#include "unique_sequence.h"
#include <list>

namespace liven {

// a node lineup lines up some subset of its children
// stored in a list (called 'lineup')
// nodes are placed next to each other with no overlap,
// but no space between their bounding_rects
// if dim = dimension::x, nodes a aligned horizontally
// and all lined up nodes have zero y-coordinate in the
// parent coordinate system (that of the lineup_node)
class lineup_node : public node {
public:
	using iterator = unique_sequence<std::shared_ptr<node>>::iterator;
	using const_iterator = unique_sequence<std::shared_ptr<node>>::const_iterator;

	lineup_node(dimension dim_) : node(), dim(dim_) {} // lineup_nodes are not renderable themselves, so call node() with no arguments

	unique_sequence<std::shared_ptr<node>> get_lineup() const {return lineup;};
	void set_lineup(unique_sequence<std::shared_ptr<node>> lineup_);

	iterator begin() {return lineup.begin();}
	iterator end() {return lineup.end();}
	const_iterator cbegin() {return lineup.cbegin();}
	const_iterator cend() {return lineup.cend();}

	void remove(std::shared_ptr<node> n);
	void erase(iterator pos);
	void erase(iterator first, iterator last);
	void insert(unique_sequence<std::shared_ptr<node>>::iterator pos, std::shared_ptr<node> n);
	void splice(iterator pos, std::shared_ptr<lineup_node> other);

	void push_back(std::shared_ptr<node> n);
	void pop_back();

	dimension dim;

private:
	unique_sequence<std::shared_ptr<node>> lineup;
	void update_locations();
};

}
#endif