#ifndef ALGEBRA_H
#define ALGEBRA_H

#include <tuple>
#include <memory>
#include <string>
#include <map>

class algebra {
public:
	static std::tuple<bool,int,int,int,int> quad_factor(int a, int b, int c);
	static std::string factorization_to_string(std::tuple<bool,int,int,int,int> factorization);
	static float discriminant(float a, float b, float c);
	static bool is_square_number(int n);
	static std::tuple<int,int,int> gcd_ext(int a, int b);
	static std::tuple<int,int> fdiv_qr(int a, int b); // floored division with remainder
	static std::tuple<int,int> tdiv_qr(int a, int b); // truncated division with remainder
	static int gcd(int a, int b);
	static std::string term_to_string(int coeff, std::string var);
	static std::string product_to_string(const std::string &a, const std:: string &b);
	static std::string line_to_string(int coeff, std::string var, int constant);
	static std::string sum_to_string(std::map<std::string,int> var_coeff_map, bool descending_exponents = false);
	static std::map<std::string,int> expand(std::map<std::string,int> sum1, std::map<std::string,int>sum2); // expand sum1 * sum2
	static std::string enclose(std::string expr);
};

#endif