#ifndef RENDERER_H
#define RENDERER_H

// uncomment to use unicode box drawing characters
//#define USE_BOX_DRAWING_CHARACTERS

#include "scene.h"
#include "text_node.h"

class length;
class p_rect;

template <class IMAGE_TYPE>
class renderer {
public:
	std::shared_ptr<IMAGE_TYPE> render(const view&);

	virtual double scene_x_coordinate_to_pixels(int scene_coord) = 0;
	virtual double scene_y_coordinate_to_pixels(int scene_coord) = 0;
	located<rect,2> scene_rect_to_pixel_rect(located<rect,2> scene_rect);
	std::shared_ptr<IMAGE_TYPE> render(const node&) {
		std::cout << __PRETTY_FUNCTION__ << std::endl;
		return std::shared_ptr<ascii_image>();
	}
};


class ascii_renderer : public renderer<ascii_image> {
public:
	using renderer::render;
	std::shared_ptr<ascii_image> render(const text_node& n);

	static constexpr int scene_x_coordinates_per_pixel = 1;
	static constexpr int scene_y_coordinates_per_pixel = 2;

	// these could be non-integer values.  truncate for now.
	virtual double scene_x_coordinate_to_pixels(int scene_coord) override {return (double) scene_coord / (double) scene_x_coordinates_per_pixel;}
	virtual double scene_y_coordinate_to_pixels(int scene_coord) override {return (double) scene_coord / (double) scene_y_coordinates_per_pixel;}
};

namespace bdc {
	extern wchar_t ur;

	extern wchar_t v;

	extern wchar_t dr;

	extern wchar_t h;

	extern wchar_t dl;

	extern wchar_t ul;

	extern wchar_t dh;

	extern wchar_t uh;

	extern wchar_t vr;

	extern wchar_t vl;

	extern wchar_t vh;
}

#endif