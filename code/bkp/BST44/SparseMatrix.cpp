/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"
using namespace std;

// SETTING THE ERROR MESSAGE TO BE DISPLAYED
void errorMessage(const char *msg)
{
	throw invalid_argument(msg);
}

// FUNCTION TO CREATE A NODE
Node *createNode(int currRow, int currCol, int value)
{
	// CREATING A NEW NODE
	Node *data = new Node;
	data->row = currRow;
	data->col = currCol;
	data->value = value;
	data->left = NULL;
	data->right = NULL;

	// RETURNING THE NEW NODE
	return data;
}

// CONSTRUCTOR TO INITIALIZE THE SPARSE MATRIX WITH THE GIVEN NUMBER OF ROWS AND COLUMNS
SparseMatrix::SparseMatrix(int numRows, int numCols)
{
	// CHECKING IF THE NUMBER OF ROWS AND COLUMNS ARE POSITIVE, NOT EXCEEDING THE MAXIMUM LIMIT AND NOT ZERO
	if (numRows <= 0 || numCols <= 0)
	{
		errorMessage("Number of rows and columns is incorrect!");
	}

	// SETTING THE NUMBER OF ROWS AND COLUMNS OF THE MATRIX TO THE GIVEN VALUES OF THE PARAMETERS
	rows = numRows;
	cols = numCols;

	// CREATING AN ARRAY OF BINARY SEARCH TREES WITH SIZE = NUMBER OF ROWS OF THE MATRIX
	// TO STORE NON-ZERO ELEMENTS WITH THE SAME ROW NUMBER IN A SORTED ORDER IN THE BINARY SEARCH TREE
	treesArr = new BSTree[rows];
}

// SETTING AND ADDING ELEMENT IN THE MATRIX
int SparseMatrix::setElement(int currRow, int currCol, int value)
{
	// CHECKING IF THE ROW AND COLUMN NUMBER IS WITHIN THE RANGE OF THE MATRIX
	if (currRow < 0 || currRow > rows || currCol < 0 || currCol > cols)
	{
		errorMessage("Row or column number is out of range");
	}

	// CHECKING IF THE VALUE IS ZERO AND RETURN 1
	if (value == 0)
	{
		return 1;
	}

	// TRAVERSE THE BST CORRESPONDING TO THE ROW NUMBER AND CHECK IF THE ELEMENT IS PRESENT
	Node *currentNode = treesArr[currRow].root;

	// IF THE BST IS EMPTY OR THE ELEMENT IS NOT PRESENT IN THE BST (BASE CASE), THEN ADD THE ELEMENT TO THE BST
	if (currentNode == NULL)
	{

		// ADDING THE NEW NODE TO THE BST BY SETTING THE ROOT OF THE BST TO THE NEW NODE
		treesArr[currRow].root = createNode(currRow, currCol, value);
	}

	// CHECKING IF THE ELEMENT IS PRESENT IN THE BST
	else
	{
		while (currentNode != NULL)
		{
			// IF THE ELEMENT IS AT THE CURRENT NODE, THEN UPDATE THE VALUE OF THE ELEMENT
			if (currentNode->col == currCol)
			{
				currentNode->value = value;
				return 1;
			}

			// IF THE COLUMN NUMBER OF THE ELEMENT IS LESS THAN THE COLUMN NUMBER OF THE CURRENT NODE,
			// THEN TRAVERSE TO THE LEFT CHILD
			else if (currentNode->col > currCol)
			{
				// IF THE LEFT CHILD IS NULL, THEN ADD THE ELEMENT TO THE LEFT CHILD
				if (currentNode->left == NULL)
				{

					// SETTING THE LEFT CHILD OF THE CURRENT NODE TO THE NEW NODE
					currentNode->left = createNode(currRow, currCol, value);
					return 1;
				}

				// TRAVERSING TO THE LEFT CHILD
				else
				{
					currentNode = currentNode->left;
				}
			}

			// ADD THE ELEMENT TO THE RIGHT DEPENDING ON THE COLUMN NUMBER IN ORDER TO BE SORTED BINARLY
			else
			{
				// CHECKING IF THE RIGHT CHILD IS NULL
				if (currentNode->right == NULL)
				{
					// SETTING THE RIGHT CHILD OF THE CURRENT NODE TO THE NEW NODE
					currentNode->right = createNode(currRow, currCol, value);
					return 1;
				}

				// TRAVERSING TO THE RIGHT CHILD
				else
				{
					currentNode = currentNode->right;
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
	if (treesArr[currRow].root == NULL)
	{
		return 0;
	}

	// FINDING THE ELEMENT USING BINARY SEARCH
	Node *currentNode = treesArr[currRow].root;

	while (currentNode != NULL)
	{
		// FOUND THE ELEMENT AT THE CURRENT NODE AND RETURNING THE VALUE OF THE ELEMENT
		if (currentNode->col == currCol)
		{
			return currentNode->value;
		}

		// CHECKING IF THE ELEMENT IS PRESENT IN THE LEFT SUBTREE OF THE BST
		else if (currentNode->col > currCol)
		{
			currentNode = currentNode->left;
		}

		// CHECKING IF THE ELEMENT IS PRESENT IN THE RIGHT SUBTREE OF THE BST
		else
		{
			currentNode = currentNode->right;
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
		throw ios_base::failure(message);
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
	treesArr = new BSTree[rows];

	// READING THE ELEMENTS FROM THE FILE AND STORING THEM IN THE BST
	while (fgets(line, 2048, inFileStream))
	{
		sscanf(line, "(%d, %d, %d)", &row, &col, &value);
		setElement(row, col, value);
	}

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
		throw ios_base::failure(message);
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

// ADDING THE TWO MATRICES AND RETURNING THE RESULT
SparseMatrix SparseMatrix::operator+(SparseMatrix &inputObject)
{
	if (rows != inputObject.rows) // CHECKING IF THE NUMBER OF ROWS ARE EQUAL
	{
		char *message = new char[2048];
		sprintf(message, "Number of rows are not same");
		throw invalid_argument(message);
		delete[] message;
	}

	if (cols != inputObject.cols) // CHECKING IF THE NUMBER OF COLUMNS ARE EQUAL
	{
		char *message = new char[2048];
		sprintf(message, "Number of cols are not same");
		throw invalid_argument(message);
		delete[] message;
	}

	// CREATING THE RESULT MATRIX OBJECT TO STORE THE RESULT OF ADDITION OF THE TWO MATRICES
	SparseMatrix resultMat(inputObject.rows, inputObject.cols);
	int value;

	// ADDING THE TWO MATRICES AND STORING THE RESULT IN THE RESULT MATRIX OBJECT CREATED
	for (int currRow = 0; currRow < inputObject.rows; currRow++)
	{
		// SKIP EMPTY ROWS FOR FASTER ADDITION ON LARGE MATRICES (IF BOTH ROWS ARE EMPTY)
		if (treesArr[currRow].root == NULL && inputObject.treesArr[currRow].root == NULL)
		{
			continue;
		}

		// IF THE ROW IN THE FIRST MATRIX IS EMPTY, COPY THE ROW FROM THE SECOND MATRIX TO THE RESULT MATRIX
		else if (treesArr[currRow].root == NULL)
		{
			resultMat.treesArr[currRow] = inputObject.treesArr[currRow];
			continue;
		}

		// IF THE ROW IN THE SECOND MATRIX IS EMPTY, COPY THE ROW FROM THE FIRST MATRIX TO THE RESULT MATRIX
		else if (inputObject.treesArr[currRow].root == NULL)
		{
			resultMat.treesArr[currRow] = treesArr[currRow];
			continue;
		}

		// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS AND STORE THE RESULT IN THE RESULT MATRIX
		else
		{
			// ADDING THE TWO ROWS AND STORING THE RESULT IN THE RESULT MATRIX
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				// ADD THE CORRESPONDING ELEMENTS FROM THE TWO ROWS
				value = getElement(currRow, currCol) + inputObject.getElement(currRow, currCol);

				// IF THE RESULT IS NOT ZERO, STORE IT IN THE RESULT MATRIX
				if (value != 0)
				{
					// SET THE ELEMENT IN THE RESULT MATRIX
					resultMat.setElement(currRow, currCol, value);
				}
			}
		}
	}

	// RETURN THE RESULT MATRIX
	return resultMat;
}

// SUBTRACTING THE TWO MATRICES AND STORING THE RESULT IN THE RESULT MATRIX OBJECT CREATED
SparseMatrix SparseMatrix::operator-(SparseMatrix &inputObject)
{
	if (inputObject.rows != rows) // CHECKING IF THE NUMBER OF ROWS ARE EQUAL
	{
		char *message = new char[2048];
		sprintf(message, "Number of rows are not same");
		throw invalid_argument(message);
		delete[] message;
	}

	if (inputObject.cols != cols) // CHECKING IF THE NUMBER OF COLUMNS ARE EQUAL
	{
		char *message = new char[2048];
		sprintf(message, "Number of cols are not same");
		throw invalid_argument(message);
		delete[] message;
	}

	// CREATING THE RESULT MATRIX OBJECT TO STORE THE RESULT OF SUBTRACTION OF THE TWO MATRICES
	SparseMatrix resultMat(rows, cols);
	int value;

	// SUBTRACTING THE TWO MATRICES AND STORING THE RESULT IN THE RESULT MATRIX OBJECT CREATED
	for (int currRow = 0; currRow < rows; currRow++)
	{
		Node *ptrToNode = treesArr[currRow].root;			   // POINTING TO THE ROOT OF THE FIRST ROW
		Node *ptrToNode2 = inputObject.treesArr[currRow].root; // POINTING TO THE ROOT OF THE SECOND ROW

		// IF BOTH ROWS ARE EMPTY, SKIP THE ROW (FOR FASTER SUBTRACTION ON LARGE MATRICES)
		while (ptrToNode != NULL || ptrToNode2 != NULL)
		{
			// GET THE COLUMN NUMBER OF THE FIRST ELEMENT IN THE ROW
			int col1 = (ptrToNode != NULL) ? ptrToNode->col : cols;

			// GET THE COLUMN NUMBER OF THE SECOND ELEMENT IN THE ROW
			int col2 = (ptrToNode2 != NULL) ? ptrToNode2->col : cols;

			// IF THE FIRST ROW HAS MORE ELEMENTS THAN THE SECOND ROW
			if (col1 < col2)
			{
				// SET THE ELEMENT IN THE RESULT MATRIX
				resultMat.setElement(currRow, col1, ptrToNode->value);

				// MOVE TO THE NEXT ELEMENT IN THE ROW
				ptrToNode = ptrToNode->right;
			}

			// IF THE SECOND ROW HAS MORE ELEMENTS THAN THE FIRST ROW
			else if (col2 < col1)
			{
				// SET THE ELEMENT IN THE RESULT MATRIX
				resultMat.setElement(currRow, col2, -1 * ptrToNode2->value);

				// MOVE TO THE NEXT ELEMENT IN THE ROW
				ptrToNode2 = ptrToNode2->right;
			}

			// IF THE TWO ROWS HAVE THE SAME NUMBER OF ELEMENTS
			else
			{
				// ADD THE CORRESPONDING ELEMENTS FROM THE TWO ROWS
				value = ptrToNode->value - ptrToNode2->value;

				// IF THE RESULT IS NOT ZERO, STORE IT IN THE RESULT MATRIX
				if (value != 0)
				{
					// SET THE ELEMENT IN THE RESULT MATRIX
					resultMat.setElement(currRow, col1, value);
				}

				// MOVE TO THE NEXT ELEMENT IN THE ROW FOR THE 1ST MATRIX
				ptrToNode = ptrToNode->right;

				// MOVE TO THE NEXT ELEMENT IN THE ROW FOR THE 2ND MATRIX
				ptrToNode2 = ptrToNode2->right;
			}
		}
	}

	// RETURN THE RESULT MATRIX
	return resultMat;
}

// MULTIPLYING THE TWO MATRICES AND STORING THE RESULT IN THE RESULT MATRIX OBJECT CREATED
SparseMatrix SparseMatrix::operator*(SparseMatrix &inputObject)
{
	if (inputObject.rows != cols) // CHECKING IF ROWS IN THE SECOND MATRIX IS EQUAL TO COLUMNS IN THE FIRST MATRIX
	{
		char *message = new char[2048];
		sprintf(message, "Input does not satisfy following condition: Number of rows in second matrix must be equal to num of cols in first matrix");
		throw invalid_argument(message);
		delete[] message;
	}

	if (inputObject.cols != rows) // CHECKING IF COLUMNS IN THE SECOND MATRIX IS EQUAL TO ROWS IN THE FIRST MATRIX
	{
		char *message = new char[2048];
		sprintf(message, "Input does not satisfy following condition: Number of cols in second matrix must be equal to num of rows in first matrix");
		throw invalid_argument(message);
		delete[] message;
	}

	/**
	 * In matrix multiplication, number of rows in the result is equal to number of rows in first matrix.
	 * number of cols in the result is equal to number of cols in first matrix.
	 */

	// CREATING THE RESULT MATRIX OBJECT TO STORE THE RESULT OF MULTIPLICATION OF THE TWO MATRICES
	SparseMatrix resultMat(rows, inputObject.cols);
	int value;

	// ITERATING OVER THE NON-ZERO ELEMENTS OF THE FIRST MATRIX
	for (int i = 0; i < rows; i++)
	{
		Node *currentNode = treesArr[i].root;

		// ITERATING OVER THE NON-ZERO ELEMENTS OF THE FIRST MATRIX ROW BY ROW
		while (currentNode != NULL)
		{
			// COL OF THE CURRENT NODE OF THE FIRST MATRIX
			int j = currentNode->col;

			// ITERATING OVER THE NON-ZERO ELEMENTS OF THE SECOND MATRIX
			Node *currentNode2 = inputObject.treesArr[j].root;

			// ITERATING OVER THE NON-ZERO ELEMENTS OF THE SECOND MATRIX ROW BY ROW
			while (currentNode2 != NULL)
			{
				// COL OF THE CURRENT NODE OF THE SECOND MATRIX
				int k = currentNode2->col;

				// MULTIPLYING THE CORRESPONDING ELEMENTS FROM THE TWO ROWS
				value = resultMat.getElement(i, k) + (currentNode->value * currentNode2->value);

				// IF THE RESULT IS NOT ZERO, STORE IT IN THE RESULT MATRIX
				if (value != 0)
				{
					// SET THE ELEMENT IN THE RESULT MATRIX
					resultMat.setElement(i, k, value);
				}

				// MOVING TO THE NEXT NODE OF THE SECOND MATRIX ROW
				currentNode2 = currentNode2->right;
			}

			// MOVING TO THE NEXT NODE OF THE FIRST MATRIX ROW
			currentNode = currentNode->right;
		}
	}

	// RETURN THE RESULT MATRIX
	return resultMat;
}

void SparseMatrixTester::generateTestCases(char *outputFolderPath)
{
}

void SparseMatrixTester::generateTestCases(char *outputFilePath, int min, int max, int numRows, int numCols)
{
}
