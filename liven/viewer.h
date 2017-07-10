#ifndef VIEWER_H
#define VIEWER_H

#include <iostream>

namespace liven {

template <class PIXEL_TYPE>
class animation;
template <class PIXEL_TYPE>
class image;
class ascii_image;

class viewer {
	friend class image<wchar_t>;
	friend class ascii_image;
public:
	template <class PIXEL_TYPE>
	void present(const animation<PIXEL_TYPE> &a);
	virtual void clear_display() = 0;

private:
	// clears the current image from the display
	// and replaces it with img
	virtual void show(const image<wchar_t> &img)  = 0;
	virtual void show(const ascii_image &img)  = 0;
};

class ascii_viewer : public viewer {
public:
	void clear_display() override;
private:
	bool image_on_display = false; // is an image currently shown on the display?
	unsigned int image_width = 0;
	unsigned int image_height = 0;

	void show(const image<wchar_t> &img)  override {
		/* debug */
		std::cout << "ascii_viewer::_show(const image<wchar_t> &img)" << std::endl;
	}
	void show(const ascii_image &img) override;
};

}

#endif