#include <iostream>
#include <iomanip>
#include <chrono>

#include "gol_file_handler.h"

void printTime(double timeInSeconds);

int main(int argc, char* argv[]) {
	const std::string loadPath = "gol_files/in/";
	const std::string savePath = "gol_files/out/";
	std::string loadFileNameWithPath = loadPath + "random10000_in.gol";
	std::string saveFileNameWithPath = savePath + "default_save.gol";

	const char optionLoad[] = "--load";
	const char optionGenerations[] = "--generations";
	const char optionSave[] = "--save";
	const char optionMode[] = "--mode";
	const char optionThreads[] = "--threads";

	unsigned int numberOfGenerations = 250;
	unsigned int numberOfThreads = 8;

	enum Mode {
		Seq, OpenMP, CUDA
	};

	Mode mode = CUDA;

	for (int i = 1; i < argc; ++i) {
		if (argv[i] != nullptr) {
			if (strcmp(argv[i], optionLoad) == 0) {
				loadFileNameWithPath = loadPath + argv[i + 1];
			}
			else if (strcmp(argv[i], optionGenerations) == 0) {
				numberOfGenerations = atoi(argv[i + 1]);
			}
			else if (strcmp(argv[i], optionSave) == 0) {
				saveFileNameWithPath = savePath + argv[i + 1];
			}
			else if (strcmp(argv[i], optionMode) == 0) {
				if (strcmp(argv[i + 1], "seq") == 0) {
					mode = Seq;
				}
				else if (strcmp(argv[i + 1], "omp") == 0) {
					mode = OpenMP;
				}
				else if (strcmp(argv[i + 1], "cuda") == 0) {
					mode = CUDA;
				}
			}
			else if (strcmp(argv[i], optionThreads) == 0) {
				numberOfThreads = atoi(argv[i + 1]);
			}
		}
	}

	std::chrono::high_resolution_clock::time_point timeBegin = std::chrono::high_resolution_clock::now();

	GolFileHandler golFileHandler(loadFileNameWithPath);
	unsigned int numberOfRows, numberOfColumns;

	golFileHandler.getBoardSizeFromFile(numberOfRows, numberOfColumns);
	Board board(numberOfRows, numberOfColumns);
	golFileHandler.initializeBoardFromFile(board);

	if (mode == Mode::Seq) {
		board.doGeneration(numberOfGenerations);
	}
	else if (mode == Mode::OpenMP) {
		board.doGenerationOpenMP(numberOfGenerations, numberOfThreads);
	}
	else if (mode == Mode::CUDA) {
		board.doGenerationCUDA(numberOfGenerations);
	}
	
	golFileHandler.saveBoardAsFile(saveFileNameWithPath, board);

	std::chrono::high_resolution_clock::time_point timeEnd = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> timeSpan = std::chrono::duration_cast<std::chrono::duration<double>>(timeEnd - timeBegin);

	std::cout << mode << "\n";

	printTime(timeSpan.count());
	std::cout << "\n";

	return 0;
}

void printTime(double timeInSeconds) {
	int milliseconds = timeInSeconds * 1000;
	int hours = (milliseconds / (1000 * 60 * 60)) % 24;
	milliseconds -= hours * (1000 * 60 * 60);
	int minutes = (milliseconds / (1000 * 60)) % 60;
	milliseconds -= minutes * (1000 * 60);
	int seconds = (milliseconds / 1000) % 60;
	milliseconds -= seconds * 1000;

	std::cout << std::setfill('0') << std::setw(2) << hours << ":" << std::setfill('0') << std::setw(2) << minutes << ":" << std::setfill('0') << std::setw(2) << seconds << "." << std::setfill('0') << std::setw(3) << milliseconds;
}