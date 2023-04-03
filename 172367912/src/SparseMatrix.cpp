/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"

// SparseMatrix::SparseMatrix(char *matrixFilePath)
// {
// 	FILE *inFileStream = fopen(matrixFilePath, "r");
// 	if (!inFileStream)
// 	{
// 		char *message = new char[2048];
// 		sprintf(message, "Cannot open input file for reading");
// 		throw std::ios_base::failure(message);
// 		delete[] message;
// 	}
// 	LogManager::writePrintfToLog(LogManager::Level::Status, "SparseMatrix::SparseMatrix",
// 								 "Loading input file: %s", matrixFilePath);

// }

void SparseMatrix::printToASCIIFile(const char *outputFileName)
{
	FILE *outFileStream = fopen(outputFileName, "w");
	if (!outFileStream)
	{
		char *message = new char[2048];
		sprintf(message, "Cannot open output file for writing, file name=%s", outputFileName);
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
	fclose(outFileStream);
}

SparseMatrix SparseMatrix::operator+(SparseMatrix &inputObject)
{
	if (inputObject.rows != rows)
	{
		char *message = new char[2048];
		sprintf(message, "Matrix arguments passed to + Number of rows are not same %d <> %d", this->rows, inputObject.rows);
		throw std::invalid_argument(message);
		delete[] message;
	}
	if (inputObject.cols != cols)
	{
		char *message = new char[2048];
		sprintf(message, "Matrix arguments passed to + Number of cols are not same %d <> %d", this->cols, inputObject.cols);
		throw std::invalid_argument(message);
		delete[] message;
	}

	SparseMatrix resultMat(inputObject.rows, inputObject.cols);
	int value;

	for (int currRow = 0; currRow < inputObject.rows; currRow++)
	{
		if (!(rowIsEmpty(currRow) && inputObject.rowIsEmpty(currRow)))
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = getElement(currRow, currCol) + inputObject.getElement(currRow, currCol);

				if (value != 0)
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

	for (int currRow = 0; currRow < inputObject.rows; currRow++)
		if (!(rowIsEmpty(currRow) && inputObject.rowIsEmpty(currRow)))
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = getElement(currRow, currCol) - inputObject.getElement(currRow, currCol);
				if (value != 0)
					resultMat.setElement(currRow, currCol, value);
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
	for (int currRow = 0; currRow < rows; currRow++)
	{
		if (!rowIsEmpty(currRow))
		{
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = 0;
				for (int rowOf2 = 0; rowOf2 < inputObject.rows; rowOf2++)
				{
					if (!inputObject.rowIsEmpty(rowOf2))
					{
						int colOf1 = rowOf2;
						value = value + getElement(currRow, colOf1) * inputObject.getElement(rowOf2, currCol);
					}
				}
				if (value != 0)
					resultMat.setElement(currRow, currCol, value);
			}
		}
	}
	return resultMat;
}
