#include "aa_controller.h"
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

	// factors ax^2 + bx + c into (a0x + b0)(a1x + b1)
	std::tuple<bool,int,int,int,int> f = algebra::quad_factor(a,b,c);

	bool is_factorable = std::get<0>(f);
	if (!is_factorable) {
		std::cout << "not factorable." << std::endl;
		return;
	}
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

	


}