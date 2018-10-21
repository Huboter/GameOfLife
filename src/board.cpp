#include <omp.h>

#include "board.h"

Board::Board(unsigned int rows, unsigned int columns) {
	m_numberOfRows = rows;
	m_numberOfColumns = columns;
	m_board.resize(m_numberOfRows * m_numberOfColumns);
}

void Board::setValueOnPosition(unsigned int position, uint8_t value) {
	m_board[position] = value;
}

uint8_t Board::getValueOnPosition(unsigned int position) const {
	return m_board[position];
}

unsigned int Board::getNumberOfRows() const {
	return m_numberOfRows;
}

unsigned int Board::getNumberOfColumns() const {
	return m_numberOfColumns;
}

void Board::doGeneration(unsigned int numberOfGenerations) {
	const size_t boardSize = m_numberOfRows * m_numberOfColumns;
	std::vector<uint8_t> tempBoard = m_board;

	for (size_t k = 0; k < numberOfGenerations; ++k) {
		for (size_t i = 0; i < boardSize; ++i) {
			const uint8_t numberOfLivingNeighbours = calculateNumberOfLivingNeighbours(i);
			if (m_board[i] == 0) {
				if (numberOfLivingNeighbours == 3) {
					tempBoard[i] = 1;
				}
			}
			else {
				if (numberOfLivingNeighbours != 2 && numberOfLivingNeighbours != 3) {
					tempBoard[i] = 0;
				}
			}
		}

		m_board = tempBoard;
	}
}

void Board::doGenerationOpenMP(unsigned int numberOfGenerations, uint8_t numberOfThreads) {
	const size_t boardSize = m_numberOfRows * m_numberOfColumns;
	std::vector<uint8_t> tempBoard = m_board;

	for (size_t k = 0; k < numberOfGenerations; ++k) {
		omp_set_dynamic(0);
		omp_set_num_threads(numberOfThreads);
		#pragma omp parallel for
		for (int i = 0; i < boardSize; ++i) {
			const uint8_t numberOfLivingNeighbours = calculateNumberOfLivingNeighbours(i);
			if (m_board[i] == 0) {
				if (numberOfLivingNeighbours == 3) {
					tempBoard[i] = 1;
				}
			}
			else {
				if (numberOfLivingNeighbours != 2 && numberOfLivingNeighbours != 3) {
					tempBoard[i] = 0;
				}
			}
		}

		m_board = tempBoard;
	}
}

void Board::doGenerationCUDA(unsigned int numberOfGenerations) {
	gpgpu::doGeneration(m_board, m_numberOfRows, m_numberOfColumns, numberOfGenerations);
}

uint8_t Board::calculateNumberOfLivingNeighbours(unsigned int position) {
	uint8_t numberOfLivingNeighbours = 0;
	
	unsigned int row = position / m_numberOfColumns;
	unsigned int column = position - (row * m_numberOfColumns);

	unsigned int topRow = row - 1;
	unsigned int bottomRow = row + 1;
	unsigned int leftColumn = column - 1;
	unsigned int rightColumn = column + 1;

	if (row == 0) {
		topRow = m_numberOfRows - 1;
	}
	else if (row == m_numberOfRows - 1) {
		bottomRow = 0;
	}

	if (column == 0) {
		leftColumn = m_numberOfColumns - 1;
	}
	else if (column == m_numberOfColumns - 1) {
		rightColumn = 0;
	}

	unsigned int rowAdder = topRow * m_numberOfColumns;
	numberOfLivingNeighbours += m_board[leftColumn + rowAdder];
	numberOfLivingNeighbours += m_board[column + rowAdder];
	numberOfLivingNeighbours += m_board[rightColumn + rowAdder];

	rowAdder = row * m_numberOfColumns;
	numberOfLivingNeighbours += m_board[leftColumn + rowAdder];
	numberOfLivingNeighbours += m_board[rightColumn + rowAdder];

	rowAdder = bottomRow * m_numberOfColumns;
	numberOfLivingNeighbours += m_board[leftColumn + rowAdder];
	numberOfLivingNeighbours += m_board[column + rowAdder];
	numberOfLivingNeighbours += m_board[rightColumn + rowAdder];

	return numberOfLivingNeighbours;
}