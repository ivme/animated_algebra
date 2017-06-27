#include <numeric>
#include <cmath>
#include <string>
#include "renderer.h"
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

// explicit instantiations
template class renderer<ascii_image>;
