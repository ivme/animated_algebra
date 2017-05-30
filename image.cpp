#include "image.h"
#include <typeinfo>

wchar_t ascii_image::pixel_at(unsigned int x, unsigned int y) {
	return pixels.entry(pixels.rows() - x, y);
}

void ascii_image::show(viewer &v) const {
	v.show(*this);
}
