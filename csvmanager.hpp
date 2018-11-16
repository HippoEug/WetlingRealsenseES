#pragma once

#include <memory>
#include <vector>

template <class T, size_t W, size_t H>
class Array2D {
public:
	const int width = W;
	const int height = H;
	typedef typename T type;

	Array2D() {
		buffer.resize(width*height);
	}

	T& operator() (int x, int y) {
		return buffer[y*width + x];
	}

	const T& operator() (int x, int y) const {
		return buffer[y*width + x];
	}

private:
	std::vector<T> buffer;
};
