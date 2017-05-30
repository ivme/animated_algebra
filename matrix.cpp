#include "matrix.h"
#include <stdexcept>

template<typename T>
T &matrix<T>::entry(unsigned int row, unsigned int col) {
	if (row >= rows()) {
		throw std::out_of_range("row index out of range");
	}
	if (col >= cols()) {
		throw std::out_of_range("column index out of range");
	}
	return data[row * cols() + col];
}

// explicit instantiations
template class matrix<wchar_t>;
template class matrix<int>;