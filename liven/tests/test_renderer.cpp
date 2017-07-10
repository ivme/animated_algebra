#include "renderer.h"
#include <cassert>
#include <iostream> // debug
#include "viewer.h"

using namespace liven;

grid_node grid_node_from_image_units(
		int image_width,
		int image_height,
		std::vector<int> v_image_partitions,
		std::vector<int> h_image_partitions)
{
	int width;
	int height;
	std::vector<int> v_partitions;
	std::vector<int> h_partitions;

	width = image_width * ascii_renderer::scene_x_coordinates_per_pixel;
	height = image_height * ascii_renderer::scene_y_coordinates_per_pixel;
	for (auto p : v_image_partitions) {v_partitions.push_back(p * ascii_renderer::scene_x_coordinates_per_pixel);}
	for (auto p : h_image_partitions) {h_partitions.push_back(p * ascii_renderer::scene_y_coordinates_per_pixel);}

	return grid_node(width, height, v_partitions, h_partitions);
}



int main() {
	ascii_renderer r{};
	/* In our ascii renderings, + represents the lower-left corner of a rect.
	   Say the rect's lower left corner is located at (0,0) in scene coordinates.
	   Also suppose this lower-left corner is drawn using pixel_at(0,0) in an image.
	   Therefore, (0,0) in scene coordinates is actually located at (.5,.5) in image coordinates.
	   If the rect is one pixel unit high, its top left corner will be drawn in the center
	   of the pixel_at(0,1), i.e at (.5,1.5) in image coordinates.
	   If the rect is one pixel unit wide, its lower right corner will be draw in the center
	   of the pixel_at(1,0), i.e. at (1.5,.5) in image coordinates.
	   The spaces between the pairs of image-coordinate points ((.5,0),(.5,1)) and ((.5,1.5),(.5,2))
	   goes unused (this is space above and below the rect).
	   In an ideal world, the bottom left corner of the rect would have image coordinates (0,0)
	   and its top left would have image coordinates (0,1), but this isn't possible with ascii.
	   Therefore the rect "invades" the row of pixels above where it should ideally end, and it also
	   invades the column of pixels to the right of where it sould ideally end.
	   That's ok, though.  Imagine we place another unit rect at (1,0) in scene coordinates.  Its
	   lower left corner will be located in the middle of pixel_at(1,0), that is at image
	   coordinates (1.5,.5) -- exactly where the first rect's lower right corner is located!

	   The net effect is to shift the location of all rects by (.5,.5) in image coordinates.
	   The other effect is that a rect that is 1 image unit wide and 1 image unit high
	   partially occupies 2 pixels horizontally and 2 vertically, because half a pixel goes
	   unused at either end.
	   A rect that is w image units wide and h image units high occupies (w + 1) pixels horizontally
	   and (h + 1) units vertically.
	   
	   Furthermore, in our ascii renderings, 2 vertical scene coordinates correspond to 1 pixel,
	   whereas 1 horizontal scene coordinate corresponds to 1 pixel.  So a rect that is 2 scene
	   coordinates high is one pixel high.  Unfortunately, that means that a rect that is 1 scene
	   coordinate high would be .5 pixels high, which is impossible.  In order to render correctly in ascii,
	   then, all scene coordinate heights must be a multiple of 2.  Consequently, only even-height
	   rect_nodes and grid-nodes are tested in this file.
	*/

	grid_node gn0a{0,0,{},{}};
	/* no partitions --> should render as an empty image */
	auto img0a = gn0a.render(r);
	assert(img0a->pixel_height() == 0);
	assert(img0a->pixel_width() == 0);

	grid_node gn0b{0,0,{0},{0}};
	/* should render as a point
		.
	*/
	auto img0b = gn0b.render(r);
	assert(img0b->pixel_height() == 1);
	assert(img0b->pixel_width() == 1);
	assert(img0b->pixel_at(0,0) == bdc::p);

	grid_node gn0c = grid_node_from_image_units(0,1,{0},{0,1});
	/* the two horizontal partitions each have 0 width
	   and therefore should not render at all.
	   gn0c should render as
	   ,
	   '
	   or, with box drawing characters
	   ╷
	   ╵
	*/
	auto img0c = gn0c.render(r);
	assert(img0c->pixel_width() == 1);
	assert(img0c->pixel_height() == 2);
	assert(img0c->pixel_at(0,0) == bdc::u);
	assert(img0c->pixel_at(0,1) == bdc::d);

	grid_node gn1 = grid_node_from_image_units(1,1,{0,1},{0,1});
	/* should render as
	++
	++

	or, with box-drawing characters
	┌┐
	└┘
	*/
	auto img1 = gn1.render(r);
	assert(img1->pixel_height() == 2);
	assert(img1->pixel_width() == 2);
	assert(img1->pixel_at(0,0) == bdc::ur);
	assert(img1->pixel_at(0,1) == bdc::dr);
	assert(img1->pixel_at(1,0) == bdc::ul);
	assert(img1->pixel_at(1,1) == bdc::dl);


	grid_node gn2 = grid_node_from_image_units(6,4,{0,2,5,6},{0,3});
	/* should render as	

	, ,  ,,
    +-+--++
    | |  ||
	| |  ||
	+-+--++

	or, with box-drawing characters,

	╷ ╷  ╷╷
    ├─┼──┼┤
    │ │  ││
	│ │  ││
	└─┴──┴┘
	*/
	auto img2 = gn2.render(r);

	assert(img2->pixel_width() == 7);
	assert(img2->pixel_height() == 5);
	// row 0
	assert(img2->pixel_at(0,0) == bdc::ur);
	assert(img2->pixel_at(1,0) == bdc::h);
	assert(img2->pixel_at(2,0) == bdc::uh);
	assert(img2->pixel_at(3,0) == bdc::h);
	assert(img2->pixel_at(4,0) == bdc::h);
	assert(img2->pixel_at(5,0) == bdc::uh);
	assert(img2->pixel_at(6,0) == bdc::uh);
	
	// row 1
	assert(img2->pixel_at(0,1) == bdc::v);
	assert(img2->pixel_at(1,1) == ascii_image::default_pixel);
	assert(img2->pixel_at(2,1) == bdc::v);
	assert(img2->pixel_at(3,1) == ascii_image::default_pixel);
	assert(img2->pixel_at(4,1) == ascii_image::default_pixel);
	assert(img2->pixel_at(5,1) == bdc::v);
	assert(img2->pixel_at(6,1) == bdc::v);

	// row 2 should be identical to row 1
	for (int x = 0; x < 7; ++x) {
		assert(img2->pixel_at(x,2) == img2->pixel_at(x,1));
	}

	// row 3
	assert(img2->pixel_at(0,3) == bdc::vr);
	assert(img2->pixel_at(1,3) == bdc::h);
	assert(img2->pixel_at(2,3) == bdc::vh);
	assert(img2->pixel_at(3,3) == bdc::h);
	assert(img2->pixel_at(4,3) == bdc::h);
	assert(img2->pixel_at(5,3) == bdc::vh);
	assert(img2->pixel_at(6,3) == bdc::vh);

	// row 4
	assert(img2->pixel_at(0,4) == bdc::d);
	assert(img2->pixel_at(1,4) == ascii_image::default_pixel);
	assert(img2->pixel_at(2,4) == bdc::d);
	assert(img2->pixel_at(3,4) == ascii_image::default_pixel);
	assert(img2->pixel_at(4,4) == ascii_image::default_pixel);
	assert(img2->pixel_at(5,4) == bdc::d);
	assert(img2->pixel_at(6,4) == bdc::d);
}