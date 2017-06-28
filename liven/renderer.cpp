#include <numeric>
#include <cmath>
#include <string>
#include "renderer.h"
#include <iostream> // debug
#include <cassert>

namespace bdc {
#ifdef USE_BOX_DRAWING_CHARACTERS
	wchar_t ur = u'└'; // up and right

	wchar_t v = u'│'; // vertical

	wchar_t dr = u'┌'; // down and right

	wchar_t h = u'─'; // horizontal

	wchar_t dl = u'┐'; // down and left

	wchar_t ul = u'┘'; // up and left

	wchar_t dh = u'┬'; // down and horizontal

	wchar_t uh = u'┴'; // up and horizontal

	wchar_t vr = u'├'; // vertical and right

	wchar_t vl = u'┤'; // vertical and left

	wchar_t vh = u'┼'; // vertical and horizontal
#else
	wchar_t ur = '+';

	wchar_t v = '|';

	wchar_t dr = '+';

	wchar_t h = '-';

	wchar_t dl = '+';

	wchar_t ul = '+';

	wchar_t dh = '+';

	wchar_t uh = '+';

	wchar_t vr = '+';

	wchar_t vl = '+';

	wchar_t vh = '+';
#endif
}

template <class IMAGE_TYPE>
std::shared_ptr<IMAGE_TYPE> renderer<IMAGE_TYPE>::render(const view& v) {
	if (!v.scn) {return std::make_shared<located<IMAGE_TYPE,3>>();}
	auto nodes = v.scn->nodes();
	std::shared_ptr<IMAGE_TYPE> p_img;
	std::shared_ptr<located<IMAGE_TYPE,3>> p_located_img;
	layered_image<IMAGE_TYPE> layered_img{};
	
	for (auto node_ptr : nodes) {
		if (node_ptr->is_renderable()) {
			p_img = node_ptr->render(*this);
			auto scene_location = node_ptr->get_scene_location();
			int x = std::floor(scene_x_coordinate_to_pixels(scene_location.x));
			int y = std::floor(scene_y_coordinate_to_pixels(scene_location.y));
			int z = scene_location.z;
			point<3> pixel_location{x,y,z};
			p_located_img = std::make_shared<located<IMAGE_TYPE,3>>(*p_img,pixel_location);
			layered_img.insert(p_located_img);
		}
	}
	return layered_img.flatten_and_crop(scene_rect_to_pixel_rect(v.rectangle));
}

template <class IMAGE_TYPE>
located<rect,2> renderer<IMAGE_TYPE>::scene_rect_to_pixel_rect(located<rect,2> scene_rect) {
	int x = std::floor(scene_x_coordinate_to_pixels(scene_rect.location.x));
	int y = std::floor(scene_y_coordinate_to_pixels(scene_rect.location.y));
	int width = std::ceil(scene_x_coordinate_to_pixels(scene_rect.width));
	int height = std::ceil(scene_y_coordinate_to_pixels(scene_rect.height));
	return located<rect,2>(rect(width,height),point<2>(x,y));
}


std::shared_ptr<ascii_image> ascii_renderer::render(const text_node& n) {
	std::string text = n.get_text();
	int text_size = text.size();
	auto output = std::make_shared<ascii_image>(rect(text_size,1));
	pixel_range<wchar_t> pixels{output,0,0,text_size,1};
	std::copy(text.begin(),text.end(),pixels.begin());
	return output;
}

// grid_node rendering

template <class T, class OutputIt>
void copy(T t, OutputIt first, OutputIt last) {
	for (auto it = first; it != last; ++it) {
		*it = t;
	}
}

void draw_h_lines(const grid_node &gn, std::shared_ptr<ascii_image> img, const ascii_renderer &r) {
	for (int y : gn.h_partitions) {
		// draw a horizontal line at y
		int pixel_y = r.scene_y_coordinate_to_pixels(y);
		pixel_range<wchar_t> range{img,0,pixel_y,img->pixel_width(),pixel_y + 1};
		copy(bdc::h,range.begin(),range.end());
	}
}
	
void draw_v_lines(const grid_node &gn, std::shared_ptr<ascii_image> img, const ascii_renderer &r) {
	for (int x : gn.v_partitions) {
		// draw a vertical line at x
		int pixel_x = r.scene_x_coordinate_to_pixels(x);
		pixel_range<wchar_t> range{img,pixel_x,0,pixel_x + 1,img->pixel_height()};
		copy(bdc::v,range.begin(),range.end());
	}
}

void draw_intersections(const grid_node &gn, std::shared_ptr<ascii_image> img, const ascii_renderer &r) {
	for (int x : gn.v_partitions) {
		int pixel_x = r.scene_x_coordinate_to_pixels(x);
		for (int y : gn.h_partitions) {
			int pixel_y = r.scene_y_coordinate_to_pixels(y);
			img->pixel_at(pixel_x,pixel_y) = bdc::vh;
		}
	}
}

std::shared_ptr<ascii_image> ascii_renderer::render(const grid_node &gn) {
	int pixel_width = std::ceil(scene_x_coordinate_to_pixels(gn.width));
	int pixel_height = std::ceil(scene_y_coordinate_to_pixels(gn.height));
	auto out = std::make_shared<ascii_image>(rect(pixel_width,pixel_height));
	draw_v_lines(gn,out,*this);
	draw_h_lines(gn,out,*this);
	draw_intersections(gn,out,*this);
	return out;
}

// explicit instantiations
template class renderer<ascii_image>;
