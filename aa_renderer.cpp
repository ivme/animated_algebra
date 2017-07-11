#include "aa_renderer.h"
#include "p_rect.h"
#include "algebra.h"

using liven::located;
using liven::point;
using liven::ascii_image;
using liven::ascii_renderer;
using liven::pixel_range;

void draw_centered_text(std::string text,point<2> center,std::shared_ptr<ascii_image> img) {
	int left_x = center.x - text.size() / 2;
	int right_x = left_x + text.size();
	pixel_range<wchar_t> range{img,left_x,center.y,right_x,center.y + 1};
	std::copy(text.begin(),text.end(),range.begin());
}

int h_pixels_per_unit = p_rect::unit_size / ascii_renderer::scene_x_coordinates_per_pixel;
int v_pixels_per_unit = p_rect::unit_size / ascii_renderer::scene_y_coordinates_per_pixel;

void draw_labels(const p_rect &pr, std::shared_ptr<ascii_image> img) {
	int left = 0;  // leftmost x-coordinate of current cell
	int bottom = 0;  // bottom y-coordinate of current cell
	for (auto x_len : pr.x_lengths) {
		for (auto y_len : pr.y_lengths) {
			std::string label = algebra::product_to_string(x_len.name, y_len.name);
			int label_y = bottom +(y_len.val * v_pixels_per_unit / 2);
			int label_x = left + (x_len.val * h_pixels_per_unit / 2) - (int) (label.size() / 2);

			pixel_range<wchar_t> range{img,label_x,label_y,static_cast<int>(label_x + label.size()),label_y + 1};
			std::copy(label.begin(),label.end(),range.begin());

			bottom += y_len.val * v_pixels_per_unit;
		}
		left += x_len.val * h_pixels_per_unit;
		bottom = 0;
	}
}

void draw_center_label(const p_rect &pr, std::shared_ptr<ascii_image> img, std::string text) {
	ascii_renderer r{};
	if (text.size() > pr.width() - 1) {
		text.resize(pr.width() - 1, L' ');
	}
	int x_mid = r.scene_x_coordinate_to_pixels(pr.width() / 2);
	int y_mid = r.scene_y_coordinate_to_pixels(pr.height() / 2);
	draw_centered_text(text,point<2>(x_mid,y_mid),img);
}

template<>
aa_renderer::image_type aa_renderer::render<p_rect>(const p_rect &pr) {
	std::shared_ptr<ascii_image> out;
	if (pr.show_interior_lines) {
		out = std::make_shared<ascii_image>(render(pr.get_grid_node()));
	} else {
		auto outer_rect = liven::grid_node(pr.width(),pr.height(),{0,pr.width()},{0,pr.height()});
		out = std::make_shared<ascii_image>(render(outer_rect));
	}
	
	if (pr.show_sub_rect_labels) {
		draw_labels(pr,out);
	}
	if (pr.show_center_factored) {
		draw_center_label(pr,out,pr.get_factored_string());
	}
	if (pr.show_center_expanded) {
		draw_center_label(pr,out,pr.get_expanded_string());
	}
	return *out;
}
