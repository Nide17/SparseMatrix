/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"
using namespace std;

// CONSTRUCTOR TO INITIALIZE THE SPARSE MATRIX WITH THE GIVEN NUMBER OF ROWS AND COLUMNS
SparseMatrix::SparseMatrix(int numRows, int numCols)
{
	rows = numRows;
	cols = numCols;
	values = new int[rows * cols]();
	rowPtr = new int[rows + 1](); // +1 for the last element to avoid segfault
	colInd = new int[rows * cols]();
	nnz = 0;
}

// CONSTRUCTOR TO INITIALIZE THE SPARSE MATRIX WITH THE GIVEN NUMBER OF ROWS, COLUMNS AND NON-ZERO ELEMENTS
SparseMatrix::SparseMatrix(int numRows, int numCols, int numNonZeroElements)
{
	rows = numRows;
	cols = numCols;
	nnz = numNonZeroElements;
	values = new int[nnz]();
	rowPtr = new int[rows + 1](); // +1 for the last element to avoid segfault
	colInd = new int[nnz]();
}

// SETTING AND ADDING ELEMENT IN THE MATRIX
int SparseMatrix::setElement(int currRow, int currCol, int value)
{
	// CHECK IF THE ROW AND COLUMN ARE VALID
	if (currRow < 0 || currCol < 0 || currRow > rows || currCol > cols)
	{
		return -1;
	}

	// FIND THE START AND END INDEX OF THE CURRENT ROW IN THE CSR MATRIX
	int startIndex = rowPtr[currRow];
	int endIndex = rowPtr[currRow + 1];

	// USE BINARY SEARCH TO FIND THE INSERTION POINT FOR THE ELEMENT
	// SEARCH FOR THE ELEMENT IN THE CURRENT ROW
	int insertIndex = binarySearch(colInd, startIndex, endIndex, currCol);

	// IF THE ELEMENT IS ALREADY IN THE MATRIX, UPDATE ITS VALUE
	if (insertIndex < endIndex && colInd[insertIndex] == currCol)
	{
		values[insertIndex] = value;
		return 1;
	}

	// If the element is not found, insert it in the CSR matrix
	// Shift the elements in the CSR matrix to make space for the new element
	// Avoid shifting the elements if the new element is to be inserted at the end
	if (insertIndex != nnz)
	{
		for (int i = nnz - 1; i >= insertIndex; i--)
		{
			values[i + 1] = values[i];
			colInd[i + 1] = colInd[i];
		}
	}

	// INSERT THE VALUE OF THE ELEMENT IN THE VALUES ARRAY AS NEW ELEMENT
	values[insertIndex] = value;

	// UPDATE THE COLUMN INDEX OF THE NEW ELEMENT
	// THE COLUMN INDEX OF THE NEW ELEMENT MUST BE THE CURRENT COLUMN INDEX
	colInd[insertIndex] = currCol;

	// UPDATE THE ROW POINTER OF THE ROWS FOLLOWING THE CURRENT ROW
	for (int i = currRow + 1; i <= rows; i++)
	{
		rowPtr[i]++;
	}

	// UPDATE THE NUMBER OF NON-ZERO ELEMENTS IN THE MATRIX
	nnz++;

	// RETURN 1 IF THE ELEMENT IS SET
	return 1;
}

// GETTING THE ELEMENT FROM THE MATRIX AT THE GIVEN ROW AND COLUMN
int SparseMatrix::getElement(int currRow, int currCol)
{
	// CHECK IF THE ROW AND COLUMN ARE VALID
	if (currRow < 0 || currCol < 0 || currRow > rows || currCol > cols)
	{
		return -1;
	}

	// FIND THE START AND END INDEX OF THE CURRENT ROW IN THE CSR MATRIX
	int startIndex = rowPtr[currRow];
	int endIndex = rowPtr[currRow + 1];

	// Binary search for the element in the current row
	int index = binarySearch(colInd, startIndex, endIndex, currCol);

	// If the element is found, return its value
	if (index < endIndex && colInd[index] == currCol)
	{
		return values[index];
	}

	// If the element is not found, return 0
	else
	{
		return 0;
	}
}

// READING THE MATRIX FROM THE FILE AND STORING IT IN THE CSR FORMAT IN THE MATRIX
SparseMatrix::SparseMatrix(char *matrixFilePath)
{
	// OPEN THE FILE AND READ THE NUMBER OF ROWS AND COLUMNS
	std::ifstream infile(matrixFilePath);
	if (!infile)
	{
		std::cerr << "Error: Unable to open matrix file" << std::endl;
		exit(1);
	}

	// READ THE FIRST LINE OF THE FILE TO GET THE NUMBER OF ROWS
	std::string line;
	getline(infile, line);
	rows = stoi(line.substr(line.find("=") + 1));

	// READ THE SECOND LINE OF THE FILE TO GET THE NUMBER OF COLUMNS
	getline(infile, line);
	cols = stoi(line.substr(line.find("=") + 1));

	// INITIALIZE THE CSR MATRIX
	nnz = 0;
	values = new int[rows * cols]();
	rowPtr = new int[rows + 1](); // +1 for the last element to avoid segfault
	colInd = new int[rows * cols]();

	// READ THE REMAINING LINES OF THE FILE TO GET THE NON-ZERO ELEMENTS
	while (getline(infile, line))
	{
		// FIND THE OPENING AND CLOSING PARENTHESES AND COMMAS POSITIONS IN THE LINE
		int openParenthesis = line.find("(");
		int comma1 = line.find(",", openParenthesis);
		int comma2 = line.find(",", comma1 + 1);
		int closeParenthesis = line.find(")", comma2);

		// GET THE ROW, COLUMN AND VALUE OF THE ELEMENT
		int row = stoi(line.substr(openParenthesis + 1, comma1 - openParenthesis - 1));
		int col = stoi(line.substr(comma1 + 1, comma2 - comma1 - 1));
		int value = stoi(line.substr(comma2 + 1, closeParenthesis - comma2 - 1));

		// SET OR ADD THE ELEMENT TO THE MATRIX
		setElement(row, col, value);
	}
	printf("Finished reading a matrix of %d x %d \n", rows, cols);
	// SparseMatrix::print();

	// CLOSE THE FILE
	infile.close();

	// FILL THE LAST ROW POINTER IN THE MATRIX TO AVOID SEGMENTATION FAULT - OUT OF BOUNDS
	rowPtr[rows] = nnz;
}

void SparseMatrix::printToASCIIFile(char *outputFileName)
{
	printf("Writing matrix to file: %s\n", outputFileName);
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
	printf("Finished writing matrix to file: %s\n", outputFileName);
	fclose(outFileStream);
}

SparseMatrix SparseMatrix::operator+(SparseMatrix &inputObject)
{
	printf("Started adding two matrices \n");
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
		if (rowPtr[currRow] == rowPtr[currRow + 1] && inputObject.rowPtr[currRow] == inputObject.rowPtr[currRow + 1])
		{
			continue;
		} 

		// IF THE ROW IN THE FIRST MATRIX IS EMPTY, COPY THE ROW FROM THE SECOND MATRIX
		if (rowPtr[currRow] == rowPtr[currRow + 1])
		{
			for (int currCol = inputObject.rowPtr[currRow]; currCol < inputObject.rowPtr[currRow + 1]; currCol++)
			{
				resultMat.setElement(currRow, inputObject.colInd[currCol], inputObject.values[currCol]);
			}
			continue;
		}

		// IF THE ROW IN THE SECOND MATRIX IS EMPTY, COPY THE ROW FROM THE FIRST MATRIX
		else if (inputObject.rowPtr[currRow] == inputObject.rowPtr[currRow + 1])
		{
			for (int currCol = rowPtr[currRow]; currCol < rowPtr[currRow + 1]; currCol++)
			{
				resultMat.setElement(currRow, colInd[currCol], values[currCol]);
			}
			continue;
		}

		// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS
		else {
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = getElement(currRow, currCol) + inputObject.getElement(currRow, currCol);

				// printf("Adding value %d to %d = %d at (%d, %d)\n", getElement(currRow, currCol), inputObject.getElement(currRow, currCol), value, currRow, currCol);

				resultMat.setElement(currRow, currCol, value);
			}
		}
	}
	printf("Finished adding two matrices \n");
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
	{
		// SKIP EMPTY ROWS FOR BETTER PERFORMANCE ON LARGE MATRICES
		if (rowPtr[currRow] == rowPtr[currRow + 1] && inputObject.rowPtr[currRow] == inputObject.rowPtr[currRow + 1])
		{
			continue;
		}

		// IF THE ROW IN THE FIRST MATRIX IS EMPTY, COPY THE ROW FROM THE SECOND MATRIX
		if (rowPtr[currRow] == rowPtr[currRow + 1])
		{
			for (int currCol = inputObject.rowPtr[currRow]; currCol < inputObject.rowPtr[currRow + 1]; currCol++)
			{
				resultMat.setElement(currRow, inputObject.colInd[currCol], -inputObject.values[currCol]);
			}
			continue;
		}

		// IF THE ROW IN THE SECOND MATRIX IS EMPTY, COPY THE ROW FROM THE FIRST MATRIX
		else if (inputObject.rowPtr[currRow] == inputObject.rowPtr[currRow + 1])
		{
			for (int currCol = rowPtr[currRow]; currCol < rowPtr[currRow + 1]; currCol++)
			{
				resultMat.setElement(currRow, colInd[currCol], values[currCol]);
			}
			continue;
		}

		// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS
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
