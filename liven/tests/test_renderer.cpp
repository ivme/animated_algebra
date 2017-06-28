#include "renderer.h"
#include <cassert>
#include <iostream> // debug

int main() {
	ascii_renderer r{};
	grid_node gn1{
		2 * ascii_renderer::scene_x_coordinates_per_pixel,
		2 * ascii_renderer::scene_y_coordinates_per_pixel,
		{0,1 * ascii_renderer::scene_x_coordinates_per_pixel},
		{0,1 * ascii_renderer::scene_y_coordinates_per_pixel}};
	/* should render as
	++
	++
	*/

	auto img1 = gn1.render(r);
	assert(img1->pixel_height() == 2);
	assert(img1->pixel_width() == 2);
	assert(img1->pixel_at(0,0) == bdc::vh);
	assert(img1->pixel_at(0,1) == bdc::vh);
	assert(img1->pixel_at(1,0) == bdc::vh);
	assert(img1->pixel_at(1,1) == bdc::vh);

	int pixel_width2 = 7;
	int pixel_height2 = 5;
	// v_pp == vertical pixel partitions
	// h_pp == horizontal pixel partitions
	std::vector<int> v_pp2 = {0,2,5,6};
	std::vector<int> h_pp2 = {0,3};
	

	// convert to scene coordinates:
	int scene_width2 = pixel_width2 * ascii_renderer::scene_x_coordinates_per_pixel;
	int scene_height2 = pixel_height2 * ascii_renderer::scene_y_coordinates_per_pixel;
	std::vector<int> v_sp2{};
	std::vector<int> h_sp2{};
	for (auto p : v_pp2) {v_sp2.push_back(p * ascii_renderer::scene_x_coordinates_per_pixel);}
	for (auto p : h_pp2) {h_sp2.push_back(p * ascii_renderer::scene_y_coordinates_per_pixel);}


	grid_node gn2{scene_width2,scene_height2,v_sp2,h_sp2};
	/* should render as	
	| |  ||
    +-+--++
    | |  ||
	| |  ||
	+-+--++
	*/

	auto img2 = gn2.render(r);
	assert(img2->pixel_height() == pixel_height2);
	assert(img2->pixel_width() == pixel_width2);
	assert(img2->pixel_at(0,0) == bdc::vh);
	assert(img2->pixel_at(1,0) == bdc::h);
	assert(img2->pixel_at(2,0) == bdc::vh);
	assert(img2->pixel_at(3,0) == bdc::h);
	assert(img2->pixel_at(4,0) == bdc::h);
	assert(img2->pixel_at(5,0) == bdc::vh);
	assert(img2->pixel_at(6,0) == bdc::vh);
	assert(img2->pixel_at(0,1) == bdc::v);
	assert(img2->pixel_at(1,1) == ascii_image::default_pixel);
	assert(img2->pixel_at(2,1) == bdc::v);
	assert(img2->pixel_at(3,1) == ascii_image::default_pixel);
	assert(img2->pixel_at(4,1) == ascii_image::default_pixel);
	assert(img2->pixel_at(5,1) == bdc::v);
	assert(img2->pixel_at(6,1) == bdc::v);
	for (int x = 0; x < 7; ++x) {
		assert(img2->pixel_at(x,3) == img2->pixel_at(x,0));
		for (int y : {2,4}) {
			assert(img2->pixel_at(x,y) == img2->pixel_at(x,1));
		}
	}
	

}