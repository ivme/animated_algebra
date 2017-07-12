#include "text_node.h"
#include <cassert>
#include <iostream>

using namespace liven;

int main() {
	text_node tn{};
	tn.set_text("hello");
	assert(tn.get_text() == "hello");
	tn.align_center();
	assert(tn.get_location() == point<3>((-5 * tn.char_width) / 2, 0, 0));
	tn.align_left();
	assert(tn.get_location() == point<3>(0,0,0));
	tn.align_right();
	assert(tn.get_location() == point<3>(-5 * tn.char_width, 0, 0));
}