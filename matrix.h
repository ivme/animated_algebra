#ifndef MATRIX_H
#define MATRIX_H

#include <vector>
#include <iostream>

template <class T> 
class matrix {
public:
	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::const_iterator const_iterator;

	// constructors
	matrix(unsigned int rows, unsigned int cols) : row_count(rows), col_count(cols), data(std::vector<T>(rows * cols)) {}
	matrix(unsigned int rows, unsigned int cols, const T &initial_value) :
		row_count(rows), col_count(cols), data(std::vector<T>(rows * cols,initial_value)) {}

	T &entry(unsigned int row,unsigned int col);
	
	iterator begin() {return data.begin();}
	iterator end() {return data.end();}
	const_iterator cbegin() const {return data.cbegin();}
	const_iterator cend() const {return data.cend();}
	iterator row_begin(unsigned int row) {return data.begin() + row * col_count;}
	iterator row_end(unsigned int row) {return data.begin() + (row + 1) * col_count;}
	const_iterator row_cbegin(unsigned int row) const {return data.begin() + row * col_count;}
	const_iterator row_cend(unsigned int row) const {return data.begin() + (row + 1) * col_count;}

	unsigned int rows() const {return row_count;}
	unsigned int cols() const {return col_count;}

private:
	const unsigned int row_count;
	const unsigned int col_count;
	std::vector<T> data;
};

template <typename T>
std::ostream &operator <<(std::ostream &os,const matrix<T> &m) {
	std::ostream_iterator<T,wchar_t> out_it (std::wcout);
	for (int i = 0; i < m.rows(); ++i) {
		std::copy(m.row_cbegin(i),m.row_cend(i), out_it);
		std::wcout << (i == m.rows() - 1 ? "" : "\n");
	}
	return os;
}

#endif