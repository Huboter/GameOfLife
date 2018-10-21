#ifndef BOARD_H
#define BOARD_H

#include "kernel.h"

class Board {
	public:
		Board(unsigned int rows, unsigned columns);

		void setValueOnPosition(unsigned int position, uint8_t value);
		uint8_t getValueOnPosition(unsigned int position) const;

		unsigned int getNumberOfRows() const;
		unsigned int getNumberOfColumns() const;

		void doGeneration(unsigned int numberOfGenerations);
		void doGenerationOpenMP(unsigned int numberOfGenerations, uint8_t numberOfThreads);
		void doGenerationCUDA(unsigned int numberOfGenerations);

	private:
		std::vector<uint8_t> m_board;
		unsigned int m_numberOfRows;
		unsigned int m_numberOfColumns;

		uint8_t calculateNumberOfLivingNeighbours(unsigned int position);
};

#endif