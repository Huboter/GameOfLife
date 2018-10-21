#include "kernel.h"

#include "cuda_runtime.h"
#include "device_launch_parameters.h"

__device__ uint8_t calculateNumberOfLivingNeighbours(uint8_t* board, const unsigned int position, const unsigned int numberOfRows, const unsigned int numberOfColumns) {
	uint8_t numberOfLivingNeighbours = 0;

	unsigned int row = position / numberOfColumns;
	unsigned int column = position - (row * numberOfColumns);

	unsigned int topRow = row - 1;
	unsigned int bottomRow = row + 1;
	unsigned int leftColumn = column - 1;
	unsigned int rightColumn = column + 1;

	if (row == 0) {
		topRow = numberOfRows - 1;
	}
	else if (row == numberOfRows - 1) {
		bottomRow = 0;
	}

	if (column == 0) {
		leftColumn = numberOfColumns - 1;
	}
	else if (column == numberOfColumns - 1) {
		rightColumn = 0;
	}

	unsigned int rowAdder = topRow * numberOfColumns;
	numberOfLivingNeighbours += board[leftColumn + rowAdder];
	numberOfLivingNeighbours += board[column + rowAdder];
	numberOfLivingNeighbours += board[rightColumn + rowAdder];

	rowAdder = row * numberOfColumns;
	numberOfLivingNeighbours += board[leftColumn + rowAdder];
	numberOfLivingNeighbours += board[rightColumn + rowAdder];

	rowAdder = bottomRow * numberOfColumns;
	numberOfLivingNeighbours += board[leftColumn + rowAdder];
	numberOfLivingNeighbours += board[column + rowAdder];
	numberOfLivingNeighbours += board[rightColumn + rowAdder];

	return numberOfLivingNeighbours;
}

__global__ void doGenerationKernel(uint8_t* board, uint8_t* tempBoard, const unsigned int numberOfRows, const unsigned int numberOfColumns, const unsigned int boardSize) {
	unsigned int index = (blockIdx.x * blockDim.x) + threadIdx.x;
	unsigned int stride = blockDim.x * gridDim.x;

	for (unsigned int i = index; i < boardSize; i += stride) {
		const uint8_t numberOfLivingNeighbours = calculateNumberOfLivingNeighbours(board, i, numberOfRows, numberOfColumns);

		if (board[i] == 0 && numberOfLivingNeighbours == 3) {
			tempBoard[i] = 1;
		}
		else if (numberOfLivingNeighbours != 2 && numberOfLivingNeighbours != 3) {
			tempBoard[i] = 0;
		}
	}
}

void gpgpu::doGeneration(std::vector<uint8_t>& board, const unsigned int numberOfRows, const unsigned int numberOfColumns, const unsigned int numberOfGenerations) {
	const size_t boardSize = numberOfRows * numberOfColumns;
	const size_t boardSizeInByte = boardSize * sizeof(uint8_t);

	uint8_t* cudaBoard;
	uint8_t* cudaTempBoard;

	cudaMallocManaged(&cudaBoard, boardSizeInByte);
	cudaMallocManaged(&cudaTempBoard, boardSizeInByte);

	for (size_t i = 0; i < boardSize; ++i) {
		cudaBoard[i] = board[i];
		cudaTempBoard[i] = board[i];
	}

	const unsigned int blockSize = 256;
	const unsigned int numberOfBlocks = (boardSize + blockSize - 1) / blockSize;

	for (size_t i = 0; i < numberOfGenerations; ++i) {
		doGenerationKernel<<<numberOfBlocks, blockSize>>>(cudaBoard, cudaTempBoard, numberOfRows, numberOfColumns, boardSize);
		cudaDeviceSynchronize();
		cudaMemcpy(cudaBoard, cudaTempBoard, boardSizeInByte, cudaMemcpyDeviceToDevice);
	}

	for (size_t i = 0; i < boardSize; ++i) {
		board[i] = cudaBoard[i];
	}

	cudaFree(cudaTempBoard);
	cudaFree(cudaBoard);
}