// #include "animator.h"
#include "algebra.h"
#include "matrix.h"
#include "image.h"
#include "animation.h"
#include "viewer.h"
#include "scene.h"
#include "p_rect.h"
#include "aa_controller.h"
#include "animator.h"
#include "action.h"
#include "quad_factor_animator.h"
//#include "stack_cutter.h"
#include <cassert>
#include <iostream>
#include <thread>
#include <memory>

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

	assert(std::get<0>(algebra::tdiv_qr(5,3)) == 1);
	assert(std::get<1>(algebra::tdiv_qr(5,3)) == 2);
	assert(std::get<0>(algebra::tdiv_qr(-5,3)) == -1);
	assert(std::get<1>(algebra::tdiv_qr(-5,3)) == -2);
	assert(std::get<0>(algebra::tdiv_qr(5,-3)) == -1);
	assert(std::get<1>(algebra::tdiv_qr(5,-3)) == 2);

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
	assert(img->pixel_at(r.h_pixels_per_unit(),0) == bdc::ul);
	assert(img->pixel_at(0,r.v_pixels_per_unit()) == bdc::dr);
	assert(img->pixel_at(r.h_pixels_per_unit(),r.v_pixels_per_unit()) == bdc::dl);
	assert(img->pixel_at(r.h_pixels_per_unit() / 2, r.v_pixels_per_unit() /2) == '1');

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

	assert(point<3>(1,2,3) + point<3>(4,5,6) == point<3>(5,7,9));
	assert(point<3>(1,2,-1) - point<3>(3,5,-7) == point<3>(-2,-3,6));
}

void test_scene() {
	ascii_viewer vwr{};
	ascii_renderer r{};

	auto rect1_ptr = unit_p_rect_ptr();
	auto rect2_ptr = unit_p_rect_ptr();

	view v1{};
	v1.scn = std::shared_ptr<scene>(new scene);
	v1.scn->set_root(rect1_ptr);
	rect1_ptr->add_child(rect2_ptr);
	rect2_ptr->set_location(point<3>(ascii_renderer::h_pixels_per_unit() + 1,0,1));
	
	v1.rectangle = v1.scn->bounding_rect();
	
	auto snapshot1 = v1.render(r);
	assert(snapshot1->pixel_at(0,0) == bdc::ur);
	assert(snapshot1->pixel_at(ascii_renderer::h_pixels_per_unit(),0) == bdc::ul);
	assert(snapshot1->pixel_at(ascii_renderer::h_pixels_per_unit() + 1,0) == bdc::ur);
	assert(snapshot1->pixel_at(ascii_renderer::h_pixels_per_unit() + 1,1) == bdc::v);
	assert(snapshot1->pixel_at(ascii_renderer::h_pixels_per_unit() + 2,1) == ascii_image::default_pixel);
	assert(snapshot1->pixel_at(0,ascii_renderer::v_pixels_per_unit()) == bdc::dr);

	rect2_ptr->shift(point<3>(1,1,0));
	auto snapshot2 = v1.render(r);
	rect1_ptr->shift(0,-2,0);
	auto snapshot3 = v1.render(r);
	rect1_ptr->shift(0,2,0);
	rect2_ptr->shift(-3,0,0);
	auto snapshot4 = v1.render(r);
	rect2_ptr->shift(0,0,-2);
	auto snapshot5 = v1.render(r);
	
	// snapshot1->show(vwr);
	// snapshot2->show(vwr);
	// snapshot3->show(vwr);
	// snapshot4->show(vwr);
	// snapshot5->show(vwr);

	length one{"1",1};
	length x{"x",2};
	auto rect3_ptr = std::make_shared<p_rect>(std::vector<length>({x,x,x,one,one}),std::vector<length>({x,x,one}));
	auto s2 = std::make_shared<scene>();
	s2->set_root(rect3_ptr);
	std::shared_ptr<node> split_node = rect3_ptr->split(dimension::x,{1,4});
	assert(s2->get_root()->get_children().size() == 3);
	assert(s2->nodes().size() == 4);
	assert(s2->get_root() != rect3_ptr);
	assert(s2->get_root() == split_node);
	std::shared_ptr<p_rect> split_child_0 = std::dynamic_pointer_cast<p_rect>(split_node->get_children()[0]);
	std::shared_ptr<p_rect> split_child_1 = std::dynamic_pointer_cast<p_rect>(split_node->get_children()[1]);
	std::shared_ptr<p_rect> split_child_2 = std::dynamic_pointer_cast<p_rect>(split_node->get_children()[2]);
	// child 1
	assert(split_child_0->x_lengths == std::vector<length>({x}));
	assert(split_child_0->y_lengths == rect3_ptr->y_lengths);
	assert(split_child_0->get_location() == point<3>(0,0,0));
	// child 2
	assert(split_child_1->x_lengths == std::vector<length>({x,x,one}));
	assert(split_child_1->y_lengths == rect3_ptr->y_lengths);
	assert(split_child_1->get_location() == point<3>(2 * p_rect::unit_size,0,0));
	// child 3
	assert(split_child_2->x_lengths == std::vector<length>({one}));
	assert(split_child_2->y_lengths == rect3_ptr->y_lengths);
	assert(split_child_2->get_location() == point<3>(7 * p_rect::unit_size,0,0));

	// move children
	split_child_2->shift(8,4);
	split_child_1->shift(4,0);

	auto s2_bound = s2->bounding_rect();
	assert(s2_bound.location.x == 0);
	assert(s2_bound.location.y == 0);

	// view v2{};
	// v2.rectangle = s2->bounding_rect();
	// v2.scn = std::weak_ptr<scene>(s2);
	// auto img = v2.render(r);
	// img->show(vwr);;
}

void test_animator() {
	ascii_viewer vwr{};
	ascii_renderer r{};

	auto unit = unit_p_rect_ptr();
	view v{};
	auto s = std::make_shared<scene>();
	s->set_root(unit);
	v.scn = s;
	animator m{};
	m.v = v;

	auto shift1 = std::make_shared<shift>(unit,2,10,0); // shift unit node 1 right
	shift1->fixed_frame_count = 5;
	shift1->use_frame_count();
	m.set_root_action(shift1);
	vwr.present(*m.animate());

	unit->set_location(0,0,0);
	shift1->node_speed = 5;
	shift1->use_node_speed();
	vwr.present(*m.animate());	
	/*
	auto a = std::make_shared<animation<wchar_t>>();
	a->set_animation_speed(5);
	
	auto unit = unit_p_rect_ptr();
	view v{};
	auto s = std::make_shared<scene>();
	s->set_root(unit);
	v.scn = s;
	animator m{v};

	unit->set_location(20,0,0);	
	m.snapshot();
	assert(a->frame_count() == 1);

	m.use_node_speed();
	m.node_speed = 10;
	m.shift(unit, -20, 0);
	assert(a->frame_count() == 3);
	assert(unit->get_location() == point<3>(0,0,0));
	
	m.use_frame_count();
	m.frame_count = 5;
	m.shift(unit, 2, 12);
	assert(a->frame_count() == 8);
	assert(unit->get_location() == point<3>(2,12,0));

	

	vwr.present(*a);*/

}
/*
void test_stack_cutter() {
	ascii_viewer vwr{};
	ascii_renderer r{};
	length one{"1",1};
	length x{"x",2};
	auto rect_ptr = std::make_shared<p_rect>(std::vector<length>({x}),std::vector<length>({one,one,one,one,one,one}));
	auto s = std::make_shared<scene>();
	s->set_root(rect_ptr);
	view v{};
	v.scn = std::weak_ptr<scene>(s);
	auto bounding_rect = s->bounding_rect();
	v.rectangle = located<rect,3>(rect(STD_VIEW_WIDTH,STD_VIEW_HEIGHT),bounding_rect.location);
	auto a = std::make_shared<animation<wchar_t>>();
	a->set_animation_speed(3);
	a->append_frame(v.render(r));

	stack_cutter c{v,a};
	c.y_spacing = 2;
	c.x_spacing = 4;
	c.cut_and_stack(rect_ptr,3);
	vwr.present(*a);
}
*/
void test_aa_controller() {
	aa_controller ctrl;
	ctrl.animate_quadratic_factorization(6,22,20);
}

void test_quad_factor_animator() {
	// TODO: test edge case where b = 0 and c = 0

	// (2x + 3)(x + 4) = 2x^2 + 11x + 12
	quad_factor_animator qfa{2,11,12};
	qfa.set_up_scene();
	auto scn = qfa.v.scn;
	assert(scn);
	auto root = scn->get_root();
	assert(root->get_children().size() == 3);
/*
#ifdef PRIVACY_OFF
	int var_length_scene_coordinates = quad_factor_animator::default_var_val * p_rect::unit_size;
	assert(qfa.col1->width() == var_length_scene_coordinates);
	assert(qfa.col1->height() == var_length_scene_coordinates * 2);
	assert(qfa.col2->width() == var_length_scene_coordinates);
	assert(qfa.col2->height() == 11 * p_rect::unit_size);
	assert(qfa.col3->width() == p_rect::unit_size);
	assert(qfa.col3->height() == 12 * p_rect::unit_size);
#endif
*/

	// qfa.set_up_actions();

	ascii_viewer vwr{};
	vwr.present(*qfa.animate());

	qfa = quad_factor_animator(4,16,15);
	vwr.present(*qfa.animate());

	qfa = quad_factor_animator(3,23,14);
	vwr.present(*qfa.animate());

}

int main() {
	test_algebra();
	test_graphics();
	test_matrix();
	test_image();
	test_animation();
	// test_viewer();
	test_p_rect();
	test_scene();
	//test_animator();
	//test_aa_controller();
	//test_stack_cutter();
	test_quad_factor_animator();
}