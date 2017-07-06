#ifndef ASCII_VIEWER_H
#define ASCII_VIEWER_H

class ascii_viewer : public viewer {
public:
	void clear_display() override;
	void show(const ascii_image &img);
private:
	bool image_on_display = false; // is an image currently shown on the display?
	unsigned int image_width = 0;
	unsigned int image_height = 0;
};

#endif