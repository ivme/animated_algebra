#ifndef ASCII_RENDERER_H
#define ASCII_RENDERER_H

namespace liven {
class ascii_image;
class grid_node;
class text_node;
class rect_node;

class ascii_renderer {
public:
	using image_type = ascii_image;

	static image_type render(const grid_node&);
	static image_type render(const text_node&);
	static image_type render(const rect_node&);

	static constexpr int scene_x_coordinates_per_pixel = 1;
	static constexpr int scene_y_coordinates_per_pixel = 2;

	// these could be non-integer values, which can be problematic.
	static double scene_x_coordinate_to_pixels(int scene_coord) {return (double) scene_coord / (double) scene_x_coordinates_per_pixel;}
	static double scene_y_coordinate_to_pixels(int scene_coord) {return (double) scene_coord / (double) scene_y_coordinates_per_pixel;}
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

}

#endif
