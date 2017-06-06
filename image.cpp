#include "image.h"
#include <typeinfo>

wchar_t &ascii_image::pixel_at(unsigned int x, unsigned int y) {
	return pixels.entry(pixels.rows() - y - 1, x);
}

// layered_image
template <class IMAGE_TYPE>
std::shared_ptr<IMAGE_TYPE> layered_image<IMAGE_TYPE>::flatten() {
	return flatten_and_crop(bounding_rect());
}

template <class IMAGE_TYPE>
void layered_image<IMAGE_TYPE>::insert(p_loc_image p_img) {
	images.push_back(p_img);
	full_image_bounding_rect = union_bounding_rect(full_image_bounding_rect,img_bounding_rect(*p_img));
}

template <class IMAGE_TYPE>
std::shared_ptr<IMAGE_TYPE> layered_image<IMAGE_TYPE>::flatten_and_crop(located<rect,2> cropping_rect) {
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
	return out_ptr;
}

// explicit instantiations
ascii_image ascii_img{};
pixel_range<wchar_t> pr{};
layered_image<ascii_image> l_img{};
template std::shared_ptr<ascii_image> layered_image<ascii_image>::flatten();
template void layered_image<ascii_image>::insert(p_loc_image p_img);