/*
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
	if (currRow < 0 || currCol < 0 || currRow > rows || currCol > cols)
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
	// Avoid shifting the elements if the new element is to be inserted at the end
	if (insertIndex != matrix->nnz)
	{
		for (int i = matrix->nnz - 1; i >= insertIndex; i--)
		{
			matrix->values[i + 1] = matrix->values[i];
			matrix->colInd[i + 1] = matrix->colInd[i];
		}
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

	// Return 1 to indicate that the element was successfully inserted
	return 1;
}

/**
 * Return the value of the element located at a position in the matrix
 * @param currRow Row of the position whose value is needed.
 * @param currCol Col of the position whose value is needed.
 */
int SparseMatrix::getElement(int currRow, int currCol)
{
	if (currRow < 0 || currCol < 0 || currRow > rows || currCol > cols)
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

void SparseMatrix::print()
{
	printf("The matrix is %d x %d \n", rows, cols);
	printf("The number of non-zero elements is %d \n", matrix->nnz);
	printf("The row pointers are: \n");
	for (int i = 0; i < matrix->rows + 1; i++)
	{
		printf("%d ", matrix->rowPtr[i]);
	}
	printf("\n");
	printf("The column indices are: \n");
	for (int i = 0; i < matrix->nnz; i++)
	{
		printf("%d ", matrix->colInd[i]);
	}
	printf("\n");
	printf("The values are: \n");
	for (int i = 0; i < matrix->nnz; i++)
	{
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

		if(infile.eof())
			break;
	}
	infile.close();

	// FILL THE LAST ROW POINTER TO AVOID SEGMENTATION FAULT - OUT OF BOUNDS
	matrix->rowPtr[rows] = matrix->nnz;
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
		for (int currCol = 0; currCol < inputObject.cols; currCol++)
		{
			value = getElement(currRow, currCol) + inputObject.getElement(currRow, currCol);
			resultMat.setElement(currRow, currCol, value);
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
		for (int currCol = 0; currCol < inputObject.cols; currCol++)
		{
			value = getElement(currRow, currCol) - inputObject.getElement(currRow, currCol);
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
		for (int currCol = 0; currCol < inputObject.cols; currCol++)
		{
			value = 0;
			for (int colOf1 = 0; colOf1 < cols; colOf1++)
				for (int rowOf2 = 0; rowOf2 < inputObject.rows; rowOf2++)
				{
					value = value + getElement(currRow, colOf1) * inputObject.getElement(rowOf2, currCol);
				}
			resultMat.setElement(currRow, currCol, value);
		}
	}
	return resultMat;
}

void SparseMatrixTester::generateTestCases(char *outputFolderPath)
{
}

void SparseMatrixTester::generateTestCases(char *outputFilePath, int min, int max, int numRows, int numCols)
{
}