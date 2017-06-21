#include "aa_controller.h"
#include "animation.h"
#include "p_rect.h"
//#include "stack_cutter.h"
#include <iostream>
#include <string>

std::array<int,3> aa_controller::prompt_for_quadratic_coeffs(bool allow_negatives) {
	std::array<int,3> out{};
	std::string vars[] = {"a","b","c"};
	for (int i = 0; i < 3; ++i) {
		std::cout << vars[i] << " = " << std::endl;
		std::cin >> out[i];
		while (!allow_negatives && out[i] < 0) {
			std::cout << "Non-negative values are not permitted." << std::endl;
			std::cout << vars[i] << " = " << std::endl;
			std::cin >> out[i];
		}
	}
	return out;
}

void aa_controller::animate_quadratic_factorization(std::array<int,3> coeffs) {
	int a = std::get<0>(coeffs);
	int b = std::get<1>(coeffs);
	int c = std::get<2>(coeffs);
	animate_quadratic_factorization(a,b,c);
}

void aa_controller::animate_quadratic_factorization(const int a, const int b, const int c) {
	// factors ax^2 + bx + c into (a0x + b0)(a1x + b1)
	std::tuple<bool,int,int,int,int> f = algebra::quad_factor(a,b,c);

	bool is_factorable = std::get<0>(f);
	if (!is_factorable) {
		std::cout << "not factorable." << std::endl;
		return;
	}
	int a0 = std::get<1>(f);
	int b0 = std::get<2>(f);
	int a1 = std::get<3>(f);
	int b1 = std::get<4>(f);

	// set up scene
	// 3 p_rects to start
	// 1st column: 1x by a x's
	// 2nd column: 1x by b 1's
	// 3rd column: 1 by c 1's

	// first split second column into two sub-columns
	// col 2a:  1x wide by a0*b1 1's high
	// col 2b:  1x wide by a1*b0 1's high

	// split first column into a0 columns, each of which is 1x wide and a1 x's high (group 1)
	// split subcolumns 2a and 2b into two groups of columns:
	// group 2a: a0 columns, each of which is 1x wide and b1 1's high
	// group 2b: a1 columns, each of which is 1x wide and b0 1's high
	// split 3rd column into b0 columns, each of which is 1 wide and b1 1's high (group 3)

	// lock the groups together
	// move group 2a on top of group 1
	// rotate group 2b
	// move group 3 on top of group 2b

	/*
	ascii_viewer vwr{};
	auto s = std::make_shared<scene>();
	auto root = std::make_shared<node>();
	s->set_root(root);
	view v{};
	ascii_renderer r{};
	v.rectangle = located<rect,2>(rect(STD_VIEW_WIDTH,STD_VIEW_HEIGHT),point<2>(0,0));
	v.scn = std::weak_ptr<scene>(s);
	auto animation_ = std::make_shared<animation<wchar_t>>();
	animation_->set_animation_speed(3);

	length one{"1",1};
	length x{"x",2};
	std::shared_ptr<p_rect> group1, group2, group3;
	group1 = std::make_shared<p_rect>(std::vector<length>(1,x),std::vector<length>(a,x));
	group2 = std::make_shared<p_rect>(std::vector<length>(1,x),std::vector<length>(b,one));
	group3 = std::make_shared<p_rect>(std::vector<length>(1,one),std::vector<length>(c,one));

	root->add_child(group1);
	root->add_child(group2);
	root->add_child(group3);
	group1->set_location(0,0,0);
	group2->set_location(group1->width() + p_rect::unit_size,0,0);
	group3->set_location(group2->get_location() + point<3>(group2->width() + p_rect::unit_size,0,0));

	animation_->append_frame(v.render(r));

	stack_cutter sc{v,animation_};
	auto group3_p = sc.cut_and_stack(group3,b0);
	group3_p->shift(group2->width() + p_rect::unit_size,0,0);
	auto group2_p = sc.cut_and_stack(group2,std::set<int>({static_cast<int>(a0 * b1)}));
	auto group2_a = group2_p->get_children()[0];
	auto group2_b = group2_p->get_children()[1];
	group2_a->set_parent(root,true);
	group2_b->set_parent(root,true);

	

	vwr.present(*animation_);
	*/
}