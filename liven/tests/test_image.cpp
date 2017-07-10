#include "image.h"
#include <cassert>

using namespace liven;

int main() {
	matrix<wchar_t> m = matrix<wchar_t>(5,3, L'e');
	m.entry(0,2) = L'w';
	m.entry(2,1) = L'h';
	ascii_image img(m);
	assert(img.pixel_at(2,4) == L'w'); // convert row,col to coordinates
	assert(img.pixel_at(1,2) == L'h');

	auto img_ptr = std::make_shared<ascii_image>(img);
	pixel_range<wchar_t> range{img_ptr,0,0,img_ptr->pixel_width(),img_ptr->pixel_height()};
	assert(*(range.begin()) == L'e');
	for (auto it = range.begin(); it != range.end(); ++it) {
		*it = 'n';
	}
	assert(*(++range.begin()) == L'n');
	assert(img_ptr->pixel_at(2,4) == L'n');

	located<ascii_image,3> located_img_1{img,point<3>(0,0,0)};
	located<ascii_image,3> located_img_2{*img_ptr,point<3>(6,2,1)};

	layered_image<ascii_image> layered_img{};
	layered_img.insert(std::make_shared<located<ascii_image,3>>(located_img_1));
	layered_img.insert(std::make_shared<located<ascii_image,3>>(located_img_2));
	rect boundary = layered_img.bounding_rect();
	assert(boundary.width == 9 && boundary.height == 7);
	std::shared_ptr<image<wchar_t>> snapshot = layered_img.flatten();
	assert(snapshot->pixel_width() == 9 && snapshot->pixel_height() == 7);
	assert(snapshot->pixel_at(0,0) == L'e');
	assert(snapshot->pixel_at(1,2) == L'h');
	assert(snapshot->pixel_at(2,4) == L'w');
	assert(snapshot->pixel_at(3,4) == ascii_image::default_pixel);
	assert(snapshot->pixel_at(6,2) == L'n');
	assert(snapshot->pixel_at(8,6) == L'n');
	//ascii_viewer v{};
	//snapshot->show(v);
}