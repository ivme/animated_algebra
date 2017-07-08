#ifndef AA_CONTROLLER_H
#define AA_CONTROLLER_H

#include <tuple>
#include "algebra.h"
#include "liven/ascii_viewer.h"
#include "liven/scene.h"

#define STD_VIEW_WIDTH 150
#define STD_VIEW_HEIGHT 100	

// animated algebra controller
class aa_controller {
public:
	// launch
	void launch();

	// prompt user for (a,b,c) of ax^2 +bx + c
	std::array<int,3> prompt_for_quadratic_coeffs(bool allow_negatives);
	void animate_quadratic_factorization(std::array<int,3> coeffs);
	void animate_quadratic_factorization(int a, int b, int c);

private:
	liven::scene scene_;
	liven::view view_;
	liven::ascii_viewer viewer_;

	static void display_welcome_message();
	static void display_main_menu();
	static int select_menu_option(void (*display_menu)(), int greatest_menu_option);
};
#endif