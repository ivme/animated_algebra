#ifndef VIEWER_H
#define VIEWER_H

#include <iostream>

class animation;
class image;
class ascii_image;

class viewer {
	friend class image;
	friend class ascii_image;
public:
	void present(const animation &a);
	virtual void clear_display() = 0;

private:
	// clears the current image from the display
	// and replaces it with img
	virtual void show(const image &img)  = 0;
	virtual void show(const ascii_image &img)  = 0;
};

class ascii_viewer : public viewer {
public:
	void clear_display() override;
private:
	bool image_on_display = false; // is an image currently shown on the display?
	unsigned int image_width = 0;
	unsigned int image_height = 0;

	void show(const image &img)  override {/* debug */std::cout << "ascii_viewer::_show(const image &img)" << std::endl;}
	void show(const ascii_image &img) override;
};

#endif