#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"
#include "viewer.h"

class image {
public:
	virtual void show(viewer &v) const = 0;
};

class ascii_image : public image {
	friend class ascii_viewer;

public:
	ascii_image(matrix<wchar_t> pixels) : pixels(pixels) {}
	wchar_t pixel_at(unsigned int x, unsigned int y); // (0,0) = lower left of image

	virtual void show(viewer &v) const;

private:
	matrix<wchar_t> pixels;
};

#endif