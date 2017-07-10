#ifndef IMAGE_H
#define IMAGE_H

#include "matrix.h"
#include "viewer.h"
#include "graphics.h"
#include <set>
#include <iostream> // debug

namespace liven {

template <class PIXEL_TYPE>
class image {
public:
	using pixel_type = PIXEL_TYPE;

	image() {};
	image(rect pixel_dimensions) {}
	image(matrix<PIXEL_TYPE> pixels_) {}

	virtual PIXEL_TYPE &pixel_at(unsigned int x, unsigned int y) = 0; // (0,0) = lower left of image

	virtual int pixel_height() const = 0;
	virtual int pixel_width() const = 0;
};

template <class PIXEL_TYPE>
class pixel_range {
	// iterates left to right, then increments y
	class iterator : public std::iterator<std::random_access_iterator_tag,
											 PIXEL_TYPE,
											 ptrdiff_t,
											 PIXEL_TYPE*,
											 PIXEL_TYPE&> {
	public:
		iterator(pixel_range<PIXEL_TYPE> range_) : range(range_), current_x(range.low_x), current_y(range.low_y) {}
		iterator(pixel_range<PIXEL_TYPE> range_, int current_x_, int current_y_) :
				range(range_), current_x(current_x_), current_y(current_y_) {}
		
		bool operator==(const pixel_range<PIXEL_TYPE>::iterator &other) {
			return this->range == other.range && \
				this->current_x == other.current_x && \
				this->current_y == other.current_y;
		}
		bool operator!=(const pixel_range<PIXEL_TYPE>::iterator &other) {
			return !(*this == other);
		}

		// prefix increment
		iterator &operator++() {
			if (current_x < range.high_x - 1) {current_x++;}
			else {current_x = range.low_x; current_y++;}
			return (*this);
		}
		// prefix decrement
		iterator &operator--() {
			if (current_x == range.low_x) {current_x = range.high_x - 1; --current_y;}
			else {--current_x;}
			return (*this);
		}
		PIXEL_TYPE &operator*() const {return range.img->pixel_at(current_x,current_y);}
	//private:
		pixel_range range;
		int current_x;
		int current_y;
	};

public:
	pixel_range() : img(nullptr) {}
	pixel_range(std::shared_ptr<image<PIXEL_TYPE>> img_, int low_x_ = 0, int low_y_ = 0, int high_x_ = 0, int high_y_ = 0) :
				img(img_), low_x(low_x_), low_y(low_y_), high_x(high_x_), high_y(high_y_) {
					if (high_x > img->pixel_width()) {throw std::runtime_error("high_x larger than image's width");}
					if (high_y > img->pixel_height()) {throw std::runtime_error("high_y larger than image's height");}
				}
	pixel_range(std::shared_ptr<image<PIXEL_TYPE>> img_, located<rect,2> r) : img(img_), low_x(r.location.x), low_y(r.location.y),
																			  high_x(r.location.x + r.width), high_y(r.location.y + r.height) {}


	bool operator==(const pixel_range<PIXEL_TYPE> &other) {
		return this->img == other.img && \
			this->low_x == other.low_x && \
			this->low_y == other.low_y && \
			this->high_x == other.high_x && \
			this->high_y == other.high_y;
	}
	bool operator!=(const pixel_range<PIXEL_TYPE> &other) {
		return !(*this == other);
	}
	iterator begin() {return iterator(*this);}
	iterator end() {return iterator(*this,low_x,high_y);}

private:	
	std::shared_ptr<image<PIXEL_TYPE>> img;

	// includes pixels at coordinates (x,y) such that
	// low_x <= x < high_x
	// low_y <= y < high_y
	int low_x = 0;
	int low_y = 0;
	int high_x = 0;
	int high_y = 0;
};


class ascii_image : public image<wchar_t> {
	friend class ascii_viewer;

public:
	ascii_image() : pixels(matrix<wchar_t>(0,0)) {}
	ascii_image(rect pixel_dimensions) : pixels(matrix<wchar_t>(pixel_dimensions.height, pixel_dimensions.width,default_pixel)) {	}
	ascii_image(matrix<wchar_t> pixels_) : pixels(pixels_) {}

	inline wchar_t &pixel_at(unsigned int x, unsigned int y) override {
		return pixels.entry(pixels.rows() - y - 1, x);
	} // (0,0) = lower left of image
	wchar_t cpixel_at(unsigned int x, unsigned int y) const {return pixels.centry(pixels.rows() - y - 1, x);}
	
	int pixel_height() const override {return pixels.rows();}
	int pixel_width() const override {return pixels.cols();}

	static constexpr wchar_t default_pixel = L' ';
private:
	matrix<wchar_t> pixels;
};

template <class IMAGE_TYPE, int DIM>
located<rect,2> img_bounding_rect(const located<IMAGE_TYPE,DIM> &img) {
	return located<rect,2>(rect(img.pixel_width(),img.pixel_height()),img.location);
}

template<class IMAGE_TYPE>
class layered_image {
	typedef typename std::shared_ptr<located<IMAGE_TYPE,3>> p_loc_image;

public:
	layered_image() : images() {}

	inline IMAGE_TYPE flatten() {
		return flatten_and_crop(bounding_rect());
	}

	inline void insert(p_loc_image p_img) {
		images.push_back(p_img);
		full_image_bounding_rect = union_bounding_rect(full_image_bounding_rect,img_bounding_rect(*p_img));
	}

	IMAGE_TYPE flatten_and_crop(located<rect,2> cropping_rect) {
		// iterate through images from background to foreground
		// if the image intersects with the cropping rect
		// copy pixels from intersection onto output
		std::shared_ptr<IMAGE_TYPE> out_ptr = std::make_shared<IMAGE_TYPE>(cropping_rect);
		std::sort(images.begin(),images.end(),compare_z());
		for (auto img_ptr : images) {
			located<rect,2> intersection_ = intersection(img_bounding_rect(*img_ptr),cropping_rect);
			if (intersection_.width == 0 || intersection_.height == 0) {
				continue;
			} else {
				// call each image a child and the layered image the parent.
				// if child.location = (h,k) in parent's coordinate system, and
				// intersection_.location = (p,q) in parent's coordinate system,
				// then the location of the intersection in the child's coordinate 
				// system is (p - h, q - k)
				located<rect,2> intersection_in_img_coordinates = intersection_;
				intersection_in_img_coordinates.location.x -= img_ptr->location.x;
				intersection_in_img_coordinates.location.y -= img_ptr->location.y;

				located<rect,2> intersection_in_crop_coordinates = intersection_;
				intersection_in_crop_coordinates.location.x -= cropping_rect.location.x;
				intersection_in_crop_coordinates.location.y -= cropping_rect.location.y;

				auto src_range = pixel_range<typename IMAGE_TYPE::pixel_type>(img_ptr, intersection_in_img_coordinates);
				auto dest_range = pixel_range<typename IMAGE_TYPE::pixel_type>(out_ptr, intersection_in_crop_coordinates);
				std::copy(src_range.begin(),src_range.end(),dest_range.begin());
			}
		}
		return *out_ptr;
	}
	located<rect,2> bounding_rect() {
		return full_image_bounding_rect;
	}

private:
	located<rect,2> full_image_bounding_rect;
	std::vector<p_loc_image> images;
};



} // liven

#endif