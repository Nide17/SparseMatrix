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

	CSRMatrix()
	{
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
		rowPtr = new int[rows + 1]();
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

#endif /* MATRIX_H_ */