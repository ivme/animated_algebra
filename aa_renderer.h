#ifndef AA_RENDERER_H
#define AA_RENDERER_H

#include "liven/ascii_renderer.h"
#include "liven/image.h"

class p_rect;

class aa_renderer {
public:
	using image_type = liven::ascii_renderer::image_type;
	static double scene_x_coordinate_to_pixels(int scene_coord) {
		return liven::ascii_renderer::scene_x_coordinate_to_pixels(scene_coord);
	}
	static double scene_y_coordinate_to_pixels(int scene_coord) {
		return liven::ascii_renderer::scene_y_coordinate_to_pixels(scene_coord);
	}
	template <class T>
	static image_type render(const T&t) {return liven::ascii_renderer::render(t);}
	static image_type render(const p_rect &pr);
};

#endif