/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */

#ifndef MATRIX_H_
#define MATRIX_H_

#include <iostream>
#include <fstream>
#include <limits.h>
#include <math.h>
#include <unistd.h>
#include "../../util/GetMemUsage.h"
#include "../../util/LogManager.h"

// sparse matrices using a compressed sparse row (CSR) format
// representing a matrix by three (one-dimensional) arrays, that respectively contain nonzero values, the extents of rows, and column indices
struct CSRMatrix
{
	int *values;
	int *rowPtr;
	int *colInd;
	int rows;
	int cols;
	int nnz;

	CSRMatrix(){
		values = NULL;
		rowPtr = NULL;
		colInd = NULL;
		rows = 0;
		cols = 0;
		nnz = 0;
	};

	CSRMatrix(int numRows, int numCols)
	{
		rows = numRows;
		cols = numCols;
		values = new int[rows * cols]();
		rowPtr = new int[rows]();
		colInd = new int[rows * cols]();
		nnz = 0;
	};
	
	CSRMatrix(int numRows, int numCols, int *values, int *rowPtr, int *colInd, int nnz)
	{
		rows = numRows;
		cols = numCols;
		this->values = values;
		this->rowPtr = rowPtr;
		this->colInd = colInd;
		this->nnz = nnz;
	};
};

/**
 * Implement basic functions on a sparse matrix that contains integers.
 *
 * The default value in each element of the matrix is assumed to be 0.
 */
class SparseMatrix
{
private:
	int rows, cols; // Number of rows and columns in the matrix
	int *values;
	int *rowPtr;
	int *colInd;
	CSRMatrix *matrix;

public:
	/**
	 * Given an input text file, load the matrix values into the matrix dta structure.
	 *
	 * @param matrixFilePath Path of the file which contains the data to create a matrix.
	 *
	 * If the input file cannot be read throw an error of type ios_base::failure
	 */
	SparseMatrix(char *matrixFilePath);

	/**
	 * Create a sparse matrix that contains numRows and numCols elements.
	 * Since this is a sparse matrix, all values will be zeros unless filled.
	 */
	SparseMatrix(int numRows, int numCols);
	/**
	 * Print the matrix to an output file.
	 */
	void printToASCIIFile(char *outputFileName);

	/**
	 * Return the value of the element located at a position in the matrix
	 * @param currRow Row of the position whose value is needed.
	 * @param currCol Col of the position whose value is needed.
	 */
	int getElement(int currRow, int currCol);
	/**8
	 * Set the value of the element located at a position in the matrix
	 *
	 * @param currRow Row of the position whose value is needed.
	 * @param currCol Col of the position whose value is needed.
	 * @param value Value of the element
	 *
	 * @return int: 1 In case the value is set. -1 if the value is not set (currRow > rows or currCol > cols) or currRow or currCol is -ve
	 *
	 */
	int setElement(int currRow, int currCol, int value);
	void print();

	// operator+ IS A CALL TO THE DEFAULT CONSTRUCTOR OF THE CLASS SparseMatrix
	SparseMatrix operator+(SparseMatrix &inputObject);

	// operator- IS A CALL TO THE DEFAULT CONSTRUCTOR OF THE CLASS SparseMatrix
	SparseMatrix operator-(SparseMatrix &inputObject);

	// operator* IS A CALL TO THE DEFAULT CONSTRUCTOR OF THE CLASS SparseMatrix
	SparseMatrix operator*(SparseMatrix &inputObject);
};

class SparseMatrixTester
{
	/**
	 * Get a random int value between INT_MIN and INT_MAX
	 */
	static int genRandomInt()
	{
		const int bitsNeededForRAND_MAX = (int)(log2(RAND_MAX / 2 + 1) + 1.0); /* Or log(RAND_MAX + 1) / log(2) with older language standards */
		int ret = 0;
		for (int i = 0; i < sizeof(int) * CHAR_BIT; i += bitsNeededForRAND_MAX)
		{
			ret <<= bitsNeededForRAND_MAX;
			ret |= rand();
		}
		return ret;
	}

	/**
	 * Generate a random integer between min and max.
	 */
	static int getRandomInt(int min, int max)
	{
		if (min == max)
			return min;
		if (min == INT_MIN && max == INT_MAX)
			return genRandomInt();
		if (max == INT_MIN && min == INT_MAX)
			return genRandomInt();

		int range = max - min + 1;
		if (min > max)
		{
			range = min - max + 1;
		}
		static bool first = true;
		if (first)
		{
			srand(time(NULL)); // seeding for the first time only!
			first = false;
		}

		int randomNumber = genRandomInt();
		int withinRangeNum = (randomNumber % range);
		int value = min + withinRangeNum;
		LogManager::writePrintfToLog(LogManager::Level::Status,
									 "SparseMatrixTester::getRandomInt", "1 randomNumber=%d, min=%d, max=%d, range=%d, value=%d", randomNumber, min, max, range, value);

		// Under some conditions, it is possible that value is not within min and max.
		// for example, if (withinRangeNum < 0 && min < 0)
		if (false)
		{
			if (value < min || value > max)
			{
				value = getRandomInt(min, max);
				return value;
			}
		}
		else
		{
			int indx = 2;
			LogManager::writePrintfToLog(LogManager::Level::Status,
										 "SparseMatrixTester::getRandomInt", "1 withinRangeNum=%d", withinRangeNum);

			if (withinRangeNum < 0 && min < 0)
				withinRangeNum = withinRangeNum * -0.5;

			while (value < min || value > max)
			{
				withinRangeNum = (min + withinRangeNum + max) / indx++;
				value = min + withinRangeNum;
				if (indx == 10)
					indx = 2;
				LogManager::writePrintfToLog(LogManager::Level::Status,
											 "SparseMatrixTester::getRandomInt", "1 withinRangeNum=%d, min=%d, max=%d, indx=%d, value=%d", withinRangeNum, min, max, indx, value);
			}
			return value;
		}

		LogManager::writePrintfToLog(LogManager::Level::Status,
									 "SparseMatrixTester::getRandomInt", "randomNumber=%d, min=%d, max=%d, range=%d, value=%d", randomNumber, min, max, range, value);
		LogManager::writePrintfToLog(LogManager::Level::Status,
									 "SparseMatrixTester::getRandomInt", "%d+ (%d mod %d)\n", min, randomNumber, range);
		return value;
	};

	/**
	 * Function to generate test matrices
	 *
	 * @param outputFilePath Path of the output file.
	 */
	static void generateTestCases(char *outputFilePath, int min, int max, int numRows, int numCols);

public:
	/**
	 * This is the entry level function to generate test cases.
	 *
	 * @param outputFolderPath Path of the output folder.
	 */
	static void generateTestCases(char *outputFolderPath);
};

#endif /* MATRIX_H_ *//*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"
using namespace std;

/**
 * Set the value of the element located at a position in the matrix
 *
 * @param currRow Row of the position whose value is needed.
 * @param currCol Col of the position whose value is needed.
 * @param value Value of the element
 *
 * @return int: 1 In case the value is set. -1 if the value is not set (currRow > rows or currCol > cols) or currRow or currCol is -ve
 *
 */
int SparseMatrix::setElement(int currRow, int currCol, int value)
{
	if (currRow < 0 || currCol < 0 || currRow >= rows || currCol >= cols)
	{
		return -1;
	}

	// Find the start and end indices of the current row in the CSR matrix
	int startIndex = matrix->rowPtr[currRow];
	int endIndex = matrix->rowPtr[currRow + 1];

	// Search for the element in the current row
	for (int i = startIndex; i < endIndex; i++)
	{
		if (matrix->colInd[i] == currCol)
		{
			matrix->values[i] = value;
			return 1;
		}
	}

	// If the element is not found, insert it in the CSR matrix
	int insertIndex = endIndex;
	for (int i = endIndex; i < matrix->nnz; i++)
	{
		if (matrix->colInd[i] > currCol)
		{
			insertIndex = i;
			break;
		}
	}

	// Shift the elements in the CSR matrix to make space for the new element
	for (int i = matrix->nnz; i > insertIndex; i--)
	{
		matrix->values[i] = matrix->values[i - 1];
		matrix->colInd[i] = matrix->colInd[i - 1];
	}

	// Insert the new element in the CSR matrix
	matrix->values[insertIndex] = value;

	// Update the column indices
	matrix->colInd[insertIndex] = currCol;

	// Update the row pointers
	for (int i = currRow + 1; i <= matrix->rows; i++)
	{
		matrix->rowPtr[i]++;
	}

	// Update the number of non-zero elements
	matrix->nnz++;

	return 1;
}

/**
 * Return the value of the element located at a position in the matrix
 * @param currRow Row of the position whose value is needed.
 * @param currCol Col of the position whose value is needed.
 */
int SparseMatrix::getElement(int currRow, int currCol)
{
	if (currRow < 0 || currCol < 0 || currRow >= rows || currCol >= cols)
	{
		return -1;
	}

	// Find the start and end indices of the current row in the CSR matrix
	int startIndex = matrix->rowPtr[currRow];
	int endIndex = matrix->rowPtr[currRow + 1];

	// Search for the element in the current row
	for (int i = startIndex; i < endIndex; i++)
	{
		if (matrix->colInd[i] == currCol)
		{
			return matrix->values[i];
		}
	}

	return 0;
}

SparseMatrix::SparseMatrix(int numRows, int numCols)
{
	rows = numRows;
	cols = numCols;
	matrix = new CSRMatrix(rows, cols);
}

void SparseMatrix::print() {
	printf("The matrix is %d x %d \n", rows, cols);
	printf("The number of non-zero elements is %d \n", matrix->nnz);
	printf("The row pointers are: \n");
	for (int i = 0; i < matrix->rows + 1; i++) {
		printf("%d ", matrix->rowPtr[i]);
	}
	printf("\n");
	printf("The column indices are: \n");
	for (int i = 0; i < matrix->nnz; i++) {
		printf("%d ", matrix->colInd[i]);
	}
	printf("\n");
	printf("The values are: \n");
	for (int i = 0; i < matrix->nnz; i++) {
		printf("%d ", matrix->values[i]);
	}
	printf("\n");
}

SparseMatrix::SparseMatrix(char *matrixFilePath)
{
	std::ifstream infile(matrixFilePath);
	if (!infile)
	{
		std::cerr << "Error: Unable to open matrix file" << std::endl;
		exit(1);
	}
	std::string line;
	getline(infile, line);
	rows = stoi(line.substr(line.find("=") + 1));

	getline(infile, line);
	cols = stoi(line.substr(line.find("=") + 1));

	matrix = new CSRMatrix(rows, cols);

	// Read in the matrix elements
	while (getline(infile, line))
	{
		size_t openParen = line.find("(");
		size_t comma1 = line.find(",", openParen);
		size_t comma2 = line.find(",", comma1 + 1);
		size_t closeParen = line.find(")", comma2);

		int row = stoi(line.substr(openParen + 1, comma1 - openParen - 1));
		int col = stoi(line.substr(comma1 + 1, comma2 - comma1 - 1));
		int value = stoi(line.substr(comma2 + 1, closeParen - comma2 - 1));

		setElement(row, col, value);
	}
	infile.close();
	
	// printf("Reading finished!\n Printing beginning ...\n");
	// print();
}

void SparseMatrix::printToASCIIFile(char *outputFileName)
{
	FILE *outFileStream = fopen(outputFileName, "w");
	if (!outFileStream)
	{
		char *message = new char[2048];
		sprintf(message, "Cannot open output file for writing");
		throw std::ios_base::failure(message);
		delete[] message;
	}
	LogManager::writePrintfToLog(LogManager::Level::Status, "SparseMatrix::printToASCIIFile",
								 "Writing matrix to file: %s", outputFileName);
	fprintf(outFileStream, "rows=%d\n", rows);
	fprintf(outFileStream, "cols=%d\n", cols);
	int value;
	for (int currRow = 0; currRow < rows; currRow++)
		for (int currCol = 0; currCol < cols; currCol++)
		{
			value = getElement(currRow, currCol);
			if (value != 0)
			{
				// printf("The value at (%d, %d) is %d \n", currRow, currCol, value);
				fprintf(outFileStream, "(%d, %d, %d)\n", currRow, currCol, value);
			}
		}
}

SparseMatrix SparseMatrix::operator+(SparseMatrix &inputObject)
{
	if (inputObject.rows != rows)
	{
		char *message = new char[2048];
		sprintf(message, "Number of rows are not same");
		throw std::invalid_argument(message);
		delete[] message;
	}
	if (inputObject.cols != cols)
	{
		char *message = new char[2048];
		sprintf(message, "Number of cols are not same");
		throw std::invalid_argument(message);
		delete[] message;
	}

	SparseMatrix resultMat(inputObject.rows, inputObject.cols);
	int value;

	for (int currRow = 0; currRow < inputObject.rows; currRow++)
	{
		// SKIP EMPTY ROWS FOR BETTER PERFORMANCE ON LARGE MATRICES
		if (inputObject.matrix->rowPtr[currRow] == inputObject.matrix->rowPtr[currRow + 1])
		{
			continue;
		}

		// IF THE ROW IN THE FIRST MATRIX IS EMPTY, COPY THE ROW FROM THE SECOND MATRIX
		if (matrix->rowPtr[currRow] == matrix->rowPtr[currRow + 1])
		{
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = inputObject.getElement(currRow, currCol);
				resultMat.setElement(currRow, currCol, value);
			}
			continue;
		}

		// IF THE ROW IN THE SECOND MATRIX IS EMPTY, COPY THE ROW FROM THE FIRST MATRIX
		if (inputObject.matrix->rowPtr[currRow] == inputObject.matrix->rowPtr[currRow + 1])
		{
			for (int currCol = 0; currCol < cols; currCol++)
			{
				value = getElement(currRow, currCol);
				resultMat.setElement(currRow, currCol, value);
			}
			continue;
		}

		// OTHERWISE, ADD THE TWO ROWS
		for (int currCol = 0; currCol < inputObject.cols; currCol++)
		{
			// SKIP ZERO ELEMENTS
			if (getElement(currRow, currCol) == 0 && inputObject.getElement(currRow, currCol) == 0)
			{
				continue;
			}
			value = getElement(currRow, currCol) + inputObject.getElement(currRow, currCol);
			resultMat.setElement(currRow, currCol, value);
		}
	}

	return resultMat;
}

SparseMatrix SparseMatrix::operator-(SparseMatrix &inputObject)
{
	if (inputObject.rows != rows)
	{
		char *message = new char[2048];
		sprintf(message, "Number of rows are not same");
		throw std::invalid_argument(message);
		delete[] message;
	}
	if (inputObject.cols != cols)
	{
		char *message = new char[2048];
		sprintf(message, "Number of cols are not same");
		throw std::invalid_argument(message);
		delete[] message;
	}

	SparseMatrix resultMat(inputObject.rows, inputObject.cols);
	int value;

	// for (int currRow = 0; currRow < inputObject.rows; currRow++)
	// 	for (int currCol = 0; currCol < inputObject.cols; currCol++)
	// 	{
	// 		value = getElement(currRow, currCol) - inputObject.getElement(currRow, currCol);
	// 		resultMat.setElement(currRow, currCol, value);
	// 	}
	return resultMat;
}

SparseMatrix SparseMatrix::operator*(SparseMatrix &inputObject)
{
	if (inputObject.rows != cols)
	{
		char *message = new char[2048];
		sprintf(message, "Input does not satisfy following condition: Number of rows in second matrix must be equal to num of cols in first matrix");
		throw std::invalid_argument(message);
		delete[] message;
	}
	if (inputObject.cols != rows)
	{
		char *message = new char[2048];
		sprintf(message, "Input does not satisfy following condition: Number of cols in second matrix must be equal to num of rows in first matrix");
		throw std::invalid_argument(message);
		delete[] message;
	}

	/**
	 * In matrix multiplication, number of rows in the result is equal to number of rows in first matrix.
	 * number of cols in the result is equal to number of cols in first matrix.
	 */
	SparseMatrix resultMat(rows, inputObject.cols);
	int value;

	// for (int currRow = 0; currRow < rows; currRow++)
	// {
	// 	for (int currCol = 0; currCol < inputObject.cols; currCol++)
	// 	{
	// 		value = 0;
	// 		for (int colOf1 = 0; colOf1 < cols; colOf1++)
	// 			for (int rowOf2 = 0; rowOf2 < inputObject.rows; rowOf2++)
	// 			{
	// 				// printf("Multiplying!\n");
	// 				value = value + getElement(currRow, colOf1) * inputObject.getElement(rowOf2, currCol);
	// 			}
	// 		resultMat.setElement(currRow, currCol, value);
	// 	}
	// }
	return resultMat;
}

void SparseMatrixTester::generateTestCases(char *outputFolderPath)
{
}

void SparseMatrixTester::generateTestCases(char *outputFilePath, int min, int max, int numRows, int numCols)
{
}
