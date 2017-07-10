#ifndef VIEWER_H
#define VIEWER_H

#include <iostream>
#include <thread>
#include <chrono>

namespace liven {

template <class IMAGE_TYPE>
class animation;
class ascii_image;

template<class IMAGE_TYPE>
class viewer {
	friend IMAGE_TYPE;
public:
	void present(const animation<IMAGE_TYPE> &a) {
		for (auto frame : a.frames) {
			show(frame);
			std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000 / a.fps)));
		}
	}
	virtual void clear_display() = 0;

private:
	// clears the current image from the display
	// and replaces it with img
	virtual void show(const IMAGE_TYPE &img)  = 0;
};

class ascii_viewer : public viewer<ascii_image> {
public:
	void clear_display() override;
private:
	bool image_on_display = false; // is an image currently shown on the display?
	unsigned int image_width = 0;
	unsigned int image_height = 0;

	void show(const ascii_image &img) override;
};

}

#endif