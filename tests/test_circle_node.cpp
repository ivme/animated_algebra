#include "circle_node.h"
#include "liven/node.h"

using liven::node;

int main() {
	node cn{circle_node({2})};
	cn.render();
}