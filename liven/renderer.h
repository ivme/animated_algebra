#ifndef RENDERER_H
#define RENDERER_H

// uncomment to use unicode box drawing characters
//#define USE_BOX_DRAWING_CHARACTERS

#include "scene.h"
#include "text_node.h"
#include "grid_node.h"

namespace liven {

class length;
class p_rect;

template <class IMAGE_TYPE>
class renderer {
public:
	std::shared_ptr<IMAGE_TYPE> render(const view&);

	virtual double scene_x_coordinate_to_pixels(int scene_coord) const = 0;
	virtual double scene_y_coordinate_to_pixels(int scene_coord) const = 0;
	located<rect,2> scene_rect_to_pixel_rect(located<rect,2> scene_rect);
	virtual std::shared_ptr<IMAGE_TYPE> render(const node&) = 0;
	virtual std::shared_ptr<IMAGE_TYPE> render(const grid_node&) = 0;
	virtual std::shared_ptr<IMAGE_TYPE> render(const text_node&) = 0;
};


class ascii_renderer : public renderer<ascii_image> {
public:
	virtual std::shared_ptr<ascii_image> render(const node&) override {return std::shared_ptr<ascii_image>();}
	virtual std::shared_ptr<ascii_image> render(const grid_node&) override;
	virtual std::shared_ptr<ascii_image> render(const text_node&) override;

	static constexpr int scene_x_coordinates_per_pixel = 1;
	static constexpr int scene_y_coordinates_per_pixel = 2;

	// these could be non-integer values, which can be problematic.
	virtual double scene_x_coordinate_to_pixels(int scene_coord) const override {return (double) scene_coord / (double) scene_x_coordinates_per_pixel;}
	virtual double scene_y_coordinate_to_pixels(int scene_coord) const override {return (double) scene_coord / (double) scene_y_coordinates_per_pixel;}
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

	extern wchar_t u;

	extern wchar_t d;

	extern wchar_t r;

	extern wchar_t l;

	extern wchar_t p;
}

} // liven

#endif