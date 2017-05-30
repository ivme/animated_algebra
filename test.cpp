// #include "animator.h"
#include "algebra.h"
#include "matrix.h"
#include "image.h"
#include "animation.h"
#include "viewer.h"
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

animation sample_animation() {
	std::shared_ptr<ascii_image> f1 = std::make_shared<ascii_image>(image1());
	std::shared_ptr<ascii_image> f2 = std::make_shared<ascii_image>(image2());
	return animation({f1,f2});
}

void test_algebra() {
	assert(Algebra::is_square_number(49));
	assert(!Algebra::is_square_number(-1));
	assert(!Algebra::is_square_number(5));

	assert(Algebra::discriminant(2,-4,-1) == 24);
	assert(Algebra::discriminant(1,6,9) == 0);
	assert(Algebra::discriminant(-3,1,-5) == -59);

	assert(std::get<0>(Algebra::fdiv_qr(5,3)) == 1);
	assert(std::get<1>(Algebra::fdiv_qr(5,3)) == 2);
	assert(std::get<0>(Algebra::fdiv_qr(-5,3)) == -2);
	assert(std::get<1>(Algebra::fdiv_qr(-5,3)) == 1);
	assert(std::get<0>(Algebra::fdiv_qr(5,-3)) == -2);
	assert(std::get<1>(Algebra::fdiv_qr(5,-3)) == -1);

	assert(std::get<0>(Algebra::gcd_ext(5,3)) == 1); // gcd(5,3) == 1
	assert(std::get<1>(Algebra::gcd_ext(5,3)) == -1); // 1 == (-1)(5) + (2)(3)
	assert(std::get<2>(Algebra::gcd_ext(5,3)) == 2);

	assert(Algebra::gcd(32,12) == 4);

	auto f1 = Algebra::quad_factor(18,75,50);
	assert(std::get<0>(f1) == true);
	assert(std::get<1>(f1) == 6);
	assert(std::get<2>(f1) == 5);
	assert(std::get<3>(f1) == 3);
	assert(std::get<4>(f1) == 10);

	auto f2 = Algebra::quad_factor(1,-1,-6);
	assert(std::get<0>(f2) == true);
	assert(std::get<1>(f2) == 1);
	assert(std::get<2>(f2) == -3);
	assert(std::get<3>(f2) == 1);
	assert(std::get<4>(f2) == 2);

	/* user input required
	int a,b,c;
	std::cout << "a = " << std::endl;
	std::cin >> a;
	std::cout << "b = " << std::endl;
	std::cin >> b;
	std::cout << "c = " << std::endl;
	std::cin >> c;
	std::cout << Algebra::factorization_to_string(Algebra::quad_factor(a,b,c)) << std::endl;
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
	assert(img.pixel_at(2,4) == L'w');
	assert(img.pixel_at(1,2) == L'h');
}

void test_animation() {
	animation a = sample_animation();
}

void test_viewer() {
	ascii_viewer v{};
	image0().show(v);
	sleep();
	v.clear_display();
	animation a = sample_animation();
	a.present(v);
}

int main() {
	test_algebra();
	test_matrix();
	test_image();
	test_animation();
	test_viewer();
}