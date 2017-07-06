#include "node.h"

using liven::node;
using liven::text_node;
using liven::rect_node;

int main() {
	std::vector<node> nodes{};
	nodes.push_back(node{text_node("hello")});
	nodes.push_back(node{rect_node(1,2)});
	for (auto n : nodes) {n.render();}
}