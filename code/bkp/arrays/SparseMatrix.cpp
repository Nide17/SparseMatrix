/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"
using namespace std;
// Task Description:
// 1. Load two sparse matrices from an input file.
// 2. Perform addition, subtraction, and multiplication on the matrices

/**
 * 2) Implement code to:
a) Read a sparse matrix from a file. The format of the file will be:
rows=8433
cols=3180

(0, 381, -694)
(0, 128, -838)
(0, 639, 857)
(0, 165, -933)
(0, 1350, -89)

The first row gives the number of rows. The second row gives the number of
columns. From the third row onwards, there is one entry in parenthesis with row,
column, and integer values separated by commas. All other values in the matrix
will be zero by default. For example, in the given sample, the number of rows is
8433, the number of columns is 3180. Row 0 and column 381 has the value -694.
Row 0 and column 128 has the value -838, and so on.

b) Your goal is to implement a data structure that optimizes both memory and run time
while storing such large matrices. The code needs be implemented in the following
functions of SparseMatrix.h and SparseMatrix.cpp:
SparseMatrix(char* matrixFilePath)
SparseMatrix(int numRows, int numCols)
getElement(int currRow, int currCol)
setElement(int currRow, int currCol, int value)

Your code will be tested to perform operations like addition, subtraction, and
multiplication. These operations are already implemented in the code shared with
you. They make a call to the four functions given in point 

b) above. Refer to lines 47
to 58 of homework.cpp to see how your code will be tested.
e) A few samples of input files and result files are given in zip file.
f) Your code must handle following variations in the input file:
i) If the file has any whitespaces on some lines, these should be ignored.
ii) If the file has the wrong format i.e., different kind of parenthesis, or floating
point values, the code must throw an std::invalid_argument(“Input file has
wrong format”) error and stop.
*/

// Struct to store the matrix elements
struct Element
{
	int row;
	int col;
	int value;
};

// Elements of the matrix
Element *elements;

// Number of elements in the matrix
int numElements;

// Number of rows and columns in the matrix
int rows, cols;

// Constructor to initialize the matrix
SparseMatrix::SparseMatrix(int numRows, int numCols)
{
	rows = numRows;
	cols = numCols;
}

// Function to get the value of a matrix element
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
	
	printf("Reading finished!\n Printing beginning ...\n");
	// print();
}

// Function to set the value of a matrix element at a given row and column 
// It should sort the elements in the matrix in increasing order of row and column
void SparseMatrix::setElement(int currRow, int currCol, int value)
{
	// printf("Setting element (%d, %d) to %d \n", currRow, currCol, value);
	if (currRow >= rows || currCol >= cols || currRow < 0 || currCol < 0)
	{
		char *message = new char[2048];
		sprintf(message, "Index out of bounds while setting!");
		throw std::out_of_range(message);
		delete[] message;
	}

	// If the matrix is empty, add the element
	if (numElements == 0)
	{
		elements = new Element[1]; // Allocate memory for the element
		elements[0].row = currRow;
		elements[0].col = currCol;
		elements[0].value = value;
		numElements++;
		return;
	}

	// If the matrix is not empty, find the position to insert the element
	int insertPos = 0;
	while (insertPos < numElements)
	{
		if (elements[insertPos].row > currRow)
			break;
		else if (elements[insertPos].row == currRow)
		{
			if (elements[insertPos].col > currCol)
				break;
		}
		insertPos++;
	}

	// If the element is already present, update the value
	if (insertPos < numElements)
	{
		if (elements[insertPos].row == currRow && elements[insertPos].col == currCol)
		{
			elements[insertPos].value = value;
			return;
		}
	}

	// If the element is not present, insert it
	Element *newElements = new Element[numElements + 1];
	for (int i = 0; i < insertPos; i++)
	{
		newElements[i].row = elements[i].row;
		newElements[i].col = elements[i].col;
		newElements[i].value = elements[i].value;
	}
	newElements[insertPos].row = currRow;
	newElements[insertPos].col = currCol;
	newElements[insertPos].value = value;
	for (int i = insertPos; i < numElements; i++)
	{
		newElements[i + 1].row = elements[i].row;
		newElements[i + 1].col = elements[i].col;
		newElements[i + 1].value = elements[i].value;
	}

	// sort elements
	for (int i = 0; i < numElements; i++)
	{
		for (int j = i + 1; j < numElements; j++)
		{
			if (newElements[i].row > newElements[j].row)
			{
				Element temp = newElements[i];
				newElements[i] = newElements[j];
				newElements[j] = temp;
			}
			else if (newElements[i].row == newElements[j].row)
			{
				if (newElements[i].col > newElements[j].col)
				{
					Element temp = newElements[i];
					newElements[i] = newElements[j];
					newElements[j] = temp;
				}
			}
		}
	}

	delete[] elements;
	elements = newElements;
	numElements++;
}

// Function to get the value of a matrix element at a given row and column
int SparseMatrix::getElement(int currRow, int currCol)
{
	if (currRow >= rows || currCol >= cols)
	{
		char *message = new char[2048];
		sprintf(message, "Index out of bounds while getting!");
		throw std::out_of_range(message);
		delete[] message;
	}

	// If the matrix is empty, return 0
	if (numElements == 0)
		return 0;

	// If the matrix is not empty, find the element using binary search for row and column 
	int low = 0;
	int high = numElements - 1;
	while (low <= high)
	{
		int mid = (low + high) / 2;
		if (elements[mid].row == currRow && elements[mid].col == currCol)
			return elements[mid].value;
		else if (elements[mid].row < currRow)
			low = mid + 1;
		else if (elements[mid].row == currRow && elements[mid].col < currCol)
			low = mid + 1;
		else
			high = mid - 1;
	}

	// If the element is not present, return 0
	return 0;
}

// Function to print the matrix
void SparseMatrix::print()
{
	printf("rows=%d, cols=%d, numElements=%d\n", rows, cols, numElements);
	for (int i = 0; i < numElements; i++)
		printf("(%d, %d, %d) ", elements[i].row, elements[i].col, elements[i].value);
	printf("\n");
}

// Function to print the matrix to an ASCII file
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
	printf("print finished\n");
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
