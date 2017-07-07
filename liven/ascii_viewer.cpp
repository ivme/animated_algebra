#include "ascii_viewer.h"

using namespace liven;

void ascii_viewer::show(const ascii_image &img) {
	// debug
	// std::cout<< "ascii_viewer::_show(const ascii_image &img)" << std::endl;
	// std::cout << "pixel_at(0,0) == " << std::to_string(img.cpixel_at(0,0)) << std::endl;

	clear_display();
	std::wcout << img.pixels;
	std::wcout << std::endl; // advance cursor one past end of image
	image_on_display = true;
	image_width = img.pixels.cols();
	image_height = img.pixels.rows();
}

void erase_line(std::ostream &os) {
	os << "\33[2K";
}

void move_cursor_up(std::ostream &os) {
	os <<  "\033[A";
}

void ascii_viewer::clear_display() {
	if (image_on_display) {
		for (int i = 0; i < image_height; ++i) {
			move_cursor_up(std::cout);
			erase_line(std::cout);
		}
	}
	// return cursor to first col
	std::cout << "\r";
	std::cout.flush();
	image_on_display = false;
	image_width = 0;
	image_height = 0;
}