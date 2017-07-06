#include "matrix.h"
#include <stdexcept>
#include <string>

using namespace liven;

template<typename T>
T &matrix<T>::entry(unsigned int row, unsigned int col) {
	if (row >= rows()) {
		throw std::out_of_range("matrix row index [" + std::to_string(row) + "] out of range [0," + std::to_string(rows()) + ")");
	}
	if (col >= cols()) {
		throw std::out_of_range("matrix column index out of range");
	}
	return data[row * cols() + col];
}

template<typename T>
T matrix<T>::centry(unsigned int row, unsigned int col) const {
	if (row >= rows()) {
		throw std::out_of_range("matrix row index [" + std::to_string(row) + "] out of range [0," + std::to_string(rows()) + ")");
	}
	if (col >= cols()) {
		throw std::out_of_range("matrix column index out of range");
	}
	return data[row * cols() + col];
}

namespace liven {
	// explicit instantiations
	template class matrix<wchar_t>;
	template class matrix<int>;
}