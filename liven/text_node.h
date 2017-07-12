#ifndef TEXT_NODE_H
#define TEXT_NODE_H

#include "scene.h"
#include <string>

namespace liven {

class text_node : public node {
public:
	text_node() : node(this) {
		align_center();
		set_anchor(point<2>(0,0));
	}
	enum horizontal_align_type : int {left = -1, center = 0, right = 1};
	// anchor differs from location.
	// location is always the lower left corner of a node

	// if the text is...		the anchor is...
	// left-aligned				the bottom left
	// center-aligned			the bottom center
	// right-aligned			the bottom right
	point<2> get_anchor();
	void set_anchor(point<2> new_anchor);

	// the width and height of a single character
	// measured in scene coordinates
	// char_width should be a multiple of
	// ascii_renderer::scene_x_coordinates_per_pixel
	// char_height should be a multiple of
	// ascii_renderer::scene_y_coordinates_per_pixel
	static constexpr int char_width = 1;
	static constexpr int char_height = 2;
	int width() const;  // width in scene coordinates
	int height() const;  // height in scene coordinates

	void align_left();
	void align_right();
	void align_center();
	void set_h_align(horizontal_align_type align);

	std::string get_text() const;
	void set_text(std::string text_);

private:
	int get_offset(horizontal_align_type align); // return anchor's offset relative to location
	horizontal_align_type h_align = horizontal_align_type::center;
	std::string text;
};

} // liven

#endif