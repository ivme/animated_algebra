#include <numeric>
#include <string>
#include "p_rect.h"
#include "algebra.h"
#include <iostream> // debug

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
	auto scn = v.scn.lock();
	auto nodes = scn->nodes();
	std::shared_ptr<IMAGE_TYPE> p_img;
	std::shared_ptr<located<IMAGE_TYPE,3>> p_located_img;
	layered_image<IMAGE_TYPE> layered_img{};

	for (auto node_ptr : nodes) {
		p_img = node_ptr->render(*this);
		auto scene_location = node_ptr->get_scene_location();
		p_located_img = std::make_shared<located<IMAGE_TYPE,3>>(*p_img,scene_location);
		layered_img.insert(p_located_img);
	}

	return layered_img.flatten_and_crop(v.rectangle);
}

rect ascii_renderer::pixel_dimensions(const p_rect &pr) {
	int width_in_units = pr.width();
	int height_in_units = pr.height();
	if (width_in_units == 0 || height_in_units == 0) {
		return rect(0,0);
	}
	return rect(h_pixels_per_unit * width_in_units + 1, v_pixels_per_unit * height_in_units + 1);
}

// the x pixel coordinates of all vertical partitions, including left and right boundaries
std::vector<int> x_partitions(const p_rect &pr) {
	std::vector<int> output{};
	if (ascii_renderer::pixel_dimensions(pr).width == 0) {
		return output;
	}
	int x = 0;
	output.push_back(x);
	for (length l : pr.x_lengths) {
		x += l.val * ascii_renderer::h_pixels_per_unit;
		output.push_back(x);
	}
	return output;
}

// the y pixel coordinates of all horizontal partitions, including top and bottom boundaries
std::vector<int> y_partitions(const p_rect &pr) {
	std::vector<int> output{};
	if (ascii_renderer::pixel_dimensions(pr).width == 0) {
		return output;
	}
	int y = 0;
	output.push_back(y);
	for (length l : pr.y_lengths) {
		y += l.val * ascii_renderer::v_pixels_per_unit;
		output.push_back(y);
	}
	return output;
}

template <class T, class OutputIt>
void copy(T t, OutputIt first, OutputIt last) {
	for (auto it = first; it != last; ++it) {
		*it = t;
	}
}

void draw_h_lines(const p_rect &pr, std::shared_ptr<ascii_image> img) {
	rect dim = ascii_renderer::pixel_dimensions(pr);
	for (int y : y_partitions(pr)) {
		// draw a horizontal line at y
		pixel_range<wchar_t> range{img,0,y,dim.width,y + 1};
		copy(bdc::h,range.begin(),range.end());
	}
}
	
void draw_v_lines(const p_rect &pr, std::shared_ptr<ascii_image> img) {
	rect dim = ascii_renderer::pixel_dimensions(pr);
	for (int x : x_partitions(pr)) {
		// draw a vertical line at x
		pixel_range<wchar_t> range{img,x,0,x + 1,dim.height};
		copy(bdc::v,range.begin(),range.end());
	}
}

void draw_interior_intersections(const p_rect &pr, std::shared_ptr<ascii_image> img) {
	for (int x : x_partitions(pr)) {
		for (int y : y_partitions(pr)) {
			img->pixel_at(x,y) = bdc::vh;
		}
	}
}

void draw_top_and_bottom_intersections(const p_rect &pr, std::shared_ptr<ascii_image> img) {
	rect dim = ascii_renderer::pixel_dimensions(pr);
	for (int x : x_partitions(pr)) {
		img->pixel_at(x,0) = bdc::uh;
		img->pixel_at(x,dim.height - 1) = bdc::dh;
	} 
}

void draw_left_and_right_intersections(const p_rect &pr, std::shared_ptr<ascii_image> img) {
	rect dim = ascii_renderer::pixel_dimensions(pr);
	for (int y : y_partitions(pr)) {
		img->pixel_at(0, y) = bdc::vr;
		img->pixel_at(dim.width - 1, y) = bdc::vl;
	}
}

void draw_corners(const p_rect &pr, std::shared_ptr<ascii_image> img) {
	rect dim = ascii_renderer::pixel_dimensions(pr);
	img->pixel_at(0,0) = bdc::ur;
	img->pixel_at(0,dim.height - 1) = bdc::dr;
	img->pixel_at(dim.width - 1, 0) = bdc::ul;
	img->pixel_at(dim.width - 1, dim.height - 1) = bdc::dl;
}

void draw_labels(const p_rect &pr, std::shared_ptr<ascii_image> img) {
	int left = 0;  // leftmost x-coordinate of current cell
	int bottom = 0;  // bottom y-coordinate of current cell
	for (auto x_len : pr.x_lengths) {
		for (auto y_len : pr.y_lengths) {
			std::string label = Algebra::product_to_string(x_len.name, y_len.name);
			int label_y = bottom +(y_len.val * ascii_renderer::v_pixels_per_unit / 2);
			int label_x = left + (x_len.val * ascii_renderer::h_pixels_per_unit / 2) - (int) (label.size() / 2);

			pixel_range<wchar_t> range{img,label_x,label_y,static_cast<int>(label_x + label.size()),label_y + 1};
			std::copy(label.begin(),label.end(),range.begin());

			bottom += y_len.val * ascii_renderer::v_pixels_per_unit;
		}
		left += x_len.val * ascii_renderer::h_pixels_per_unit;
		bottom = 0;
	}
}

std::shared_ptr<ascii_image> ascii_renderer::render(const p_rect &pr) {
	rect dim = pixel_dimensions(pr);
	auto output = std::make_shared<ascii_image>(dim);
	if (dim.width == 0) {
		return output;
	}

	draw_h_lines(pr,output);
	draw_v_lines(pr,output);
	draw_interior_intersections(pr,output);
	draw_left_and_right_intersections(pr,output);
	draw_top_and_bottom_intersections(pr,output);
	draw_corners(pr,output);
	draw_labels(pr,output);
	return output;
}

// explicit instantiations
template class renderer<ascii_image>;
