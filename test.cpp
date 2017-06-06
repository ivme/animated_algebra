// #include "animator.h"
#include "algebra.h"
#include "matrix.h"
#include "image.h"
#include "animation.h"
#include "viewer.h"
#include "scene.h"
#include "p_rect.h"
#include <cassert>
#include <iostream>
#include <thread>


void sleep() {
	std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000)));
}

ascii_image image0() {
	return ascii_image(matrix<wchar_t>(5,3, L'0'));
}

ascii_image image1() {
	return ascii_image(matrix<wchar_t>(5,3, L'1'));
}

ascii_image image2() {
	return ascii_image(matrix<wchar_t>(5,3, L'2'));
}

animation<wchar_t> sample_animation() {
	std::shared_ptr<ascii_image> f1 = std::make_shared<ascii_image>(image1());
	std::shared_ptr<ascii_image> f2 = std::make_shared<ascii_image>(image2());
	return animation<wchar_t>({f1,f2});
}

void test_algebra() {
	assert(algebra::is_square_number(49));
	assert(!algebra::is_square_number(-1));
	assert(!algebra::is_square_number(5));

	assert(algebra::discriminant(2,-4,-1) == 24);
	assert(algebra::discriminant(1,6,9) == 0);
	assert(algebra::discriminant(-3,1,-5) == -59);

	assert(std::get<0>(algebra::fdiv_qr(5,3)) == 1);
	assert(std::get<1>(algebra::fdiv_qr(5,3)) == 2);
	assert(std::get<0>(algebra::fdiv_qr(-5,3)) == -2);
	assert(std::get<1>(algebra::fdiv_qr(-5,3)) == 1);
	assert(std::get<0>(algebra::fdiv_qr(5,-3)) == -2);
	assert(std::get<1>(algebra::fdiv_qr(5,-3)) == -1);

	assert(std::get<0>(algebra::gcd_ext(5,3)) == 1); // gcd(5,3) == 1
	assert(std::get<1>(algebra::gcd_ext(5,3)) == -1); // 1 == (-1)(5) + (2)(3)
	assert(std::get<2>(algebra::gcd_ext(5,3)) == 2);

	assert(algebra::gcd(32,12) == 4);

	auto f1 = algebra::quad_factor(18,75,50);
	assert(std::get<0>(f1) == true);
	assert(std::get<1>(f1) == 6);
	assert(std::get<2>(f1) == 5);
	assert(std::get<3>(f1) == 3);
	assert(std::get<4>(f1) == 10);

	auto f2 = algebra::quad_factor(1,-1,-6);
	assert(std::get<0>(f2) == true);
	assert(std::get<1>(f2) == 1);
	assert(std::get<2>(f2) == -3);
	assert(std::get<3>(f2) == 1);
	assert(std::get<4>(f2) == 2);

	assert(algebra::product_to_string("a","b") == "a*b");
	assert(algebra::product_to_string("1","b") == "b");
	assert(algebra::product_to_string("a","1") == "a");
	assert(algebra::product_to_string("a","a") == "a^2");
	/* user input required
	int a,b,c;
	std::cout << "a = " << std::endl;
	std::cin >> a;
	std::cout << "b = " << std::endl;
	std::cin >> b;
	std::cout << "c = " << std::endl;
	std::cin >> c;
	std::cout << algebra::factorization_to_string(algebra::quad_factor(a,b,c)) << std::endl;
	*/
}

void test_matrix() {
	matrix<wchar_t> m = matrix<wchar_t>(5,3, L'e');
	m.entry(0,2) = L'w';
	m.entry(2,1) = L'h';
	// std::cout << m << std::endl;
	assert(m.entry(0,0) == L'e');
	assert(m.entry(0,2) == L'w');
	assert(m.entry(2,1) == L'h');
}

void test_image() {
	matrix<wchar_t> m = matrix<wchar_t>(5,3, L'e');
	m.entry(0,2) = L'w';
	m.entry(2,1) = L'h';
	ascii_image img(m);
	assert(img.pixel_at(2,4) == L'w'); // convert row,col to coordinates
	assert(img.pixel_at(1,2) == L'h');

	auto img_ptr = std::make_shared<ascii_image>(img);
	pixel_range<wchar_t> range{img_ptr,0,0,img_ptr->pixel_width(),img_ptr->pixel_height()};
	assert(*(range.begin()) == L'e');
	for (auto it = range.begin(); it != range.end(); ++it) {
		*it = 'n';
	}
	assert(*(++range.begin()) == L'n');
	assert(img_ptr->pixel_at(2,4) == L'n');

	located<ascii_image,3> located_img_1{img,point<3>(0,0,0)};
	located<ascii_image,3> located_img_2{*img_ptr,point<3>(6,2,1)};

	layered_image<ascii_image> layered_img{};
	layered_img.insert(std::make_shared<located<ascii_image,3>>(located_img_1));
	layered_img.insert(std::make_shared<located<ascii_image,3>>(located_img_2));
	rect boundary = layered_img.bounding_rect();
	assert(boundary.width == 9 && boundary.height == 7);
	std::shared_ptr<image<wchar_t>> snapshot = layered_img.flatten();
	assert(snapshot->pixel_width() == 9 && snapshot->pixel_height() == 7);
	assert(snapshot->pixel_at(0,0) == L'e');
	assert(snapshot->pixel_at(1,2) == L'h');
	assert(snapshot->pixel_at(2,4) == L'w');
	assert(snapshot->pixel_at(3,4) == ascii_image::default_pixel);
	assert(snapshot->pixel_at(6,2) == L'n');
	assert(snapshot->pixel_at(8,6) == L'n');
	//ascii_viewer v{};
	//snapshot->show(v);
}

void test_animation() {
	animation<wchar_t> a = sample_animation();
}

void test_viewer() {
	ascii_viewer v{};
	image0().show(v);
	sleep();
	v.clear_display();
	animation<wchar_t> a = sample_animation();
	a.present(v);
}

p_rect unit_p_rect() {
	length one{"1",1};
	return p_rect({one},{one});
}

std::shared_ptr<p_rect> unit_p_rect_ptr() {
	length one{"1",1};
	std::vector<length> v_one = {one};
	return std::make_shared<p_rect>(v_one,v_one);
}

void test_p_rect() {
	std::shared_ptr<image<wchar_t>> img;
	ascii_renderer r{};

	p_rect unit_pr = unit_p_rect();
	img = unit_pr.render(r);
	assert(img->pixel_at(0,0) == bdc::ur);
	assert(img->pixel_at(1,0) == bdc::h);
	assert(img->pixel_at(0,1) == bdc::v);
	assert(img->pixel_at(r.h_pixels_per_unit,0) == bdc::ul);
	assert(img->pixel_at(0,r.v_pixels_per_unit) == bdc::dr);
	assert(img->pixel_at(r.h_pixels_per_unit,r.v_pixels_per_unit) == bdc::dl);
	assert(img->pixel_at(r.h_pixels_per_unit / 2, r.v_pixels_per_unit /2) == '1');

	length one{"1",1};
	length x{"x",2};
	p_rect pr{{x,one,one},{x,x,one}};
	
	img = pr.render(r);
	assert(img->pixel_at(0,0) == bdc::ur);
	assert(img->pixel_at(1,0) == bdc::h);
	assert(img->pixel_at(0,1) == bdc::v);
	//ascii_viewer v{};
	//img->show(v);
}

void test_graphics() {
	using located_rect_2 = located<rect,2>;
	located<rect,2> rect0{rect(3,5),point<2>(-1,-4)};
	located<rect,2> rect1{rect(3,5),point<2>(-1,-4)};
	located<rect,2> rect2{rect(6,7),point<2>(-5,-2)};
	assert(rect0 == rect1);
	assert(rect1 != rect2);
	assert(intersection(rect1,rect2) == located_rect_2(rect(2,3),point<2>(-1,-2)));
	assert(union_bounding_rect(rect1,rect2) == located_rect_2(rect(7,9),point<2>(-5,-4)));
	
	located<rect,3> rect4{rect(0,0),point<3>(0,0,0)};
	located<rect,3> rect5{rect(0,0),point<3>(0,0,1)};
	compare_z cmp_z = compare_z{};
	assert(cmp_z(rect4,rect5));
	assert(!cmp_z(rect5,rect4));
}

void test_scene() {
	auto rect1_ptr = unit_p_rect_ptr();
	auto rect2_ptr = unit_p_rect_ptr();
	auto s = std::make_shared<scene>();
	s->set_root(rect1_ptr);
	rect1_ptr->add_child(rect2_ptr);
	rect2_ptr->set_location(point<3>(ascii_renderer::h_pixels_per_unit + 1,0,1));
	
	view v{};
	v.scn = std::weak_ptr<scene>(s);
	v.rectangle = located<rect,2>(rect(2 * ascii_renderer::h_pixels_per_unit + 2, ascii_renderer::v_pixels_per_unit + 1),point<2>(0,0));
	ascii_renderer r{};
	auto snapshot1 = v.render(r);
	assert(snapshot1->pixel_width() == 2 * ascii_renderer::h_pixels_per_unit + 2);
	assert(snapshot1->pixel_height() == ascii_renderer::v_pixels_per_unit + 1);
	assert(snapshot1->pixel_at(0,0) == bdc::ur);
	assert(snapshot1->pixel_at(ascii_renderer::h_pixels_per_unit,0) == bdc::ul);
	assert(snapshot1->pixel_at(ascii_renderer::h_pixels_per_unit + 1,0) == bdc::ur);
	assert(snapshot1->pixel_at(ascii_renderer::h_pixels_per_unit + 1,1) == bdc::v);
	assert(snapshot1->pixel_at(ascii_renderer::h_pixels_per_unit + 2,1) == ascii_image::default_pixel);
	assert(snapshot1->pixel_at(0,ascii_renderer::v_pixels_per_unit) == bdc::dr);

	rect2_ptr->shift(point<3>(1,1,0));
	auto snapshot2 = v.render(r);
	rect1_ptr->shift(0,-2,0);
	auto snapshot3 = v.render(r);
	rect1_ptr->shift(0,2,0);
	rect2_ptr->shift(-3,0,0);
	auto snapshot4 = v.render(r);
	rect2_ptr->shift(0,0,-2);
	auto snapshot5 = v.render(r);

	// ascii_viewer vwr{};
	// snapshot1->show(vwr);
	// snapshot2->show(vwr);
	// snapshot3->show(vwr);
	// snapshot4->show(vwr);
	// snapshot5->show(vwr);
	
}

int main() {
	test_algebra();
	test_graphics();
	test_matrix();
	test_image();
	test_animation();
	//test_viewer();
	test_p_rect();
	test_scene();
	
}