#include "algebra.h"
#include <cassert>

void main() {
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

	auto f3 = algebra::quad_factor(1,6,9);  // 0x^2 + 2x + 6 = (0x + 2)(x + 3)
	assert(std::get<0>(f3) == true);
	assert(std::get<1>(f3) == 1);
	assert(std::get<2>(f3) == 3);
	assert(std::get<3>(f3) == 1);
	assert(std::get<4>(f3) == 3);

	assert(algebra::term_to_string(0,"x") == "0");
	assert(algebra::term_to_string(1,"x") == "x");
	assert(algebra::term_to_string(2,"x") == "2x");
	assert(algebra::term_to_string(-1,"x") == "-x");
	assert(algebra::term_to_string(2,"") == "2");
	assert(algebra::term_to_string(1,"1") == "1");
	assert(algebra::term_to_string(5,"") == "5");

	assert(algebra::product_to_string("a","b") == "a*b");
	assert(algebra::product_to_string("1","b") == "b");
	assert(algebra::product_to_string("a","1") == "a");
	assert(algebra::product_to_string("a","a") == "a^2");

	assert(algebra::enclose("x") == "(x)");
	assert(algebra::line_to_string(0,"x",1) == "1");
	assert(algebra::line_to_string(1,"x",4) == "x + 4");
	assert(algebra::line_to_string(2,"x",-3) == "2x - 3");

	assert(algebra::sum_to_string({{"",4}}) == "4");
	assert(algebra::sum_to_string({{"x",0}}) == "0");
	assert(algebra::sum_to_string({{"x",1}}) == "x");
	assert(algebra::sum_to_string({{"y",-3},{"x",2}}) == "2x - 3y");
	assert(algebra::sum_to_string({{"a",-2},{"c",-3},{"b",1}}) == "-2a + b - 3c");
	assert(algebra::sum_to_string({{"",2},{"x",1}}) == "2 + x");

	std::map<std::string,int> x{{"x",1}};
	assert(algebra::sum_to_string(algebra::expand(x,x)) == "x^2");
	std::map<std::string,int> x_p_2{{"",2},{"x",1}};
	std::map<std::string,int> x_m_4{{"",-4},{"x",1}};
	assert(algebra::sum_to_string(algebra::expand(x_p_2,x_m_4)) == "-8 - 2x + x^2");
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