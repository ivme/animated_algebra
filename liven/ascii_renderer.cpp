#include "ascii_renderer.h"
#include "image.h"
#include "text_node.h"
#include "grid_node.h"
#include "graphics.h"
#include <numeric>
#include <cmath>
#include <string>
#include <iostream> // debug
#include <cassert>

using namespace liven;

namespace liven {

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

	wchar_t u = u'╵'; // up

	wchar_t d = u'╷'; // down

	wchar_t r = u'╶'; // right

	wchar_t l = u'╴'; // left

	wchar_t p = u'·'; // point
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

	wchar_t u = '\'';

	wchar_t d = u',';

	wchar_t r = u'-';

	wchar_t l = u'-';

	wchar_t p = '.';
#endif
}

}

ascii_image ascii_renderer::render(const text_node& n) {
	std::string text = n.get_text();
	int text_size = text.size();
	auto output = std::make_shared<ascii_image>(rect(text_size,1));
	pixel_range<wchar_t> pixels{output,0,0,text_size,1};
	std::copy(text.begin(),text.end(),pixels.begin());
	return *output;
}

// grid_node rendering

template <class T, class OutputIt>
void copy(T t, OutputIt first, OutputIt last) {
	for (auto it = first; it != last; ++it) {
		*it = t;
	}
}

// a struct with the same properties as a grid_node, but converted to scene units
struct scene_grid {
	scene_grid(const grid_node &gn, const ascii_renderer &r) :
		width(r.scene_x_coordinate_to_pixels(gn.width)),
		height(r.scene_y_coordinate_to_pixels(gn.height)),
		v_partitions(std::vector<int>()),
		h_partitions(std::vector<int>())
		{
			for (int x : gn.v_partitions) {v_partitions.push_back(r.scene_x_coordinate_to_pixels(x));}
			for (int y : gn.h_partitions) {h_partitions.push_back(r.scene_y_coordinate_to_pixels(y));}
		}

	int width;
	int height;
	std::vector<int> v_partitions;
	std::vector<int> h_partitions;
};

void draw_h_lines(const grid_node &gn, std::shared_ptr<ascii_image> img, const ascii_renderer &r) {
	assert(img->pixel_width() > 1);
	for (int y : gn.h_partitions) {
		// draw a horizontal line at y
		int pixel_y = r.scene_y_coordinate_to_pixels(y);
		pixel_range<wchar_t> range{img,0,pixel_y,img->pixel_width(),pixel_y + 1};
		*range.begin() = bdc::r;
		copy(bdc::h,++range.begin(),--range.end());
		*(--range.end()) = bdc::l;
	}
}
	
void draw_v_lines(const grid_node &gn, std::shared_ptr<ascii_image> img, const ascii_renderer &r) {
	assert(img->pixel_height() > 1);
	for (int x : gn.v_partitions) {
		// draw a vertical line at x
		int pixel_x = r.scene_x_coordinate_to_pixels(x);
		pixel_range<wchar_t> range{img,pixel_x,0,pixel_x + 1,img->pixel_height()};
		*range.begin() = bdc::u;
		copy(bdc::v,++range.begin(),--range.end());
		*(--range.end()) = bdc::d;
	}
}

void draw_interior_intersections(const grid_node &gn, std::shared_ptr<ascii_image> img, const ascii_renderer &r) {
	for (int x : gn.v_partitions) {
		int pixel_x = r.scene_x_coordinate_to_pixels(x);
		for (int y : gn.h_partitions) {
			int pixel_y = r.scene_y_coordinate_to_pixels(y);
			img->pixel_at(pixel_x,pixel_y) = bdc::vh;
		}
	}
}

void draw_boundary_intersections(const grid_node &gn, std::shared_ptr<ascii_image> img, const ascii_renderer &r) {
	bool vp_zero = std::find(gn.v_partitions.begin(),gn.v_partitions.end(),0) == gn.v_partitions.end() ? false : true;
	bool vp_width = std::find(gn.v_partitions.begin(),gn.v_partitions.end(),gn.width) == gn.v_partitions.end() ? false : true;
	bool hp_zero = std::find(gn.h_partitions.begin(),gn.h_partitions.end(),0) == gn.v_partitions.end() ? false : true;
	bool hp_height = std::find(gn.h_partitions.begin(),gn.h_partitions.end(),gn.height) == gn.h_partitions.end() ? false : true;
	scene_grid sg(gn,r);
	if (vp_zero) {for (auto y : sg.h_partitions) {img->pixel_at(0,y) = bdc::vr;}}
	if (vp_width) {for (auto y : sg.h_partitions) {img->pixel_at(sg.width,y) = bdc::dh;}}
	if (hp_zero) {for (auto x : sg.v_partitions) {img->pixel_at(x,0) = bdc::uh;}}
	if (hp_height) {for (auto x : sg.v_partitions) {img->pixel_at(x,sg.height) = bdc::dh;}}

	if (vp_zero && hp_zero) {img->pixel_at(0,0) = bdc::ur;}
	if (vp_width && hp_zero) {img->pixel_at(sg.width,0) = bdc::ul;}
	if (vp_zero && hp_height) {img->pixel_at(0,sg.height) = bdc::ul;}
	if (vp_width && hp_height) {img->pixel_at(sg.width,sg.height) = bdc::ul;}
}

ascii_image ascii_renderer::render(const grid_node &gn) {
	if (gn.v_partitions.empty() && gn.h_partitions.empty()) {return ascii_image();}
	if (gn.height == 0 && gn.width == 0) {
		auto pnt = std::make_shared<ascii_image>(rect(1,1));
		pnt->pixel_at(0,0) = bdc::p;
		return *pnt;
	}
	int pixel_width = std::ceil(scene_x_coordinate_to_pixels(gn.width)) + 1;
	int pixel_height = std::ceil(scene_y_coordinate_to_pixels(gn.height)) + 1;
	auto out = std::make_shared<ascii_image>(rect(pixel_width,pixel_height));
	ascii_renderer r{};
	if (gn.height != 0) {draw_v_lines(gn,out,r);}
	if (gn.width != 0) {draw_h_lines(gn,out,r);}
	if (!(gn.height == 0) && !(gn.width == 0)) {
		draw_interior_intersections(gn,out,r);
		draw_boundary_intersections(gn,out,r);
	}
	
	return *out;
}