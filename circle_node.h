#ifndef CIRCLE_NODE_H
#define CIRCLE_NODE_H

#include "liven/node.h"

struct circle_node : public liven::node {
	circle_node(int radius_) :
		liven::node(this),
		radius(radius_)
		{}
	int radius;
};

#endif