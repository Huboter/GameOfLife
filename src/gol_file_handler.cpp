#include <iostream>

#include "gol_file_handler.h"

GolFileHandler::GolFileHandler(const std::string& fileNameWithPath) : m_fileNameWithPath(fileNameWithPath) {

}

void GolFileHandler::getBoardSizeFromFile(unsigned int& rows, unsigned int& columns) {
	std::string line;
	std::string number = "";

	m_file.open(m_fileNameWithPath);

	if (m_file.is_open()) {
		std::getline(m_file, line);
		const size_t lineSize = line.size();

		for (size_t i = 0; i < lineSize; ++i) {
			if (line[i] == ',') {
				columns = std::stoi(number);
				number = "";
				i += 1;
			}

			number += line[i];
		}

		rows = std::stoi(number);
		m_file.close();
	}
	else {
		std::cout << "Could not open file" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void GolFileHandler::initializeBoardFromFile(Board& board) {
	std::string line;

	m_file.open(m_fileNameWithPath);

	if (m_file.is_open()) {
		std::getline(m_file, line); // the first line with the size of the board is not needed
		unsigned int offset = 0;

		while (std::getline(m_file, line)) {
			const size_t lineSize = line.size();

			for (size_t i = 0; i < lineSize; ++i) {
				if (line[i] == 'x') {
					board.setValueOnPosition(i + offset, 1);
				}
				else {
					board.setValueOnPosition(i + offset, 0);
				}
			}
			offset += lineSize;
		}

		m_file.close();
	}
	else {
		std::cout << "Could not open file" << std::endl;
		exit(EXIT_FAILURE);
	}
}

void GolFileHandler::saveBoardAsFile(const std::string& saveFileNameWithPath, const Board& board) {
	std::ofstream saveFile;
	saveFile.open(saveFileNameWithPath);

	if (saveFile.is_open()) {
		const unsigned int numberOfRows = board.getNumberOfRows();
		const unsigned int numberOfColumns = board.getNumberOfColumns();
		const unsigned int boardSize = numberOfRows * numberOfColumns;
		saveFile << numberOfColumns << "," << numberOfRows << "\n";

		for (size_t i = 0; i < boardSize; ++i) {
			if (i != 0 && i % numberOfColumns == 0) {
				saveFile << "\n";
			}

			if (board.getValueOnPosition(i) == 1) {
				saveFile << 'x';
			}
			else {
				saveFile << '.';
			}
		}

		saveFile << "\n";

		saveFile.close();
	}
	else {
		std::cout << "Could not open file" << std::endl;
		exit(EXIT_FAILURE);
	}
}