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
using namespace std;

// USING ARRAY OF RED BALCK TREES TO REPRESENT A SPARSE MATRIX
enum Color
{
	RED,
	BLACK
};

// NODE CLASS FOR NODE OF THE RED BLACK TREE
class Node
{
public:
	int row;
	int col;
	int value;
	bool color;
	Node *left, *right, *parent;

	// CONSTRUCTOR
	Node(int row, int col, int value)
	{
		this->row = row;
		this->col = col;
		this->value = value;
		left = right = parent = NULL;
		this->color = RED;
	}
};

// CLASS FOR RED BLACK TREE
class RBTree
{
public:
	Node *root;
	// CONSTRUCTOR
	RBTree();

	void rotateLeft(Node *&, Node *&);
	void rotateRight(Node *&, Node *&);
	void fixViolation(Node *&, Node *&);

	// INSERT THE NODE
	void insert(const int &row, const int &col, const int &value);

	// INSERTION HELPER FUNCTION
	Node *BSTInsert(Node *, Node *);

	// GET THE NODE
	Node *search(const int &row, const int &col);

	// TRAVERSE THE RBT IN INORDER
	// void inorder(Node *root);
};

/**
 * Implement basic functions on a sparse matrix that contains integers.
 *
 * The default value in each element of the matrix is assumed to be 0.
 */
// CLASS FOR SPARSE MATRIX
class SparseMatrix
{
private:
	int rows, cols;

	// USING ARRAY OF RED BLACK TREES TO REPRESENT A SPARSE MATRIX
	// EACH RED BLACK TREE REPRESENTS A ROW OF THE SPARSE MATRIX
	RBTree *matrix;

public:
	/**
	 * Given an input text file, load the matrix values into the matrix dta structure.
	 *
	 * @param matrixFilePath Path of the file which contains the data to create a matrix.
	 *
	 * If the input file cannot be read throw an error of type ios_base::failure
	 */
	SparseMatrix(char *matrixFilePath);

	/**
	 * Create a sparse matrix that contains numRows and numCols elements.
	 * Since this is a sparse matrix, all values will be zeros unless filled.
	 */
	SparseMatrix(int numRows, int numCols);
	/**
	 * Print the matrix to an output file.
	 */
	void printToASCIIFile(char *outputFileName);

	/**
	 * Return the value of the element located at a position in the matrix
	 * @param currRow Row of the position whose value is needed.
	 * @param currCol Col of the position whose value is needed.
	 */
	int getElement(int currRow, int currCol);

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
	int setElement(int currRow, int currCol, int value);

	SparseMatrix operator+(SparseMatrix &inputObject);
	SparseMatrix operator-(SparseMatrix &inputObject);
	SparseMatrix operator*(SparseMatrix &inputObject);
	// ~SparseMatrix();
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
/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"
using namespace std;

// IMPLEMENTING RBT OPERATIONS FOR INSERTION, INORDER TRAVERSAL, LEFT & RIGHT ROTATION, FIX VIOLATION
// RBT CONSTRUCTOR
RBTree::RBTree()
{
	root = NULL;
}

// RBT INSERTION
void RBTree::insert(const int &row, const int &col, const int &value)
{
	Node *newNode = new Node(row, col, value);
	root = BSTInsert(root, newNode);
	fixViolation(root, newNode);
}

// RBT INSERTION HELPER FUNCTION
Node *RBTree::BSTInsert(Node *root, Node *newNode)
{
	if (root == NULL)
		return newNode;

	if (newNode->row < root->row)
	{
		root->left = BSTInsert(root->left, newNode);
		root->left->parent = root;
	}
	else if (newNode->row > root->row)
	{
		root->right = BSTInsert(root->right, newNode);
		root->right->parent = root;
	}
	else if (newNode->col < root->col)
	{
		root->left = BSTInsert(root->left, newNode);
		root->left->parent = root;
	}
	else if (newNode->col > root->col)
	{
		root->right = BSTInsert(root->right, newNode);
		root->right->parent = root;
	}
	else
	{
		root->value = newNode->value;
	}
	return root;
}

// RBT LEFT ROTATION
void RBTree::rotateLeft(Node *&root, Node *&newNode)
{
	Node *newNodeRight = newNode->right;

	newNode->right = newNodeRight->left;

	if (newNode->right != NULL)
		newNode->right->parent = newNode;

	newNodeRight->parent = newNode->parent;

	if (newNode->parent == NULL)
		root = newNodeRight;

	else if (newNode == newNode->parent->left)
		newNode->parent->left = newNodeRight;

	else
		newNode->parent->right = newNodeRight;

	newNodeRight->left = newNode;
	newNode->parent = newNodeRight;
}

// RBT RIGHT ROTATION
void RBTree::rotateRight(Node *&root, Node *&newNode)
{
	Node *newNodeLeft = newNode->left;

	newNode->left = newNodeLeft->right;

	if (newNode->left != NULL)
		newNode->left->parent = newNode;

	newNodeLeft->parent = newNode->parent;

	if (newNode->parent == NULL)
		root = newNodeLeft;

	else if (newNode == newNode->parent->left)
		newNode->parent->left = newNodeLeft;

	else
		newNode->parent->right = newNodeLeft;

	newNodeLeft->right = newNode;
	newNode->parent = newNodeLeft;
}

// RBT FIX VIOLATION
void RBTree::fixViolation(Node *&root, Node *&newNode)
{
	Node *parentPtr = NULL;
	Node *grandParentPtr = NULL;

	while ((newNode != root) && (newNode->color != BLACK) &&
		   (newNode->parent->color == RED))
	{

		parentPtr = newNode->parent;
		grandParentPtr = newNode->parent->parent;

		/*  Case : A
			Parent of newNode is left child of Grand-parent of newNode */
		if (parentPtr == grandParentPtr->left)
		{

			Node *unclePtr = grandParentPtr->right;

			/* Case : 1
			   The uncle of newNode is also red
			   Only Recoloring required */
			if (unclePtr != NULL && unclePtr->color == RED)
			{
				grandParentPtr->color = RED;
				parentPtr->color = BLACK;
				unclePtr->color = BLACK;
				newNode = grandParentPtr;
			}

			else
			{
				/* Case : 2
			   newNode is right child of its parent
			   Left-rotation required */
				if (newNode == parentPtr->right)
				{
					rotateLeft(root, parentPtr);
					newNode = parentPtr;
					parentPtr = newNode->parent;
				}

				/* Case : 3
			   newNode is left child of its parent
			   Right-rotation required */
				rotateRight(root, grandParentPtr);
				std::swap(parentPtr->color, grandParentPtr->color);
				newNode = parentPtr;
			}
		}

		/* Case : B
		   Parent of newNode is right child of Grand-parent of newNode */
		else
		{
			Node *unclePtr = grandParentPtr->left;

			/*  Case : 1
				The uncle of newNode is also red
				Only Recoloring required */
			if ((unclePtr != NULL) && (unclePtr->color == RED))
			{
				grandParentPtr->color = RED;
				parentPtr->color = BLACK;
				unclePtr->color = BLACK;
				newNode = grandParentPtr;
			}
			else
			{
				/* Case : 2
			   newNode is left child of its parent
			   Right-rotation required */
				if (newNode == parentPtr->left)
				{
					rotateRight(root, parentPtr);
					newNode = parentPtr;
					parentPtr = newNode->parent;
				}

				/* Case : 3
			   newNode is right child of its parent
			   Left-rotation required */
				rotateLeft(root, grandParentPtr);
				std::swap(parentPtr->color, grandParentPtr->color);
				newNode = parentPtr;
			}
		}
	}

	root->color = BLACK;
}

// RBT SEARCH
Node *RBTree::search(const int &row, const int &col)
{
	Node *current = root;
	while (current != NULL)
	{
		// check if node matches target
		if (current->row == row && current->col == col)
			return current;

		// binary search within current level
		if (current->row < row || (current->row == row && current->col < col))
		{
			current = current->right;
		}
		else
		{
			current = current->left;
		}
	}

	return NULL; // target not found
}

// USING ARRAY OF RED BLACK TREES TO REPRESENT A SPARSE MATRIX
SparseMatrix::SparseMatrix(int rows, int cols)
{
	this->rows = rows;
	this->cols = cols;
	this->matrix = new RBTree[rows];
}

// GET ELEMENT FROM SparseMatrix
/**
 * Return the value of the element located at a position in the matrix
 * @param currRow Row of the position whose value is needed.
 * @param currCol Col of the position whose value is needed.
 */
int SparseMatrix::getElement(int currRow, int currCol)
{
	if (currRow < 0 || currRow > this->rows || currCol < 0 || currCol > this->cols)
	{
		char *message = new char[2048];
		sprintf(message, "Invalid row or column index");
		throw std::out_of_range(message);
		delete[] message;
	}

	Node *node = this->matrix[currRow].search(currRow, currCol);
	if (node == NULL)
		return 0;
	return node->value;
}

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
// SET ELEMENT IN SparseMatrix
int SparseMatrix::setElement(int currRow, int currCol, int value)
{
	if (currRow < 0 || currRow > this->rows || currCol < 0 || currCol > this->cols)
	{
		char *message = new char[2048];
		sprintf(message, "Invalid row or column index");
		throw std::out_of_range(message);
		delete[] message;
	}

	Node *node = this->matrix[currRow].search(currRow, currCol);

	if (node == NULL)
	{
		this->matrix[currRow].insert(currRow, currCol, value);
		return 1;
	}
	node->value = value;
	return 1;
}

// CONSTRUCTOR TO LOAD A SPARSE MATRIX FROM A FILE IN THE FOLLOWING FORMAT
// FIRST LINE: cols=n, SECOND: rows=n, FOLLOWING LINES: row, col, value (0, 0, 10)
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

	char *line = new char[2048];
	int row, col, value;
	fgets(line, 2048, inFileStream);
	sscanf(line, "rows=%d", &rows);

	fgets(line, 2048, inFileStream);
	sscanf(line, "cols=%d", &cols);

	matrix = new RBTree[rows];

	while (fgets(line, 2048, inFileStream))
	{
		sscanf(line, "(%d, %d, %d)", &row, &col, &value);
		setElement(row, col, value);
	}

	printf("Finished reading the file\n");

	delete[] line;
	fclose(inFileStream);
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
	{
		for (int currCol = 0; currCol < cols; currCol++)
		{
			value = getElement(currRow, currCol);
			if (value != 0)
			{
				fprintf(outFileStream, "(%d, %d, %d)\n", currRow, currCol, value);
			}
		}
	}
}

SparseMatrix SparseMatrix::operator+(SparseMatrix &inputObject)
{
	printf("Adding two matrices\n");
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
		if (inputObject.matrix[currRow].root == NULL && matrix[currRow].root == NULL)
			continue;

		// IF A ROW IS EMPTY FOR ANY OF THE MATRICES, RETURN THE ONE WHICH IS NOT EMPTY
		else if (matrix[currRow].root == NULL)
		{
			resultMat.matrix[currRow] = inputObject.matrix[currRow];
			continue;
		}

		// IF ROW IS EMPTY FOR SECOND MATRIX, RETURN THE FIRST MATRIX
		else if (inputObject.matrix[currRow].root == NULL)
		{
			resultMat.matrix[currRow] = matrix[currRow];
			continue;
		}

		// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS VALUES
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
		if (inputObject.matrix[currRow].root == NULL && matrix[currRow].root == NULL)
			continue;

		// IF A ROW IS EMPTY FOR ANY OF THE MATRICES, RETURN THE ONE WHICH IS NOT EMPTY
		else if (matrix[currRow].root == NULL)
		{
			// NEGATE THE VALUES OF THE SECOND MATRIX
			for (int currCol = 0; currCol < inputObject.cols; currCol++)
			{
				value = -1 * inputObject.getElement(currRow, currCol);
				if (value != 0)
				{
					resultMat.setElement(currRow, currCol, value);
				}
			}
		}

		// IF ROW IS EMPTY FOR SECOND MATRIX, RETURN THE FIRST MATRIX
		else if (inputObject.matrix[currRow].root == NULL)
		{
			resultMat.matrix[currRow] = matrix[currRow];
			continue;
		}

		// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS VALUES
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

	// ITERATE THROUGH EACH ROW OF THE FIRST MATRIX AND MULTIPLY WITH EACH COLUMN OF THE SECOND MATRIX
	for (int currRow = 0; currRow < rows; currRow++)
	{
		// CHECK FOR EMPTY ROWS IN THE FIRST MATRIX TO AVOID UNNECESSARY COMPUTATION
		if (matrix[currRow].root == NULL)
			continue;

		// ITERATE THROUGH EACH COLUMN OF THE SECOND MATRIX TO COMPUTE THE RESULT FOR THE CURRENT ROW OF THE FIRST MATRIX
		for (int currCol = 0; currCol < inputObject.cols; currCol++)
		{
			if (matrix[currRow].root == NULL)
			{
				continue;
			}
			value = 0;

			// ITERATE THROUGH EACH ROW OF THE SECOND MATRIX TO COMPUTE THE RESULT FOR THE CURRENT ROW OF THE FIRST MATRIX
			for (int rowOf2 = 0; rowOf2 < inputObject.rows; rowOf2++)
			{
				int colOf1 = rowOf2;

				// COMPUTE THE RESULT ONLY IF THE ELEMENTS IN THE CURRENT ROW AND CURRENT COLUMN ARE NOT ZERO
				if (getElement(currRow, rowOf2) != 0 && inputObject.getElement(rowOf2, currCol) != 0)
				{
					value += getElement(currRow, colOf1) * inputObject.getElement(rowOf2, currCol);
				}
			}

			// SET THE RESULT ONLY IF IT IS NOT ZERO
			if (value != 0)
			{
				resultMat.setElement(currRow, currCol, value);
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
