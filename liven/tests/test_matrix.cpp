#include "matrix.h"
#include <cassert>

int main() {
	matrix<wchar_t> m = matrix<wchar_t>(5,3, L'e');
	m.entry(0,2) = L'w';
	m.entry(2,1) = L'h';
	assert(m.entry(0,0) == L'e');
	assert(m.entry(0,2) == L'w');
	assert(m.entry(2,1) == L'h');
}