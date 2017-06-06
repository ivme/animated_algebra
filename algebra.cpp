#include "algebra.h"
#include <vector>
#include <cmath>

std::tuple<bool,int,int,int,int> Algebra::quad_factor(int a, int b, int c) {
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
		int gcd_a_p = Algebra::gcd(a,p);
		int gcd_q_c = Algebra::gcd(q,c);
		
		if (a < 0) {gcd_a_p *= -1;}
		if (q < 0) {gcd_q_c *= -1;}
		std::get<0>(f) = true;
		std::get<1>(f) = gcd_a_p;
		std::get<2>(f) = gcd_q_c;
		std::get<3>(f) = a / gcd_a_p;
		std::get<4>(f) = p / gcd_a_p;
		return f;
}

int signum(int x) {
	if (x < 0) return -1;
	if (x > 0) return 1;
	return 0;
}

std::string Algebra::factorization_to_string(std::tuple<bool,int,int,int,int> f) {
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

std::string Algebra::term_to_string(int coeff, std::string var) {
	switch (coeff) {
		case 1:
		return var;
		case -1:
		return "-" + var;
		default:
		return std::to_string(coeff) + var;
	}
}

std::string Algebra::product_to_string(const std::string &a, const std:: string &b) {
	if (a == "1") {return b;}
	if (b == "1") {return a;}
	if (a == b) {return a + "^2";}
	return a + "*" + b;
}

std::string Algebra::line_to_string(int coeff, std::string var, int constant) {
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

float Algebra::discriminant(float a, float b, float c) {
	return b * b - 4 * a * c;
}

bool Algebra::is_square_number(int n) {
	if (n < 0) {return false;}
	int floored_root = std::floor(std::sqrt(n));
	return floored_root * floored_root == n;
}

int Algebra::gcd(int a, int b) {
	return std::get<0>(gcd_ext(a,b));
}

std::tuple<int,int,int> Algebra::gcd_ext(int a, int b) {
	std::vector<int> q = {0};
	std::vector<int> r = {a,b};
	std::vector<int> s = {1,0};
	std::vector<int> t = {0,1};
	int i = 1;
	while (r[i] != 0) {
		auto qr = Algebra::fdiv_qr(r[i-1],r[i]);
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
std::tuple<int,int> Algebra::fdiv_qr(int a, int b) {
	int q = a / b;
	if ((q < 0) && (q * b != a)) {
		q -= 1;
	}
	int r = a - q * b;
	return std::tuple<int,int>(q, r);
}