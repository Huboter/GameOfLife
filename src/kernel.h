#ifndef KERNEL_H
#define KERNEL_H

#include <vector>

namespace gpgpu {
	void doGeneration(std::vector<uint8_t>& board, const unsigned int numberOfRows, const unsigned int numberOfColumns, const unsigned int numberOfGenerations);
}

#endif