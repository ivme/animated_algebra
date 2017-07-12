#ifndef ARROW_NODE_H
#define ARROW_NODE_H

#include "node.h"

namespace liven {

class arrow_node : public liven::node {
public:
	enum class direction : int {right = 0, left = 1, up = 2, down = 3};

	arrow_node(direction d_, int length_) : node(this), d(d_), length(length_) {}
	arrow_node() : arrow_node(direction::right,1) {}

	void set_direction(direction d_) {d = d_;}
	direction get_direction() const {return d;}
	void set_length(int length_) {length = length_;}
	int get_length() const {return length;}


private:
	direction d;
	int length;
};

}; // liven

#endif