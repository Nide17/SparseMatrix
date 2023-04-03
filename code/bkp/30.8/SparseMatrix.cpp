/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"
using namespace std;

// SETTING THE ERROR MESSAGE TO BE DISPLAYED
void errorMessage(const char *msg)
{
	throw std::invalid_argument(msg);
}

// CONSTRUCTOR TO INITIALIZE THE SPARSE MATRIX WITH THE GIVEN NUMBER OF ROWS AND COLUMNS
SparseMatrix::SparseMatrix(int numRows, int numCols)
{
	// CHECKING IF THE NUMBER OF ROWS AND COLUMNS ARE POSITIVE
	if (numRows <= 0 || numCols <= 0)
	{
		errorMessage("Number of rows and columns should be positive");
	}

	// SETTING THE NUMBER OF ROWS AND COLUMNS
	rows = numRows;
	cols = numCols;

	// CREATING AN ARRAY OF BINARY SEARCH TREES WITH SIZE = NUMBER OF ROWS
	bstArray = new BST[rows];
}

// SETTING AND ADDING ELEMENT IN THE MATRIX
int SparseMatrix::setElement(int currRow, int currCol, int value)
{
	// CHECKING IF THE ROW AND COLUMN NUMBER IS WITHIN THE RANGE OF THE MATRIX
	if (currRow < 0 || currRow > rows || currCol < 0 || currCol > cols)
	{
		errorMessage("Row or column number is out of range");
	}

	// CHECKING IF THE VALUE IS ZERO
	if (value == 0)
	{
		return 1;
	}

	// TRAVERSE THE BST CORRESPONDING TO THE ROW NUMBER AND CHECK IF THE ELEMENT IS PRESENT
	Node *currNode = bstArray[currRow].root;

	// CHECKING IF THE BST IS EMPTY
	if (currNode == NULL)
	{
		// CREATING A NEW NODE
		Node *newNode = new Node;
		newNode->row = currRow;
		newNode->col = currCol;
		newNode->value = value;
		newNode->left = NULL;
		newNode->right = NULL;

		// SETTING THE ROOT OF THE BST TO THE NEW NODE
		bstArray[currRow].root = newNode;
	}

	// CHECKING IF THE ELEMENT IS PRESENT IN THE BST
	else
	{
		while (currNode != NULL)
		{
			// FOUND THE ELEMENT
			if (currNode->col == currCol)
			{
				currNode->value = value;
				return 1;
			}

			// CHECKING IF THE ELEMENT IS PRESENT IN THE LEFT SUBTREE OF THE BST
			else if (currNode->col > currCol)
			{
				// CHECKING IF THE LEFT CHILD IS NULL
				if (currNode->left == NULL)
				{
					// CREATING A NEW NODE
					Node *newNode = new Node;
					newNode->row = currRow;
					newNode->col = currCol;
					newNode->value = value;
					newNode->left = NULL;
					newNode->right = NULL;

					// SETTING THE LEFT CHILD OF THE CURRENT NODE TO THE NEW NODE
					currNode->left = newNode;
					break;
				}

				// TRAVERSING THE LEFT SUBTREE OF THE BST
				else
				{
					currNode = currNode->left;
				}
			}

			// CHECKING IF THE ELEMENT IS PRESENT IN THE RIGHT SUBTREE OF THE BST
			else
			{
				// CHECKING IF THE RIGHT CHILD IS NULL
				if (currNode->right == NULL)
				{
					// CREATING A NEW NODE
					Node *newNode = new Node;
					newNode->row = currRow;
					newNode->col = currCol;
					newNode->value = value;
					newNode->left = NULL;
					newNode->right = NULL;

					// SETTING THE RIGHT CHILD OF THE CURRENT NODE TO THE NEW NODE
					currNode->right = newNode;
					break;
				}

				// TRAVERSING THE RIGHT SUBTREE OF THE BST
				else
				{
					currNode = currNode->right;
				}
			}
		}
	}

	return 1;
}

// GETTING THE ELEMENT FROM THE MATRIX AT THE GIVEN ROW AND COLUMN
int SparseMatrix::getElement(int currRow, int currCol)
{
	// CHECKING IF THE ROW AND COLUMN NUMBER IS WITHIN THE RANGE OF THE MATRIX
	if (currRow < 0 || currRow > rows || currCol < 0 || currCol > cols)
	{
		errorMessage("Row or column number is out of range");
	}

	// CHECKING IF THE BST IS EMPTY
	if (bstArray[currRow].root == NULL)
	{
		return 0;
	}

	// FINDING THE ELEMENT USING BINARY SEARCH
	Node *currNode = bstArray[currRow].root;

	while (currNode != NULL)
	{
		// FOUND THE ELEMENT
		if (currNode->col == currCol)
		{
			return currNode->value;
		}

		// CHECKING IF THE ELEMENT IS PRESENT IN THE LEFT SUBTREE OF THE BST
		else if (currNode->col > currCol)
		{
			currNode = currNode->left;
		}

		// CHECKING IF THE ELEMENT IS PRESENT IN THE RIGHT SUBTREE OF THE BST
		else
		{
			currNode = currNode->right;
		}
	}

	return 0;
}

// READING THE MATRIX FROM THE FILE AND STORING IT
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

	// READING THE NUMBER OF ROWS AND COLUMNS FROM THE FILE USING fgets
	int row, col, value;
	char *line = new char[2048];

	// READING THE NUMBER OF ROWS
	fgets(line, 2048, inFileStream);
	sscanf(line, "rows=%d", &row);
	rows = row;

	// READING THE NUMBER OF COLUMNS
	fgets(line, 2048, inFileStream);
	sscanf(line, "cols=%d", &col);
	cols = col;

	// CREATING THE BST ARRAY
	bstArray = new BST[rows];

	// READING THE ELEMENTS FROM THE FILE AND STORING THEM IN THE BST
	while (fgets(line, 2048, inFileStream))
	{
		sscanf(line, "(%d, %d, %d)", &row, &col, &value);
		setElement(row, col, value);

		// UPDATE THE COUNT OF THE NUMBER OF NON-ZERO ELEMENTS IN CORRESPONDING ROW
		bstArray[row].count++;
	}

	printf("Finished reading the file\n");

	// CLOSING THE INPUT FILE
	fclose(inFileStream);

	// DELETING THE LINE
	delete[] line;

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
	printf("Finished writing the file\n");
	fclose(outFileStream);
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
		if (bstArray[currRow].root == NULL && inputObject.bstArray[currRow].root == NULL)
		{
			continue;
		}

		// IF THE ROW IN THE FIRST MATRIX IS EMPTY, COPY THE ROW FROM THE SECOND MATRIX
		if (bstArray[currRow].root == NULL)
		{
			resultMat.bstArray[currRow] = inputObject.bstArray[currRow];
			continue;
		}

		// IF THE ROW IN THE SECOND MATRIX IS EMPTY, COPY THE ROW FROM THE FIRST MATRIX
		if (inputObject.bstArray[currRow].root == NULL)
		{
			resultMat.bstArray[currRow] = bstArray[currRow];
			continue;
		}

		// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS
		else 
		{
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = getElement(currRow, currCol) + inputObject.getElement(currRow, currCol);
				if (value != 0)
				{
					resultMat.setElement(currRow, currCol, value);
				}
			}
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
	{
		// SKIP EMPTY ROWS FOR BETTER PERFORMANCE ON LARGE MATRICES
		if (bstArray[currRow].root == NULL && inputObject.bstArray[currRow].root == NULL)
		{
			continue;
		}

		// IF THE ROW IN THE FIRST MATRIX IS EMPTY, COPY THE ROW FROM THE SECOND MATRIX AND MULTIPLY BY -1
		if (bstArray[currRow].root == NULL)
		{
			resultMat.bstArray[currRow] = inputObject.bstArray[currRow];
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = resultMat.getElement(currRow, currCol);
				if (value != 0)
				{
					resultMat.setElement(currRow, currCol, -1 * value);
				}
			}
			continue;
		}
		

		// IF THE ROW IN THE SECOND MATRIX IS EMPTY, COPY THE ROW FROM THE FIRST MATRIX
		if (inputObject.bstArray[currRow].root == NULL)
		{
			resultMat.bstArray[currRow] = bstArray[currRow];
			continue;
		}

		// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS
		else 
		{
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = getElement(currRow, currCol) - inputObject.getElement(currRow, currCol);
				if (value != 0)
				{
					resultMat.setElement(currRow, currCol, value);
				}
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

	// for (int currRow = 0; currRow < rows; currRow++)
	// {
	// 	for (int currCol = 0; currCol < inputObject.cols; currCol++)
	// 	{
	// 		value = 0;
	// 		for (int colOf1 = 0; colOf1 < cols; colOf1++)
	// 			for (int rowOf2 = 0; rowOf2 < inputObject.rows; rowOf2++)
	// 			{
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
