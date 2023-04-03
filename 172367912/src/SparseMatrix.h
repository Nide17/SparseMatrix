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

/**
 * Implement basic functions on a sparse matrix that contains integers.
 * The default value in each element of the matrix is assumed to be 0.
 */
class SparseMatrix
{
protected:
	int rows, cols; // Number of rows and columns in the matrix

	int *nonZerosArray;

	bool *rowStatus;

public:
	int *splitByComma(const char *str)
	{
		if (str[0] != '(')
		{
			return nullptr;
		}

		int *arrayOfInt = new int[3];
		int num, comma = 0;
		char *endptr;

		for (int i = 1; i < strlen(str); i++)
		{
			if (str[i] == ',')
			{
				arrayOfInt[comma++] = num;
				num = 0;
			}
			else if (str[i] == ')')
			{
				arrayOfInt[comma] = num;
				break;
			}
			else if (str[i] >= '0' && str[i] <= '9')
			{
				num = num * 10 + strtol(&str[i], &endptr, 10);
				i += endptr - &str[i] - 1;
			}
		}

		return arrayOfInt;
	}
	/**
	 * Return true if the current row does not have any elements.
	 * @param: rowNumber The rwow that needs to be checked.
	 * @return true if the row is empty i.e. All columns contain zeros, false if even one column
	 * in the row has a non-zero value.
	 */
	bool rowIsEmpty(int rowNumber)
	{
		return !rowStatus[rowNumber];
	};
	/**
	 * Given an input text file, load the matrix values into the matrix dta structure.
	 *
	 * @param matrixFilePath Path of the file which contains the data to create a matrix.
	 *
	 * If the input file does not exist or cannot be read throw an error of type ios_base::failure
	 */
	// SparseMatrix(const char *matrixFilePath)
	// {
	// 	// create a file stream in reading mode
	// 	FILE *inFileStream;
	// 	inFileStream = fopen(matrixFilePath, "r");

	// 	char line[50];
	// 	int num = 50;
	// 	int i, counter;
	// 	i = counter = 0;
	// 	std::string row, col, column_n, row_n, value_n, handler;
	// 	int char_count = 0;
	// 	std::string full_row, line_c;
	// 	while (fgets(line, num, inFileStream))
	// 	{
	// 		/*here I'm checking the line 1 containing total rows.
	// 		 * e.g. rows=7191.
	// 		 */
	// 		if (i == 0)
	// 		{

	// 			full_row = line;
	// 			row = full_row.substr(5, full_row.length());
	// 			// std::cout<<row<<std::endl;

	// 			this->rows = stoi(row); // assign rows with rows number read from the file
	// 			rowStatus = new bool[rows];
	// 			for (int i = 0; i < rows; i++)
	// 			{
	// 				rowStatus[i] = false;
	// 			}
	// 		}

	// 		else if (i == 1)
	// 		{
	// 			int j = 0;
	// 			line_c = line;
	// 			col = line_c.substr(5, line_c.length());
	// 			// assign cols with columns number read from the file
	// 			this->cols = stoi(col);

	// 			// Initialize spareArray with appropriate size
	// 			this->nonZerosArray = new int[this->rows * this->cols];
	// 		}
	// 		/*deal with non-zero elements at specified rows and columns
	// 		 * e.g. (24, 22, -43780598)
	// 		 */
	// 		else
	// 		{
	// 			/*
	// 			 * Array containing three items: current row, current column and the value.
	// 			 */
	// 			//   int* rowColValue=splitByComma(line);
	// 			handler = line;
	// 			for (int k = 0; k < handler.length(); k++)
	// 			{
	// 				if (line[k] == '(' || line[k] == ')')
	// 					continue;
	// 				if (line[k] == ',')
	// 				{
	// 					char_count++;
	// 				}

	// 				if (char_count == 0)
	// 				{
	// 					if (line[k] == ',')
	// 						continue;
	// 					row_n = row_n + line[k];
	// 				}
	// 				if (char_count == 1)
	// 				{
	// 					if (line[k] == ',')
	// 						continue;
	// 					column_n = column_n + line[k];
	// 				}
	// 				if (char_count == 2)
	// 				{
	// 					if (line[k] == ',')
	// 						continue;
	// 					value_n = value_n + line[k];
	// 				}
	// 			}

	// 			this->setElement(atoi(row_n.c_str()), atoi(column_n.c_str()), atoi(value_n.c_str())); // Setting values in Array.

	// 			// set element in the sparse Array with the current value at current row and current column
	// 			// setElement(rowColValue[0],rowColValue[1], rowColValue[2]);

	// 			//   counter++;
	// 		}
	// 		i++;
	// 		char_count = 0;
	// 		row_n = "";
	// 		column_n = "";
	// 		value_n = "";
	// 		handler = "";
	// 	}

	// 	fclose(inFileStream);
	// };

	SparseMatrix(const char *matrixFilePath)
	{
		// create a file stream in reading mode
		FILE *inFileStream;
		inFileStream = fopen(matrixFilePath, "r");

		int row, col, val;
		int numRead = 0;

		// read number of rows
		numRead = fscanf(inFileStream, "rows=%d\n", &rows);
		if (numRead != 1)
			throw std::ios_base::failure("Error reading number of rows");

		rowStatus = new bool[rows];
		for (int i = 0; i < rows; i++)
			rowStatus[i] = false;

		// read number of columns
		numRead = fscanf(inFileStream, "cols=%d\n", &cols);
		if (numRead != 1)
			throw std::ios_base::failure("Error reading number of columns");

		// pre-allocate memory for nonZerosArray
		nonZerosArray = (int *)malloc(rows * cols * sizeof(int));
		if (nonZerosArray == NULL)
			throw std::bad_alloc();

		// read non-zero elements
		while (fscanf(inFileStream, "(%d, %d, %d)\n", &row, &col, &val) == 3)
		{
			setElement(row, col, val);
		}

		fclose(inFileStream);
	}

	// SparseMatrix(char *matrixFilePath);
	/**l
	 * Create a sparse matrix that contains numRows and numCols elements.
	 * Since this is a sparse matrix, all values will be zeros unless filled.
	 * @param numRows: Number of rows.
	 * @param numCols: Number of columns.
	 */
	SparseMatrix(int numRows, int numCols)
	{
		rows = numRows; // update rows with numRows
		cols = numCols; // update cols with numCols

		// create nonZerosArray with size of numRows*numCols
		nonZerosArray = new int[numCols * numRows];

		// create rowStatus with size of numRows
		rowStatus = new bool[numRows];
	};

	~SparseMatrix()
	{
		// delete nonZerosArray to free memory.
		delete[] nonZerosArray;

		// delete rowStatus to free memory.
		delete[] rowStatus;
	};

	/**
	 * Print the matrix to an output file.
	 * @param outputFileName: File to which matrix will be printed.
	 */
	void printToASCIIFile(const char *outputFileName);

	/**
	 * Return the value of the element located at a position in the matrix
	 * @param currRow Row of the position whose value is needed.
	 * @param currCol Col of the position whose value is needed.
	 * @return non-zero value if present in the matrix, zero is returned by default,
	 * 		  even if the row/column values passed are outside the matrix dimensions.
	 */
	int getElement(int currRow, int currCol)
	{
		// return the element at the index (currRow*cols)+currCol.
		return nonZerosArray[(currRow * cols) + currCol];
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
	void setElement(int currRow, int currCol, int value)
	{
		// To set element in nonZerosArray, I will store it at index=(currRow*cols)+currCol.
		nonZerosArray[(currRow * cols) + currCol] = value;

		// updating rowStatus at index equal to current row.
		rowStatus[currRow] = true;
	};

	/**
	 * Operator overloaded to perform addition of matrices.
	 * @param inputObject: SparseMatrix object that needs to be added to this matrix.
	 * @throws: std::invalid_argument In case the dimensions of the input matrix do not match with dimensions of "this" matrix.
	 */
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
