#include <memory>
#include <iostream>
#include <vector>
#include <string>

/*
Store a pointer to the appropriate rendering function in each node.
Use ordinary inheritance to provide an interface to nodes.
Pros:
	Each node can render itself.
	
	The user of the library can extend both the set of nodes
	and the set of rendering functions without editing the 
	underlying code of the library.

Cons:
    To maintain a consistent interface, the render_func
    must take a node& as an parameter, which means that each
    render_func must downcast its parameter to the particular
    node type it renders.

    The user must explicitly assign the appropriate render_func
    to each node as it is created.  With this solution, it would
    be tedious to make a factory type that builds nodes and
    assigns the appropriate render_func.  The user would also
    have to work to extend this factor type as she adds new
    node types and rendering functions.

    It would be tedious to group rendering functions
    into rendering systems.
*/

struct node {
	std::function<void(node&)> render_func;
	void render() {render_func(*this);}
};

struct rect_node : public node {
	int width;
	int height;
};

struct text_node : public node {
	std::string text;
};

void render_text_node_normal(node &n) {
	auto tn = static_cast<text_node &>(n); // don't like this cast
	std::cout << tn.text << std::endl;
}

void render_text_node_caps(node &n) {
	auto tn = static_cast<text_node &>(n); // don't like this cast
	std::string upper_text = tn.text;
	for (auto &c : upper_text) {c = std::toupper(c);}
	std::cout << upper_text << std::endl;
}


int main() {
	text_node tn;
	tn.text = "hello";
	tn.render_func = render_text_node_normal;
	tn.render();
	tn.render_func = render_text_node_caps;
	tn.render();
}