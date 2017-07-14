#include "unique_sequence.h"
#include <cassert>

using namespace liven;

int main() {
	unique_sequence<int> s1;
	s1.push_back(4);
	assert(s1.size() == 1);
	s1.push_back(4);
	assert(s1.size() == 1);
	s1.insert(s1.begin(),3);
	assert(s1.size() == 2);
	assert(*s1.begin() == 3);
	assert(*(++s1.begin()) == 4);

	unique_sequence<int> s2;
	s2.push_back(11);
	s2.push_back(12);
	s1.splice(s1.end(),s2);
	assert(s1.size() == 4);
	assert(s2.size() == 0);
}