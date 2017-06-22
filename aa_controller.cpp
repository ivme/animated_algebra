#include "aa_controller.h"
#include "animation.h"
#include "p_rect.h"
#include "quad_factor_animator.h"
#include "viewer.h"
#include <iostream>
#include <string>

std::array<int,3> aa_controller::prompt_for_quadratic_coeffs(bool allow_negatives) {
	std::array<int,3> out{};
	std::string vars[] = {"a","b","c"};

	std::cout << "Enter coefficients : ax^2 + bx + c" << std::endl;
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

void aa_controller::animate_quadratic_factorization(int a, int b, int c) {
	auto qfa = quad_factor_animator(a,b,c);
	ascii_viewer vwr{};
	vwr.present(*qfa.animate());
}