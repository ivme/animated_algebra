#include "viewer.h"
#include "image.h"
#include "animation.h"
#include <iostream>
#include <thread>
#include <chrono>

void viewer::present(const animation &a) {
	for (auto frame : a.frames) {
		frame->show(*this);
		std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000 / a.fps)));
	}
}

void ascii_viewer::show(const ascii_image &img) {
	// debug
	// std::cout<< "ascii_viewer::_show(const ascii_image &img)" << std::endl;

	clear_display();
	std::cout << img.pixels;
	std::cout << std::endl; // advance cursor one past end of image
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