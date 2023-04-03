/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"

SparseMatrix::SparseMatrix(char *matrixFilePath)
{
	FILE *inFileStream = fopen(matrixFilePath, "r");
	if (!inFileStream)
	{
		char *message = new char[2048];
		sprintf(message, "Cannot open input file for reading");
		throw std::ios_base::failure(message);
		delete[] message;
	}
	LogManager::writePrintfToLog(LogManager::Level::Status, "SparseMatrix::SparseMatrix",
								 "Loading input file: %s", matrixFilePath);

	// read the number of rows and columns
	SparseMatrix::read_sparse_matrix(matrixFilePath);
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

	// loop through all rows of first matrix and add them to second matrix
	for (int currRow = 0; currRow < inputObject.rows; currRow++)
	{

		// if both rows are empty, then skip
		if (isRowEmpty(currRow) && inputObject.isRowEmpty(currRow))
			continue;

		// if first row is empty, then copy second row
		if (isRowEmpty(currRow))
		{
			resultMat.elements[currRow] = inputObject.elements[currRow];
		}

		// if second row is empty, then copy first row
		else if (inputObject.isRowEmpty(currRow))
		{
			resultMat.elements[currRow] = elements[currRow];
		}


		// if both rows are not empty, then add them
		else
		{
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				// add elements of both rows and store in result matrix
				value = getElement(currRow, currCol) + inputObject.getElement(currRow, currCol);

				// if value is zero, then do not store it
				value != 0 ? resultMat.setElement(currRow, currCol, value) : 0;
			}
		}
	}

	// return result matrix
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

	//  loop through all rows of first matrix and subtract the second matrix
	for (int currRow = 0; currRow < inputObject.rows; currRow++)
	{

		// if both rows are empty, then skip
		if (isRowEmpty(currRow) && inputObject.isRowEmpty(currRow))
			continue;

		// if row of the second matrix is empty, then copy first's row
		else if (inputObject.isRowEmpty(currRow))
		{
			resultMat.elements[currRow] = elements[currRow];
		}

		// if row of the first matrix is empty, then copy second's row and negate all elements
		else if (isRowEmpty(currRow))
		{

			// copy second's row
			resultMat.elements[currRow] = inputObject.elements[currRow];

			// negate all elements
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = resultMat.getElement(currRow, currCol);
				value != 0 ? resultMat.setElement(currRow, currCol, -value) : 0;
			}
		}


		// if both rows are not empty, then subtract them and store in result matrix
		else
		{
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = getElement(currRow, currCol) - inputObject.getElement(currRow, currCol);
				resultMat.setElement(currRow, currCol, value);
			}
		}
	}
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

	// for each row in first matrix (this) multiply with each column in second matrix (inputObject) 
	// and add the result to result matrix
	for (int currRow = 0; currRow < rows; currRow++)
	{

		// loop through all elements in the current row of first matrix - the current tree
		for (MatrixElement *currElement = elements[currRow].el; currElement != NULL; currElement = currElement->rightChild)
		{

			// loop through all elements in the current column of second matrix - the current tree in second matrix
			for (MatrixElement *currElement2 = inputObject.elements[currElement->col].el; currElement2 != NULL; currElement2 = currElement2->rightChild)
			{
				// add the result to the result matrix at the current row and current column
				value = resultMat.getElement(currRow, currElement2->col) + currElement->value * currElement2->value;

				// if the value is 0, then do not add it to the result matrix
				value != 0 ? resultMat.setElement(currRow, currElement2->col, value) : 0;
			}
		}
	}

	// return the result matrix
	return resultMat;
}

void SparseMatrixTester::generateTestCases(char *outputFolderPath)
{
}

void SparseMatrixTester::generateTestCases(char *outputFilePath, int min, int max, int numRows, int numCols)
{
}
