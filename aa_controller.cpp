#include "aa_controller.h"
#include "animation.h"
#include "p_rect.h"
#include "quad_factor_animator.h"
#include "viewer.h"
#include <iostream>
#include <string>

using liven::scene;
using liven::view;
using liven::viewer;
using liven::node;
using liven::located;
using liven::dimension;
using liven::ascii_image;
using liven::ascii_viewer;
using liven::renderer;

void aa_controller::launch() {
	display_welcome_message();
	int user_selection = select_menu_option(&display_main_menu,2);
	switch (user_selection) {
		case 1:
		animate_quadratic_factorization(prompt_for_quadratic_coeffs(false));
		std::cout << "Bye." << std::endl;
		break;
		case 2:
		std::cout << "Bye." << std::endl;
		break;
		default:
		std::cout << "Bye." << std::endl;
		break;
	}
}

void aa_controller::display_welcome_message() {
	std::cout << "Welcome to Animated Algebra." << std::endl;
}

void aa_controller::display_main_menu() {
	std::cout << "Please select:" << std::endl;
	std::cout << "1: animate quadratic factorization" << std::endl;
	std::cout << "2: quit" << std::endl;
	std::cout << ">> ";
}

int aa_controller::select_menu_option(void (*display_menu)(), int greatest_menu_option) {
	display_menu();
	int user_selection;
	std::cin >> user_selection;
	while (user_selection < 1 || user_selection > greatest_menu_option) {
		std::cout << "Invalid selection." << std::endl;
		display_menu();
		std::cin >> user_selection;
	}
	return user_selection;
}

std::array<int,3> aa_controller::prompt_for_quadratic_coeffs(bool allow_negatives) {
	std::array<int,3> out{};
	std::string vars[] = {"a","b","c"};

	std::cout << "Enter coefficients : ax^2 + bx + c" << std::endl;
	for (int i = 0; i < 3; ++i) {
		std::cout << vars[i] << " = " << std::endl;
		std::cin >> out[i];
		while (!allow_negatives && out[i] <= 0) {
			std::cout << "Only positive values are permitted.  Please enter a new value." << std::endl;
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