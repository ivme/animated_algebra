#include "unique_sequence.h"
#include <cassert>

using namespace liven;

int main() {
	unique_sequence<int> s;
	s.push_back(4);
	assert(s.size() == 1);
	s.push_back(4);
	assert(s.size() == 1);
	s.insert(s.begin(),3);
	assert(s.size() == 2);
	assert(*s.begin() == 3);
	assert(*(++s.begin()) == 4);
}