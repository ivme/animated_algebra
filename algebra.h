#include <tuple>
#include <memory>
#include <string>

class Algebra {
public:
	static std::tuple<bool,int,int,int,int> quad_factor(int a, int b, int c);
	static std::string factorization_to_string(std::tuple<bool,int,int,int,int> factorization);
	static float discriminant(float a, float b, float c);
	static bool is_square_number(int n);
	static std::tuple<int,int,int> gcd_ext(int a, int b);
	static std::tuple<int,int> fdiv_qr(int a, int b);
	static int gcd(int a, int b);
	static std::string term_to_string(int coeff, std::string var);
	static std::string product_to_string(const std::string &a, const std:: string &b);
	static std::string line_to_string(int coeff, std::string var, int constant);
};