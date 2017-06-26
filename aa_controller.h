#ifndef AA_CONTROLLER_H
#define AA_CONTROLLER_H

#include <tuple>
#include "algebra.h"
#include "viewer.h"
#include "scene.h"

#define STD_VIEW_WIDTH 150
#define STD_VIEW_HEIGHT 100	

// animated algebra controller
class aa_controller {
public:

	// prompt user for (a,b,c) of ax^2 +bx + c
	std::array<int,3> prompt_for_quadratic_coeffs(bool allow_negatives);
	void animate_quadratic_factorization(std::array<int,3> coeffs);
	void animate_quadratic_factorization(int a, int b, int c);

private:
	scene scene_;
	view view_;
	std::shared_ptr<viewer> viewer_;
};

#endif