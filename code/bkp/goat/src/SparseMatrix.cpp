/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"

/**
 * Return the value of the element located at a position in the matrix
 * @param currRow Row of the position whose value is needed.
 * @param currCol Col of the position whose value is needed.
 */
int SparseMatrix::getElement(int currRow, int currCol)
{
	// check that row and column numbers are non-negative
	if (currRow < 0 || currCol < 0 || currRow > rows || currCol > columns)
		throw std::invalid_argument("Invalid row or column number");

	// traverse the binary search tree to find the element
	BSTNode *ptrToNode = arrOfBSTs[currRow].root;

	while (ptrToNode != nullptr)
	{
		// element found in the binary search tree, return the value
		if (ptrToNode->colNumber == currCol)
			return ptrToNode->value;

		// if element is not found, traverse the binary search tree to find the element in the left subtree
		else if (ptrToNode->colNumber > currCol)
			ptrToNode = ptrToNode->left;

		// if element is not found, traverse the binary search tree to find the element in the right subtree
		else
			ptrToNode = ptrToNode->right;
	}

	// if the element is not found, return 0
	return 0;
}

/**
 * Set the value of the element located at a position in the matrix
 * @param currRow Row of the position whose value is needed.
 * @param currCol Col of the position whose value is needed.
 * @param value Value to be set at the position.
 */
// function to set the value of element at row and column specified
int SparseMatrix::setElement(int currRow, int currCol, int value)
{
	// check that row and column numbers are non-negative
	if (currRow > rows || currCol > columns || currCol < 0 || currRow < 0)
	{
		throw std::invalid_argument("Invalid row or column number");
	}

	// helper function for inserting element recursively
	insertElement(arrOfBSTs[currRow].root, currRow, currCol, value);

	return 1;
}

// helper function for inserting element recursively in the binary search tree
void SparseMatrix::insertElement(BSTNode *&node, int currRow, int currCol, int value)
{
	// if the node is null, create a new node and set it as the root of the binary search tree
	if (node == nullptr)
	{
		BSTNode *newNode = new BSTNode;
		newNode->rowNumber = currRow;
		newNode->colNumber = currCol;
		newNode->value = value;
		newNode->left = newNode->right = nullptr;
		node = newNode;
	}
	// if the node is not null, traverse the binary search tree and add the element to the binary search tree
	else if (node->colNumber > currCol)
	{
		insertElement(node->left, currRow, currCol, value);
	}

	// if the node is not null, traverse the binary search tree and add the element to the binary search tree
	else if (node->colNumber < currCol)
	{
		insertElement(node->right, currRow, currCol, value);
	}

	// if the node is not null, traverse the binary search tree and add the element to the binary search tree
	else
	{
		// if the value is 0, do nothing
		if (value == 0)
		{
			return;
		}
		// if the value is not 0, update the value of the element
		else
		{
			node->value = value;
			return;
		}
	}
}

// constructor for the SparseMatrix class that takes the path of the input file as a parameter and loads the input file
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

	// reading the number of rows and columns from the input file and storing them in the rows and columns variables
	int row, col, value;
	char *line = new char[2048];

	// reading the number of rows and columns from the input file using the fscanf function
	fscanf(inFileStream, "rows=%d\n", &rows);
	fscanf(inFileStream, "cols=%d\n", &columns);

	// allocating memory for the array of binary search trees
	arrOfBSTs = new BinarySearchTree[rows];

	// reading the elements from the input file and adding them to the sparse matrix - (0, 0, 2) means that the element at row 0 and column 0 has the value 2
	while (fscanf(inFileStream, "(%d, %d, %d)\n", &row, &col, &value) != EOF)
	{
		// setting the value of the element at row and column specified
		setElement(row, col, value);
	}
}

// function that prints the sparse matrix to the output file
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
	fprintf(outFileStream, "cols=%d\n", columns);
	int value;
	for (int currRow = 0; currRow < rows; currRow++)
		for (int currCol = 0; currCol < columns; currCol++)
		{
			value = getElement(currRow, currCol);
			if (value != 0)
			{
				fprintf(outFileStream, "(%d, %d, %d)\n", currRow, currCol, value);
			}
		}
	fclose(outFileStream);
}

// function that adds the current sparse matrix to the input sparse matrix and returns the result
SparseMatrix SparseMatrix::operator+(SparseMatrix &inputObject)
{
	if (inputObject.rows != rows)
	{
		char *message = new char[2048];
		sprintf(message, "Number of rows are not same");
		throw std::invalid_argument(message);
		delete[] message;
	}
	if (inputObject.columns != columns)
	{
		char *message = new char[2048];
		sprintf(message, "Number of cols are not same");
		throw std::invalid_argument(message);
		delete[] message;
	}

	// creating a new sparse matrix to store the result of the addition
	SparseMatrix resultMat(inputObject.rows, inputObject.columns);

	// looping through the rows of the sparse matrix and adding the corresponding rows of the input sparse matrix
	for (int currRow = 0; currRow < inputObject.rows; currRow++)
	{
		// skipping the row if both the rows are empty in both the matrices
		if (arrOfBSTs[currRow].root == nullptr && inputObject.arrOfBSTs[currRow].root == nullptr)
		{
			continue;
		}

		// if the input matrix row is empty, copy the row from the second matrix
		else if (arrOfBSTs[currRow].root == nullptr)
		{
			resultMat.arrOfBSTs[currRow] = inputObject.arrOfBSTs[currRow];
			continue;
		}

		// if the second matrix row is empty, copy the row from the first matrix
		else if (inputObject.arrOfBSTs[currRow].root == nullptr)
		{
			resultMat.arrOfBSTs[currRow] = arrOfBSTs[currRow];
			continue;
		}

		// if both the rows are not empty, add the corresponding rows of the input sparse matrix
		else
		{
			// looping through the columns of the sparse matrix and adding the corresponding columns of the input sparse matrix
			for (int currCol = 0; currCol < inputObject.columns; currCol++)
			{
				// getting the value of the element from the current sparse matrix
				int value = getElement(currRow, currCol) + inputObject.getElement(currRow, currCol);

				// if the value is not 0, update the value of the element
				if (value != 0)
					resultMat.setElement(currRow, currCol, value);
			}
		}
	}

	// returning the result sparse matrix
	return resultMat;
}

// function that subtracts the input sparse matrix from the current sparse matrix and returns the result
SparseMatrix SparseMatrix::operator-(SparseMatrix &inputObject)
{
	if (inputObject.rows != rows)
	{
		throw std::invalid_argument("Number of rows are not same");
	}
	if (inputObject.columns != columns)
	{
		throw std::invalid_argument("Number of columns are not same");
	}

	// creating a new sparse matrix to store the result of the subtraction
	SparseMatrix resultMat(inputObject.rows, inputObject.columns);

	// looping through the rows of the sparse matrix and subtracting the corresponding rows of the input sparse matrix
	for (int currRow = 0; currRow < inputObject.rows; currRow++)
	{
		// if both the rows are empty in both the matrices, skip the row
		if (arrOfBSTs[currRow].root == NULL && inputObject.arrOfBSTs[currRow].root == NULL)
		{
			continue;
		}

		// if we have empty row in the first matrix, copy the row from the second with the values negated
		if (arrOfBSTs[currRow].root == NULL)
		{
			// copying the row from the second matrix
			resultMat.arrOfBSTs[currRow] = inputObject.arrOfBSTs[currRow];
			for (int currCol = 0; currCol < inputObject.columns; currCol++)
			{
				// get the value of the current element in the second matrix
				int value = resultMat.getElement(currRow, currCol);

				// if the value is not zero, negate it
				if (value != 0)
				{
					resultMat.setElement(currRow, currCol, -value);
				}
			}
			continue;
		}

		// if we have empty row in the second matrix, copy the row from the first
		if (inputObject.arrOfBSTs[currRow].root == NULL)
		{
			resultMat.arrOfBSTs[currRow] = arrOfBSTs[currRow];
			continue;
		}

		// if both the rows are not empty, subtract the corresponding rows of the input sparse matrix
		for (int currCol = 0; currCol < inputObject.columns; currCol++)
		{
			// get the value of the current element in the first matrix
			int value = getElement(currRow, currCol) - inputObject.getElement(currRow, currCol);

			// if the value is not zero, set the value in the result matrix
			if (value != 0)
			{
				resultMat.setElement(currRow, currCol, value);
			}
		}
	}

	// returning the result sparse matrix
	return resultMat;
}

// function that multiplies the current sparse matrix with the input sparse matrix and returns the result
SparseMatrix SparseMatrix::operator*(SparseMatrix &inputObject)
{
	if (columns != inputObject.rows)
	{
		char *message = new char[2048];
		sprintf(message, "Number of columns in first matrix is not equal to number of rows in second matrix");
		throw std::invalid_argument(message);
		delete[] message;
	}

	// creating a new sparse matrix to store the result of the multiplication
	SparseMatrix resultMat(rows, inputObject.columns);

	// looping through the rows of the sparse matrix and multiplying the corresponding rows of the input sparse matrix
	for (int currRow = 0; currRow < rows; currRow++)
	{
		// if both the rows are empty in both the matrices, skip the row
		for (BSTNode *ptrToNode = arrOfBSTs[currRow].root; ptrToNode != NULL; ptrToNode = ptrToNode->right)
		{
			int col1 = ptrToNode->colNumber;

			// looping through the columns of the sparse matrix and multiplying the corresponding columns of the input sparse matrix
			for (BSTNode *ptrToNode2 = inputObject.arrOfBSTs[col1].root; ptrToNode2 != NULL; ptrToNode2 = ptrToNode2->right)
			{
				int col2 = ptrToNode2->colNumber;
				resultMat.setElement(currRow, col2, resultMat.getElement(currRow, col2) + (ptrToNode->value * ptrToNode2->value));
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
