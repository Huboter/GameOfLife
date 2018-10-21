#ifndef GOL_FILE_HANDLER_H
#define GOL_FILE_HANDLER_H

#include <fstream>
#include <string>

#include "board.h"

class GolFileHandler {
public:
	GolFileHandler(const std::string& fileNameWithPath);

	void getBoardSizeFromFile(unsigned int& rows, unsigned int& columns);
	void initializeBoardFromFile(Board& board);
	void saveBoardAsFile(const std::string& saveFileNameWithPath, const Board& board);

private:
	std::ifstream m_file;
	const std::string m_fileNameWithPath;
};

#endif
