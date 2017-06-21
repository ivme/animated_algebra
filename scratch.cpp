#include <memory>
#include <iostream>

struct foo {
	std::weak_ptr<foo> wp;
};

int main () {
	foo myFoo;
	std::cout << (typeid(foo) == typeid(myFoo)) << std::endl;
}
