#ifndef UNIQUE_SEQUENCE_H
#define UNIQUE_SEQUENCE_H

#include <list>

/*
  Store a sequence of unique values.
  Inserting an existing value at a new
  position in the sequence causes that
  value's position to change.
*/

namespace liven {

template <class T>
class unique_sequence {
public:
	using iterator = typename std::list<T>::iterator;
	using const_iterator = typename std::list<T>::const_iterator;
	using size_type = typename std::list<T>::size_type;
	using value_type = T;

	unique_sequence() = default;
	unique_sequence(std::initializer_list<T> list) : l(list) {}

	iterator begin() {return l.begin();}
	iterator end() {return l.end();}
	const_iterator cbegin() {return l.cbegin();}
	const_iterator cend() {return l.cend();}

	size_type size() {return l.size();}
	bool empty() {return l.empty();}

	void insert(iterator pos, const T &value) {
		auto it = std::find(l.begin(),l.end(),value);
		if (it != l.end()) { /* value already exists */
			if (it != pos) { /* value isn't already in the requested position*/
				l.erase(it);
				l.insert(pos,value);
			}
		} else {
			l.insert(pos,value);
		}
	}

	void erase(iterator pos) {
		l.erase(pos);
	}

	void erase(iterator first, iterator last) {
		l.erase(first,last);
	}

	void splice(iterator pos, unique_sequence &other) {
		l.splice(pos, other.l);
	}

	void push_back(const T &value) {insert(l.end(),value);}
	void pop_back() {erase(--l.end());}

private:
	std::list<T> l;
};

}

#endif