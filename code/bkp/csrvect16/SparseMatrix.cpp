/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"
using namespace std;

void errorMessage(const char *msg)
{
	throw std::invalid_argument(msg);
}

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

	// ALLOCATE MEMORY FOR ARRAYS
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

	// i) If the file has any whitespaces on some lines, these should be ignored.
	// ii) If the file has the wrong format i.e., different kind of parenthesis, or floating
	// point values, the code must throw an std::invalid_argument(“Input file has wrong format”) error and stop execution.
	if (!infile.is_open() || infile.peek() == std::ifstream::traits_type::eof() || infile.peek() == ' ' || !infile)
	{
		errorMessage("Input file has wrong format");
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
	int nbrCount = 0;

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

		// ADD THE ELEMENT TO THE VECTOR
		elements.push_back({row, col, value});
		nbrCount++;
	}
	// SparseMatrix::print();

	// SORT THE VECTOR OF ELEMENTS
	SparseMatrix::sortElements();

	// ADD THE VECTOR OF ELEMENTS TO THE CSR MATRIX
	for (int i = 0; i < elements.size(); i++)
	{
		SparseMatrix::setElement(elements[i].row, elements[i].col, elements[i].value);
	}

	// DELETE THE VECTOR OF ELEMENTS
	elements.clear();

	// INITIALIZE THE CSR MATRIX
	SparseMatrix(rows, cols, nbrCount);

	// CLOSE THE FILE
	infile.close();

	// FILL THE LAST ROW POINTER IN THE MATRIX TO AVOID SEGMENTATION FAULT - OUT OF BOUNDS
	rowPtr[rows] = nbrCount;
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
	fclose(outFileStream);
}

SparseMatrix SparseMatrix::operator+(SparseMatrix &inputObject)
{
	if (inputObject.rows != rows || inputObject.cols != cols)
	{
		errorMessage("Input file has wrong format");
	}

	SparseMatrix resultMat(inputObject.rows, inputObject.cols);
	int value;

	for (int currRow = 0; currRow < inputObject.rows; currRow++)
	{
		// SKIP EMPTY ROWS FOR BETTER PERFORMANCE ON LARGE MATRICES
		if (rowPtr[currRow] == rowPtr[currRow + 1] && inputObject.rowPtr[currRow] == inputObject.rowPtr[currRow + 1])
		{
			continue; // SKIP THE ADDITION OF THE TWO ROWS
		}

		// IF THE ROW IN THE FIRST MATRIX IS EMPTY, COPY THE ROW FROM THE SECOND MATRIX
		if (rowPtr[currRow] == rowPtr[currRow + 1])
		{
			for (int currCol = inputObject.rowPtr[currRow]; currCol < inputObject.rowPtr[currRow + 1]; currCol++)
			{
				resultMat.setElement(currRow, inputObject.colInd[currCol], inputObject.values[currCol]);
			}
		}

		// IF THE ROW IN THE SECOND MATRIX IS EMPTY, COPY THE ROW FROM THE FIRST MATRIX
		else if (inputObject.rowPtr[currRow] == inputObject.rowPtr[currRow + 1])
		{
			for (int currCol = rowPtr[currRow]; currCol < rowPtr[currRow + 1]; currCol++)
			{
				resultMat.setElement(currRow, colInd[currCol], values[currCol]);
			}
		}

		// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS
		else
		{
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				// AVOID ADDITION THAT CAN RESULT IN AN OVERFLOW
				if (getElement(currRow, currCol) > 0 && inputObject.getElement(currRow, currCol) > INT_MAX - getElement(currRow, currCol) || getElement(currRow, currCol) < 0 && inputObject.getElement(currRow, currCol) < INT_MIN - getElement(currRow, currCol))
				{
					errorMessage("Input file has wrong format");
				}

				value = getElement(currRow, currCol) + inputObject.getElement(currRow, currCol);

				// SKIP ZERO ELEMENTS AND THOSE OUT OF INT RANGE
				if (value == 0)
				{
					continue;
				}

				// SET THE RESULTING ELEMENT
				resultMat.setElement(currRow, currCol, value);
			}
		}
	}

	return resultMat;
}

SparseMatrix SparseMatrix::operator-(SparseMatrix &inputObject)
{
	if (inputObject.rows != rows || inputObject.cols != cols)
	{
		errorMessage("Input file has wrong format");
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
		}

		// IF THE ROW IN THE SECOND MATRIX IS EMPTY, COPY THE ROW FROM THE FIRST MATRIX
		else if (inputObject.rowPtr[currRow] == inputObject.rowPtr[currRow + 1])
		{
			for (int currCol = rowPtr[currRow]; currCol < rowPtr[currRow + 1]; currCol++)
			{
				resultMat.setElement(currRow, colInd[currCol], values[currCol]);
			}
		}

		// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS
		else
		{
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				// AVOID SUBTRACTION THAT CAN RESULT IN AN OVERFLOW
				if (getElement(currRow, currCol) > 0 && inputObject.getElement(currRow, currCol) < INT_MIN + getElement(currRow, currCol) || getElement(currRow, currCol) < 0 && inputObject.getElement(currRow, currCol) > INT_MAX + getElement(currRow, currCol))
				{
					errorMessage("Input file has wrong format");
				}

				value = getElement(currRow, currCol) - inputObject.getElement(currRow, currCol);
				// SKIP ZERO ELEMENTS AND THOSE OUT OF INT RANGE
				if (value == 0)
				{
					continue;
				}
				resultMat.setElement(currRow, currCol, value);
			}
		}
	}
	return resultMat;
}

SparseMatrix SparseMatrix::operator*(SparseMatrix &inputObject)
{
	if (inputObject.rows != cols || inputObject.cols != rows)
	{
		errorMessage("Input does not satisfy following condition: Number of rows in second matrix must be equal to num of cols in first matrix");
	}
	/**
	 * In matrix multiplication, number of rows in the result is equal to number of rows in first matrix.
	 * number of cols in the result is equal to number of cols in first matrix.
	 */
	SparseMatrix resultMat(rows, inputObject.rows);
	int value;

	if (cols != inputObject.rows)
	{
		throw std::invalid_argument("Matrix dimensions do not match for multiplication");
	}

	// Create a transposed copy of the second matrix to improve cache performance
	SparseMatrix inputObjectTransposed(inputObject.cols, inputObject.rows);

	for (int i = 0; i < inputObject.rows; i++)
	{
		for (int j = inputObject.rowPtr[i]; j < inputObject.rowPtr[i + 1]; j++)
		{
			int col = inputObject.colInd[j];
			int val = inputObject.values[j];
			inputObjectTransposed.setElement(col, i, val);
		}
	}

	// Perform matrix multiplication
	for (int i = 0; i < rows; i++)
	{
		for (int j = rowPtr[i]; j < rowPtr[i + 1]; j++)
		{
			int col = colInd[j];
			int val = values[j];
			for (int k = inputObjectTransposed.rowPtr[col]; k < inputObjectTransposed.rowPtr[col + 1]; k++)
			{
				int col2 = inputObjectTransposed.colInd[k];
				int val2 = inputObjectTransposed.values[k];
				resultMat.setElement(i, col2, resultMat.getElement(i, col2) + val * val2);
			}
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
