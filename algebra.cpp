#include "algebra.h"
#include <vector>
#include <cmath>

std::tuple<bool,int,int,int,int> algebra::quad_factor(int a, int b, int c) {
		// factors ax^2 + bx + c into (mx + n)(rx + s)
		// returns (factorable,m,n,r,s)
		auto f = std::tuple<bool,int,int,int,int>();
		float d = discriminant(a,b,c);
		if (!is_square_number(d)) {
			std::get<0>(f) = false;
			return f;
		}

		int sqrt_d = std::sqrt(d);
		// p and q are two integers whose product is ac
		// and whose sum is b
		int p = (b + sqrt_d) / 2;
		int q = (b - sqrt_d) / 2;
		int gcd_a_p = algebra::gcd(a,p);
		int gcd_q_c = algebra::gcd(q,c);
		
		if (a < 0) {gcd_a_p *= -1;}
		if (q < 0) {gcd_q_c *= -1;}
		std::get<0>(f) = true;
		std::get<1>(f) = gcd_a_p;
		std::get<2>(f) = gcd_q_c;
		std::get<3>(f) = a / gcd_a_p;
		std::get<4>(f) = p / gcd_a_p;
		return f;
}

std::map<std::string,int> algebra::expand(std::map<std::string,int> sum1, std::map<std::string,int>sum2) {
	std::map<std::string,int> product;
	for (auto pair1 : sum1) {
		std::string var1 = pair1.first;
		for (auto pair2 : sum2) {
			std::string var2 = pair2.first;
			std::string compound_var;
			if (var1 == var2) {
				if (var1 == "") {compound_var = "";}
				else {compound_var = var1 + "^2";}
			}
			else {
				// canonical order
				if (var1 < var2) {compound_var = var1 + var2;}
				else {compound_var = var2 + var1;}
			}
			product[compound_var] += pair1.second * pair2.second;
		}
	}
	return product;
}

int signum(int x) {
	if (x < 0) return -1;
	if (x > 0) return 1;
	return 0;
}

std::string algebra::factorization_to_string(std::tuple<bool,int,int,int,int> f) {
	if (std::get<0>(f) == false) {
		return "unfactorable";
	} else {
		// (mx + n)(rx + s)
		int m = std::get<1>(f);
		int n = std::get<2>(f);
		int r = std::get<3>(f);
		int s = std::get<4>(f);
		
		return "(" + line_to_string(m,"x",n) + ")(" 
				+ line_to_string(r,"x",s) + ")"; 
	}
}

std::string algebra::term_to_string(int coeff, std::string var) {
	if (var == "") {return std::to_string(coeff);}
	switch (coeff) {
		case 0:
		return "0";
		case 1:
		return var;
		case -1:
		return "-" + var;
		default:
		return std::to_string(coeff) + var;
	}
}

std::string algebra::product_to_string(const std::string &a, const std:: string &b) {
	if (a == "1") {return b;}
	if (b == "1") {return a;}
	if (a == b) {return a + "^2";}
	return a + "*" + b;
}

std::string algebra::line_to_string(int coeff, std::string var, int constant) {
	if (coeff == 0) {return std::to_string(constant);}
	std::string s = term_to_string(coeff,var);
	if (constant > 0) {
		return s + " + " + std::to_string(constant);
	}
	if (constant == 0) {
		return s;
	} else {
		return s + " - " + std::to_string(-1 * constant);
	}
}

template <class ITERATOR> // forward or reverse iterator to std::map<std::string,int>
std::string sum_to_string_(ITERATOR begin, ITERATOR end) {
	ITERATOR it = begin;
	std::string out{};
	if (it->second != 0) {
		out += algebra::term_to_string(it->second,it->first);
	}
	++it;
	while (it != end) {
		if (it->second > 0) {
			out += " + ";
			out += algebra::term_to_string(it->second,it->first);
		}
		if (it->second < 0) {
			out += " - ";
			out += algebra::term_to_string(-1*(it->second),it->first);	
		}
		++it;
	}
	
	if (out.empty()) {return "0";}
	else {return out;}
}

// explicitly instantiate sum_to_string for forward and reverse iterators
using map_str_int = std::map<std::string,int>;
template std::string sum_to_string_<map_str_int::const_iterator>(map_str_int::const_iterator, map_str_int::const_iterator);
template std::string sum_to_string_<map_str_int::const_reverse_iterator>(map_str_int::const_reverse_iterator, map_str_int::const_reverse_iterator);

std::string algebra::sum_to_string(map_str_int var_coeff_map, bool descending_exponents) {
	if (descending_exponents) {
		return sum_to_string_(var_coeff_map.crbegin(),var_coeff_map.crend());
	} else {
		return sum_to_string_(var_coeff_map.cbegin(),var_coeff_map.cend());
	}
}


std::string algebra::enclose(std::string expr) {
	expr.insert(expr.begin(),'(');
	expr.push_back(')');
	return expr;
}

float algebra::discriminant(float a, float b, float c) {
	return b * b - 4 * a * c;
}

bool algebra::is_square_number(int n) {
	if (n < 0) {return false;}
	int floored_root = std::floor(std::sqrt(n));
	return floored_root * floored_root == n;
}

int algebra::gcd(int a, int b) {
	return std::get<0>(gcd_ext(a,b));
}

std::tuple<int,int,int> algebra::gcd_ext(int a, int b) {
	std::vector<int> q = {0};
	std::vector<int> r = {a,b};
	std::vector<int> s = {1,0};
	std::vector<int> t = {0,1};
	int i = 1;
	while (r[i] != 0) {
		auto qr = algebra::fdiv_qr(r[i-1],r[i]);
		q.push_back(std::get<0>(qr));
		r.push_back(std::get<1>(qr));
		s.push_back(s[i-1] - q[i] * s[i]);
		t.push_back(t[i-1] - q[i] * t[i]);
		++i;
	}

	// normalize gcd, s, t
	if (r[i-1] < 0) {
		return std::tuple<int,int,int>(-1 * r[i-1],-1 * s[i-1],-1 * t[i-1]);
	} else {
		return std::tuple<int,int,int>(r[i-1],s[i-1],t[i-1]);
	}
}

// floored division
// returns (quotient, remainder)
std::tuple<int,int> algebra::fdiv_qr(int a, int b) {
	int q = a / b;
	if ((q < 0) && (q * b != a)) {
		q -= 1;
	}
	int r = a - q * b;
	return std::tuple<int,int>(q, r);
}

// truncated division
std::tuple<int,int> algebra::tdiv_qr(int a, int b) {
	int q = a / b;
	int r = a - q * b;
	return std::tuple<int,int>(q,r);
}