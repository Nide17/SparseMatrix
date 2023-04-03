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
using namespace std;


// structure of a node in the tree
struct MatrixElement
{
	int row;
	int col;
	int value;
	MatrixElement *leftChild;
	MatrixElement *rightChild;
};

struct Matrix
{
	MatrixElement *el = nullptr; // pointer to the root node
};

/**
 * Implement basic functions on a sparse matrix that contains integers.
 *
 * The default value in each element of the matrix is assumed to be 0.
 */
class SparseMatrix
{
protected:
	int rows, cols; // Number of rows and columns in the matrix
	Matrix *elements; // Matrix of elements

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

	SparseMatrix(int numRows, int numCols)
	{
		if (numRows <= 0 || numCols <= 0)
			throw invalid_argument("Rows or columns out of range");

		rows = numRows;
		cols = numCols;

		// allocate memory for the matrix
		elements = new Matrix[rows];
	}
	/**
	 * Print the matrix to an output file.
	 */
	void printToASCIIFile(char *outputFileName);

	/**
	 * Return the value of the element located at a position in the matrix
	 * @param currRow Row of the position whose value is needed.
	 * @param currCol Col of the position whose value is needed.
	 */
	int getElement(int currRow, int currCol)
	{
		if (rows < currRow || cols < currCol || currCol < 0 || currRow < 0)
			throw invalid_argument("Rows or columns out of range");

		// allocate memory for temporary node and initialize it 
		MatrixElement *point = elements[currRow].el;


		// traverse the tree and search for the node 
		while (point != NULL)
		{
			// if the node is greater than the key, then move to the left child
			if (point->col > currCol)
				point = point->leftChild;

			// if the node is less than the key, then move to the right child
			else if (point->col < currCol)
				point = point->rightChild;

			// if the node is equal to the key, then return the value
			else
				return point->value;
		}

		return 0;
	};
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
	int setElement(int currRow, int currCol, int value)
	{
		if (rows < currRow || cols < currCol || currCol < 0 || currRow < 0)
			throw invalid_argument("Rows or columns out of range");

		return add_element(elements[currRow].el, currRow, currCol, value);
	};

	// add element into the matrix and return 1 if the element is added successfully
	int add_element(MatrixElement *&element, int row, int col, int val)
	{

		// If the element is null, then create a new node and add it to the tree.
		if (element == NULL)
		{
			MatrixElement *node = new MatrixElement;
			node->value = val;
			node->col = col;
			node->row = row;
			node->leftChild = node->rightChild = NULL;
			element = node;
		}

		// If the element is not null, then check if the element is already present in the tree.
		else if (element->col > col)
			add_element(element->leftChild, row, col, val);

		else if (element->col < col)
			add_element(element->rightChild, row, col, val);

		// If the element is already present in the tree, then update the value.
		else
		{
			if (val == 0)
				return 1;
			else
			{
				element->value = val;
				return 1;
			}
		}
		return 0;
	}


	/**
	 * Read a sparse matrix from a file.
	 *
	 * @param file_name Name of the file to read the matrix from.
	 */
	void read_sparse_matrix(char *file_name)
	{
		// Open the file
		FILE *inputFile = fopen(file_name, "r");

		// Check if the file is opened successfully
		if (!inputFile)
			throw std::invalid_argument("Error opening file\n");

		// Read the number of rows and columns
		char *lineBuffer = new char[2048];

		// Read the number of rows and columns
		fscanf(inputFile, "rows=%d\n", &rows);
		fscanf(inputFile, "cols=%d\n", &cols);

		// Allocate memory for the matrix
		elements = new Matrix[rows];

		// Read the matrix elements
		int row, col, val;
		char left_paren, comma1, comma2, right_paren;

		// Read the matrix elements from the file and set them in the matrix
		while (fscanf(inputFile, "(%d, %d, %d)\n", &row, &col, &val) != EOF)
			setElement(row, col, val);
	}

	/**
	 * Check if a row is empty or not.
	 */
	bool isRowEmpty(int row)
	{
		if(elements[row].el == nullptr)
			return true;

		// If the row is not empty, then the first element in the row should be non-zero.
		return false;
	}

	SparseMatrix operator+(SparseMatrix &inputObject);
	SparseMatrix operator-(SparseMatrix &inputObject);
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
