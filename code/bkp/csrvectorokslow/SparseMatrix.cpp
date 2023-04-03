/*
 *  Created on: Dec 22, 2020
 *      Author: kompalli
 */
#include "SparseMatrix.h"

using namespace std;

// THE STRUCTURE TO STORE THE CSR MATRIX
typedef struct SparseCSR
{
	int nbrRows;
	int nbrCols;
	int nbrNonZeros;
	int *values;
	int *rowPointers;
	int *columnIndices;

} SparseCSR;

// STRUCT TO STORE ROW, COL, VALUE OF THE ELEMENT IN THE VECTOR
struct element
{
	int row;
	int col;
	int value;
};

// VECTOR TO STORE Elements
std::vector<element> elements;

// INSTANTIATE THE SparseSCR
SparseCSR csrMatrix;

// int createSparseCSR(const int *Arr, int rows, int cols, int nnz, SparseCSR * CSRarr);
int createSparseCSRMatrix(int rows, int cols, int nnz, SparseCSR *CSRarr, std::vector<element> elts)
{
	// SAVE THE DIMENSIONS
	CSRarr->nbrRows = rows;
	CSRarr->nbrCols = cols;
	CSRarr->nbrNonZeros = nnz;

	// MEMORY ALLOCATION
	CSRarr->values = (int *)malloc(nnz * sizeof(int));
	CSRarr->rowPointers = (int *)malloc((rows + 1) * sizeof(int));
	CSRarr->columnIndices = (int *)malloc(nnz * sizeof(int));

	// NON-ZERO IDX
	int nnzIdx = 0;

	// SORT THE ELEMENTS VECTOR WITHOUT ALGORITHM
	// SORT THE ELEMENTS VECTOR BY ROW
	for (int i = 0; i < elts.size(); i++)
	{
		for (int j = i + 1; j < elts.size(); j++)
		{
			if (elts[i].row > elts[j].row)
			{
				element temp = elts[i];
				elts[i] = elts[j];
				elts[j] = temp;
			}
		}
	}


	// ITERATE TABLE ROWS AND COLS TO FILL CSR MATRIX VALUES, ROW POINTERS AND COLUMN INDEX, NNZIDX
	for (int i = 0; i < rows; i++)
	{
		// ROW POINTER
		CSRarr->rowPointers[i] = nnzIdx;

		// ITERATE THROUGH THE ELEMENTS VECTOR
		for (int j = 0; j < elts.size(); j++)
		{
			// IF THE CURRENT ROW IS EQUAL TO THE ELEMENT ROW
			if (i == elts[j].row)
			{
				// FILL THE CSR MATRIX VALUES, COLUMN INDEX AND NNZIDX
				CSRarr->values[nnzIdx] = elts[j].value;
				CSRarr->columnIndices[nnzIdx] = elts[j].col;
				nnzIdx++;
			}
		}
	}

	// FILL THE LAST ROW POINTER TO AVOID SEGMENTATION FAULT - OUT OF BOUNDS
	CSRarr->rowPointers[rows] = nnzIdx;

	return EXIT_SUCCESS;
}

// PRINT THE CSR MATRIX
int printSparseCSR(const SparseCSR *CSRarr)
{
	// PRINT THE DIMENSIONS
	printf("Number of rows: %d\tNumber of columns: %d\tNumber of non-zeros: %d \n", CSRarr->nbrRows, CSRarr->nbrCols, CSRarr->nbrNonZeros);

	// PRINT THE VALUES
	printf("Values: \n");
	for (int i = 0; i < CSRarr->nbrNonZeros; i++)
	{
		printf("%d\n", CSRarr->values[i]);
	}
	printf("\n");

	// PRINT THE COLUMN INDICES
	printf("Column Indices: \n");
	for (int i = 0; i < CSRarr->nbrNonZeros; i++)
	{
		printf("%d\n", CSRarr->columnIndices[i]);
	}
	printf("\n");

	// PRINT THE ROW POINTERS
	printf("Row Pointers: \n");
	for (int i = 0; i < CSRarr->nbrRows + 1; i++)
	{
		printf("%d\n", CSRarr->rowPointers[i]);
	}
	printf("\n");

	return EXIT_SUCCESS;
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
int SparseMatrix::setElement(int currRow, int currCol, int value)
{
	if (currRow < 0 || currRow > rows || currCol < 0 || currCol > cols)
	{
		printf("CurRow: %d\tCurCol: %d\tRows: %d\tCols: %d\n", currRow, currCol, rows, cols);
		std::invalid_argument("Input file has wrong format");
		return -1;
	}

	// IF THE MATRIX IS EMPTY
	if (csrMatrix.nbrNonZeros == 0)
	{
		// CREATE A NEW ELEMENT
		element newElement;
		newElement.row = currRow;
		newElement.col = currCol;
		newElement.value = value;

		// PUSH THE NEW ELEMENT TO THE VECTOR
		elements.push_back(newElement);

		// CREATE THE CSR MATRIX
		createSparseCSRMatrix(rows, cols, elements.size(), &csrMatrix, elements);

		return 1;
	}
	else
	{
		// ADD THE NEW ELEMENT TO THE VECTOR, CREATE THE CSR MATRIX WITH NEW VECTOR SIZE 
		element newElement;
		newElement.row = currRow;
		newElement.col = currCol;
		newElement.value = value;

		// PUSH THE NEW ELEMENT TO THE VECTOR
		elements.push_back(newElement);

		// DELETE THE OLD CSR MATRIX TO FREE THE MEMORY
		delete csrMatrix.values; // FREE THE MEMORY
		delete csrMatrix.rowPointers; // FREE THE MEMORY
		delete csrMatrix.columnIndices; // FREE THE MEMORY

		// CREATE THE CSR MATRIX WITH NEW VECTOR SIZE 
		createSparseCSRMatrix(rows, cols, elements.size(), &csrMatrix, elements);

		return 1;
	}

	return 0;
}

/**
 * Return the value of the element located at a position in the matrix
 * @param currRow Row of the position whose value is needed.
 * @param currCol Col of the position whose value is needed.
 */
int SparseMatrix::getElement(int currRow, int currCol)
{
	if (currRow < 0 || currRow > rows || currCol < 0 || currCol > cols)
	{
		printf("CurRow: %d\tCurCol: %d\tRows: %d\tCols: %d\n", currRow, currCol, rows, cols);
		std::invalid_argument("Input file has wrong format");
		return -1;
	}

	// ITERATE THROUGH ROW POINTERS TO FIND THE RANGE OF COLUMN INDICES WHERE THE ELEMENT MIGHT EXIST
	int startIdx = csrMatrix.rowPointers[currRow];
	int endIdx = csrMatrix.rowPointers[currRow + 1];

	// ITERATE THROUGH THE COLUMN INDICES TO FIND THE ELEMENT
	for (int i = startIdx; i < endIdx; i++)
	{
		// IF THE CURRENT COLUMN INDEX IS EQUAL TO THE COLUMN INDEX OF THE ELEMENT
		if (csrMatrix.columnIndices[i] == currCol)
		{
			// RETURN THE VALUE OF THE ELEMENT
			return csrMatrix.values[i];
		}
	}

	// IF THE ELEMENT IS NOT FOUND, RETURN 0
	return 0;
}

SparseMatrix::SparseMatrix(int numRows, int numCols)
{
}

void SparseMatrix::print()
{
}

int printSparseCSR(const SparseCSR *arrCSR);

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

	// COUNT THE NUMBER OF NON-ZERO ELEMENTS
	int nnz = 0;

	// READ THE MATRIX ELEMENTS
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

		// STORE THE ELEMENTS
		// element e;
		// e.row = row;
		// e.col = col;
		// e.value = value;
		// elements.push_back(e);

		// INCREMENT THE NON-ZERO COUNT
		// nnz++;
	}

	// CLOSE THE FILE
	infile.close();

	printf("Finished reading the matrix file\n");

	// // CREATE THE SPARSE MATRIX
	// createSparseCSRMatrix(rows, cols, nnz, &csrMatrix, elements);

	// DEALLOCATE THE MEMORY FOR THE VECTOR AND ELEMENTS
	// elements.clear();
	// elements.shrink_to_fit();

	// PRINT THE CSR MATRIX
	printSparseCSR(&csrMatrix);

	// FREE THE MEMORY THE CSR MATRIX - THIS IS NOT NEEDED SINCE THE MEMORY IS DEALLOCATED
	// free(csrMatrix.values);
	// free(csrMatrix.rowPointers);
	// free(csrMatrix.columnIndices);
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

		// SKIP EMPTY ROWS FOR BETTER PERFORMANCE ON LARGE MATRICES
		if (inputObject.rowPointers[currRow] == inputObject.rowPointers[currRow + 1])
			continue;

	// IF THE ROW IN THE FIRST MATRIX IS EMPTY, COPY THE ROW FROM THE SECOND MATRIX
	else if (rowPointers[currRow] == rowPointers[currRow + 1])
	{
		for (int i = inputObject.rowPointers[currRow]; i < inputObject.rowPointers[currRow + 1]; i++)
		{
			resultMat.setElement(currRow, inputObject.columnIndices[i], inputObject.values[i]);
		}
		continue;
	}

	// IF THE ROW IN THE SECOND MATRIX IS EMPTY, COPY THE ROW FROM THE FIRST MATRIX
	else if (inputObject.rowPointers[currRow] == inputObject.rowPointers[currRow + 1])
	{
		for (int i = rowPointers[currRow]; i < rowPointers[currRow + 1]; i++)
		{
			resultMat.setElement(currRow, columnIndices[i], values[i]);
		}
		continue;
	}

	// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS
	else
	{
		int i = rowPointers[currRow];
		int j = inputObject.rowPointers[currRow];
		while (i < rowPointers[currRow + 1] && j < inputObject.rowPointers[currRow + 1])
		{
			if (columnIndices[i] == inputObject.columnIndices[j])
			{
				value = values[i] + inputObject.values[j];
				resultMat.setElement(currRow, columnIndices[i], value);
				i++;
				j++;
			}
			else if (columnIndices[i] < inputObject.columnIndices[j])
			{
				resultMat.setElement(currRow, columnIndices[i], values[i]);
				i++;
			}
			else
			{
				resultMat.setElement(currRow, inputObject.columnIndices[j], inputObject.values[j]);
				j++;
			}
		}
		while (i < rowPointers[currRow + 1])
		{
			resultMat.setElement(currRow, columnIndices[i], values[i]);
			i++;
		}
		while (j < inputObject.rowPointers[currRow + 1])
		{
			resultMat.setElement(currRow, inputObject.columnIndices[j], inputObject.values[j]);
			j++;
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

		// SKIP EMPTY ROWS FOR BETTER PERFORMANCE ON LARGE MATRICES
		if (inputObject.rowPointers[currRow] == inputObject.rowPointers[currRow + 1])
			continue;

	// IF THE ROW IN THE FIRST MATRIX IS EMPTY, COPY THE ROW FROM THE SECOND MATRIX
	else if (rowPointers[currRow] == rowPointers[currRow + 1])
	{
		for (int i = inputObject.rowPointers[currRow]; i < inputObject.rowPointers[currRow + 1]; i++)
		{
			resultMat.setElement(currRow, inputObject.columnIndices[i], -inputObject.values[i]);
		}
		continue;
	}

	// IF THE ROW IN THE SECOND MATRIX IS EMPTY, COPY THE ROW FROM THE FIRST MATRIX
	else if (inputObject.rowPointers[currRow] == inputObject.rowPointers[currRow + 1])
	{
		for (int i = rowPointers[currRow]; i < rowPointers[currRow + 1]; i++)
		{
			resultMat.setElement(currRow, columnIndices[i], values[i]);
		}
		continue;
	}

	// IF BOTH ROWS ARE NOT EMPTY, ADD THE TWO ROWS
	else
	{
		int i = rowPointers[currRow];
		int j = inputObject.rowPointers[currRow];

		while (i < rowPointers[currRow + 1] && j < inputObject.rowPointers[currRow + 1])
		{
			if (columnIndices[i] == inputObject.columnIndices[j])
			{
				value = values[i] - inputObject.values[j];
				resultMat.setElement(currRow, columnIndices[i], value);
				i++;
				j++;
			}
			else if (columnIndices[i] < inputObject.columnIndices[j])
			{
				resultMat.setElement(currRow, columnIndices[i], values[i]);
				i++;
			}
			else
			{
				resultMat.setElement(currRow, inputObject.columnIndices[j], -inputObject.values[j]);
				j++;
			}
		}
		while (i < rowPointers[currRow + 1])
		{
			resultMat.setElement(currRow, columnIndices[i], values[i]);
			i++;
		}
		while (j < inputObject.rowPointers[currRow + 1])
		{
			resultMat.setElement(currRow, inputObject.columnIndices[j], -inputObject.values[j]);
			j++;
		}
	}

	return resultMat;
}


SparseMatrix SparseMatrix::operator*(SparseMatrix &inputObject)
{
	if (cols != inputObject.rows)
	{
		char *message = new char[2048];
		sprintf(message, "Number of cols in first matrix is not equal to number of rows in second matrix");
		throw std::invalid_argument(message);
		delete[] message;
	}

	SparseMatrix resultMat(rows, inputObject.cols);
	int value;

	for (int currRow = 0; currRow < rows; currRow++)
	{
		for (int currCol = 0; currCol < inputObject.cols; currCol++)
		{
			value = 0;
			for (int i = rowPointers[currRow]; i < rowPointers[currRow + 1]; i++)
			{
				for (int j = inputObject.rowPointers[currCol]; j < inputObject.rowPointers[currCol + 1]; j++)
				{
					if (columnIndices[i] == inputObject.columnIndices[j])
					{
						value += values[i] * inputObject.values[j];
					}
				}
			}
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
