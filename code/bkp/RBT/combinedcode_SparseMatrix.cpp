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

/**
 * To implement a data structure that optimizes both memory and runtime while storing such large matrices, we can use Arrays of Red-Black Trees. 
 * In this approach, we create an array of Red-Black Trees, where each tree represents a row of the sparse matrix. 
 * The keys of the tree are the column indices, and the values are the non-zero elements of the corresponding row.
 * 
 * Implement the SparseMatrix class using Arrays of Red-Black Trees:
*/

/**
 * Implement basic functions on a sparse matrix that contains integers.
 *
 * The default value in each element of the matrix is assumed to be 0.
 */
class SparseMatrix
{
private:
	int rows, cols; // Number of rows and columns in the matrix
	RedBlackTree *row;

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

	// DESTRUCTOR TO DELETE THE RED BLACK TREE SPARSE MATRIX
	~SparseMatrix();
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
	/**8
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

	/**
	 * Print the matrix to the console.
	 */
	void print()
	{
	}

	/**
	 * Binary search operation to find the index of the element in the colInd array.
	 */
	int binarySearch(int *arr, int startIndex, int endIndex, int target)
	{
		int left = startIndex;
		int right = endIndex - 1;

		while (left <= right)
		{
			int mid = left + (right - left) / 2;

			if (arr[mid] == target)
			{
				return mid;
			}
			else if (arr[mid] < target)
			{
				left = mid + 1;
			}
			else
			{
				right = mid - 1;
			}
		}
		return left;
	}

	// operator+ IS A CALL TO THE DEFAULT CONSTRUCTOR OF THE CLASS SparseMatrix
	SparseMatrix operator+(SparseMatrix &inputObject);

	// operator- IS A CALL TO THE DEFAULT CONSTRUCTOR OF THE CLASS SparseMatrix
	SparseMatrix operator-(SparseMatrix &inputObject);

	// operator* IS A CALL TO THE DEFAULT CONSTRUCTOR OF THE CLASS SparseMatrix
	SparseMatrix operator*(SparseMatrix &inputObject);
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

#endif /* MATRIX_H_ *//*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"
using namespace std;

/**
 * To implement a data structure that optimizes both memory and run time while storing such large matrices, we can use arrays of Red-Black Trees (RBTs). An RBT is a self-balancing binary search tree that guarantees log(n) worst-case time complexity for insertions, deletions, and searches. By using arrays of RBTs, we can store the non-zero elements of the sparse matrix efficiently, while saving memory by not storing the zero elements. In this approach, we create an array of RBTs, where each tree represents a row of the sparse matrix. The keys of the tree are the column indices, and the values are the non-zero elements of the corresponding row. 
 */
// RED BLACK TREE
class RedBlackTree
{
private:
	// NODE OF THE RED BLACK TREE
	struct Node
	{
		int key;
		int value;
		Node *left, *right, *parent;
		bool color;
	};

	// ROOT OF THE RED BLACK TREE
	Node *root;

	// NIL NODE OF THE RED BLACK TREE
	Node *nil;

	// FUNCTION TO PERFORM LEFT ROTATION
	void leftRotate(Node *x)
	{
		Node *y = x->right;
		x->right = y->left;
		if (y->left != nil)
			y->left->parent = x;
		y->parent = x->parent;
		if (x->parent == nil)
			root = y;
		else if (x == x->parent->left)
			x->parent->left = y;
		else
			x->parent->right = y;
		y->left = x;
		x->parent = y;
	}

	// FUNCTION TO PERFORM RIGHT ROTATION
	void rightRotate(Node *x)
	{
		Node *y = x->left;
		x->left = y->right;
		if (y->right != nil)
			y->right->parent = x;
		y->parent = x->parent;
		if (x->parent == nil)
			root = y;
		else if (x == x->parent->right)
			x->parent->right = y;
		else
			x->parent->left = y;
		y->right = x;
		x->parent = y;
	}

	// FUNCTION TO INSERT A NEW NODE IN THE RED BLACK TREE
	void insert(Node *z)
	{
		Node *y = nil;
		Node *x = root;
		while (x != nil)
		{
			y = x;
			if (z->key < x->key)
				x = x->left;
			else
				x = x->right;
		}
		z->parent = y;
		if (y == nil)
			root = z;
		else if (z->key < y->key)
			y->left = z;
		else
			y->right = z;
		z->left = nil;
		z->right = nil;
		z->color = true;
		insertFixup(z);
	}

	// FUNCTION TO FIX THE RED BLACK TREE AFTER INSERTION
	void insertFixup(Node *z) {
		while (z->parent->color == true) {
			if (z->parent == z->parent->parent->left) {
				Node *y = z->parent->parent->right;
				if (y->color == true) {
					z->parent->color = false;
					y->color = false;
					z->parent->parent->color = true;
					z = z->parent->parent;
				}
				else {
					if (z == z->parent->right) {
						z = z->parent;
						leftRotate(z);
					}
					z->parent->color = false;
					z->parent->parent->color = true;
					rightRotate(z->parent->parent);
				}
			}
			else {
				Node *y = z->parent->parent->left;
				if (y->color == true) {
					z->parent->color = false;
					y->color = false;
					z->parent->parent->color = true;
					z = z->parent->parent;
				}
				else {
					if (z == z->parent->left) {
						z = z->parent;
						rightRotate(z);
					}
					z->parent->color = false;
					z->parent->parent->color = true;
					leftRotate(z->parent->parent);
				}
			}
		}
		root->color = false;
	}

	// FUNCTION TO TRANSPLANT TWO NODES IN THE RED BLACK TREE
	void transplant(Node *u, Node *v)
	{
		if (u->parent == nil)
			root = v;
		else if (u == u->parent->left)
			u->parent->left = v;
		else
			u->parent->right = v;
		v->parent = u->parent;
	}

	// FUNCTION TO DELETE A NODE FROM THE RED BLACK TREE
	void deleteNode(Node *z)
	{
		Node *y = z;
		Node *x;
		bool yOriginalColor = y->color;
		if (z->left == nil)
		{
			x = z->right;
			transplant(z, z->right);
		}
		else if (z->right == nil)
		{
			x = z->left;
			transplant(z, z->left);
		}
		else
		{
			y = minimum(z->right);
			yOriginalColor = y->color;
			x = y->right;
			if (y->parent == z)
				x->parent = y;
			else
			{
				transplant(y, y->right);
				y->right = z->right;
				y->right->parent = y;
			}
			transplant(z, y);
			y->left = z->left;
			y->left->parent = y;
			y->color = z->color;
		}
		if (yOriginalColor == false)
			deleteFixup(x);
	}

	// FUNCTION TO FIX THE RED BLACK TREE AFTER DELETION
	void deleteFixup(Node *x)
	{
		while (x != root && x->color == false)
		{
			if (x == x->parent->left)
			{
				Node *w = x->parent->right;
				if (w->color == true)
				{
					w->color = false;
					x->parent->color = true;
					leftRotate(x->parent);
					w = x->parent->right;
				}
				if (w->left->color == false && w->right->color == false)
				{
					w->color = true;
					x = x->parent;
				}
				else
				{
					if (w->right->color == false)
					{
						w->left->color = false;
						w->color = true;
						rightRotate(w);
						w = x->parent->right;
					}
					w->color = x->parent->color;
					x->parent->color = false;
					w->right->color = false;
					leftRotate(x->parent);
					x = root;
				}
			}
			else
			{
				Node *w = x->parent->left;
				if (w->color == true)
				{
					w->color = false;
					x->parent->color = true;
					rightRotate(x->parent);
					w = x->parent->left;
				}
				if (w->right->color == false && w->right->color == false)
				{
					w->color = true;
					x = x->parent;
				}
				else
				{
					if (w->left->color == false)
					{
						w->right->color = false;
						w->color = true;
						leftRotate(w);
						w = x->parent->left;
					}
					w->color = x->parent->color;
					x->parent->color = false;
					w->left->color = false;
					rightRotate(x->parent);
					x = root;
				}
			}
		}
		x->color = false;
	}

	// FUNCTION TO SEARCH A NODE IN THE RED BLACK TREE
	Node* search(int key)
	{
		Node *temp = root;
		while (temp != nil && key != temp->key)
		{
			if (key < temp->key)
				temp = temp->left;
			else
				temp = temp->right;
		}
		return temp;
	}

	// FUNCTION TO FIND THE MINIMUM NODE IN THE RED BLACK TREE
	Node* minimum(Node *temp)
	{
		while (temp->left != nil)
			temp = temp->left;
		return temp;
	}

	// FUNCTION TO FIND THE MAXIMUM NODE IN THE RED BLACK TREE
	Node* maximum(Node *temp)
	{
		while (temp->right != nil)
			temp = temp->right;
		return temp;
	}

	// FUNCTION TO FIND THE SUCCESSOR OF A NODE IN THE RED BLACK TREE
	Node* successor(Node *x)
	{
		if (x->right != nil)
			return minimum(x->right);
		Node *y = x->parent;
		while (y != nil && x == y->right)
		{
			x = y;
			y = y->parent;
		}
		return y;
	}

	// FUNCTION TO FIND THE PREDECESSOR OF A NODE IN THE RED BLACK TREE
	Node* predecessor(Node *x)
	{
		if (x->left != nil)
			return maximum(x->left);
		Node *y = x->parent;
		while (y != nil && x == y->left)
		{
			x = y;
			y = y->parent;
		}
		return y;
	}

	// FUNCTION TO PRINT THE RED BLACK TREE
	void print(Node *temp)
	{
		if (temp != nil)
		{
			print(temp->left);
			cout << temp->key << " ";
			print(temp->right);
		}
	}

	// FUNCTION TO PRINT THE RED BLACK TREE IN A GRAPHICAL WAY
	void printGraphical(Node *temp, int space)
	{
		if (temp != nil)
		{
			space += 5;
			printGraphical(temp->right, space);
			cout << endl;
			for (int i = 5; i < space; i++)
				cout << " ";
			cout << temp->key << endl;
			printGraphical(temp->left, space);
		}
	}

	// FUNCTION TO PRINT THE RED BLACK TREE IN A GRAPHICAL WAY
	void printGraphical()
	{
		printGraphical(root, 0);
	}

	// FUNCTION TO PRINT THE RED BLACK TREE
	void print()
	{
		print(root);
	}

	// FUNCTION TO GET THE ROOT OF THE RED BLACK TREE
	Node* getRoot()
	{
		return root;
	}

	// FUNCTION TO GET THE NIL NODE OF THE RED BLACK TREE
	Node* getNil()
	{
		return nil;
	}

};



// DESTRUCTOR TO DELETE THE RED BLACK TREE SPARSE MATRIX
SparseMatrix::~SparseMatrix()
{
	delete[] row;
}

// CONSTRUCTOR TO INITIALIZE THE RED BLACK TREE SPARSE MATRIX, GIVEN NUMBER OF ROWS AND COLUMNS
SparseMatrix::SparseMatrix(int numRows, int numCols)
{
	rows = numRows;
	cols = numCols;
	row = new RedBlackTree[numRows];
}

// SETTING AND ADDING ELEMENT IN THE RED BLACK TREE SPARSE MATRIX,
int SparseMatrix::setElement(int currRow, int currCol, int value)
{	
	// CHECKING IF THE ROW AND COLUMN ARE WITHIN THE RANGE
	if (currRow < 0 || currRow >= rows || currCol < 0 || currCol >= cols)
	{
		char *message = new char[2048];
		sprintf(message, "Invalid row or column index: %d, %d", currRow, currCol);
		throw std::out_of_range(message);
		delete[] message;
	}

	// ADDING THE ELEMENT TO THE RED BLACK TREE SPARSE MATRIX
	Node *temp = row[currRow].search(currCol);

}

// GETTING THE ELEMENT FROM THE RED BLACK TREE SPARSE MATRIX, AT THE GIVEN ROW AND COLUMN
int SparseMatrix::getElement(int currRow, int currCol)
{
	
}

// READING FROM THE FILE AND STORING IN THE RED BLACK TREE SPARSE MATRIX FORMAT
SparseMatrix::SparseMatrix(char *matrixFilePath)
{
	// OPEN THE FILE
	FILE *inFileStream = fopen(matrixFilePath, "r");
	if (!inFileStream)
	{
		char *message = new char[2048];
		sprintf(message, "Cannot open input file for reading: %s", matrixFilePath);
		throw std::ios_base::failure(message);
		delete[] message;
	}

	// READ THE NUMBER OF ROWS AND COLUMNS
	fscanf(inFileStream, "rows=%d\n", &rows);
	fscanf(inFileStream, "cols=%d\n", &cols);

	// INITIALIZE THE ARRAY OF RED BLACK TREES
	// EACH TREE REPRESENTS A ROW OF THE SPARSE MATRIX
	// THE KEYS OF THE TREE ARE THE COLUMN INDICES
	// THE VALUES ARE THE NON-ZERO ELEMENTS OF THE CORRESPONDING ROW
	// data = new RBTree *[rows];

	// READ THE NON-ZERO ELEMENTS OF THE SPARSE MATRIX
	int currRow, currCol, value;
	while (fscanf(inFileStream, "%d, %d, %d", &currRow, &currCol, &value) != EOF)
	{
		setElement(currRow, currCol, value);
	}

	// CLOSE THE FILE
	fclose(inFileStream);

	// PRINT THE SPARSE MATRIX
	print();
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

	// PRINTING THE RESULT MATRIX TO THE CONSOLE
	// PRINT ONLY THE NON-ZERO ELEMENTS
	for (int currRow = 0; currRow < resultMat.rows; currRow++)
	{
		for (int currCol = 0; currCol < resultMat.cols; currCol++)
		{
			value = resultMat.getElement(currRow, currCol);
			if (value != 0)
			{
				printf("(%d, %d, %d) ", currRow, currCol, value);
			}
		}
		printf("\n");

		// IF REACHED THE END OF NON-ZERO ELEMENTS, OR NON-EMPTY ROWS, BREAK
		if (resultMat.getElement(currRow, 0) == 0)
		{
			break;
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
