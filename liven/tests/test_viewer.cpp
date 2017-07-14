#include "viewer.h"
#include "animation.h"
#include "image.h"
#include <cassert>

using namespace liven;

int main() {
	unsigned int n = 58;
	unsigned int m = 100;
	matrix<wchar_t> m1{n,m, L'e'};
	ascii_image img1(m1);
	matrix<wchar_t> m2{n,m, L'f'};
	ascii_image img2(m2);
	ascii_viewer vwr{};
	animation<ascii_image> a{{m1,m2}};
	a.set_animation_speed(100.0);
	vwr.present(a);
}