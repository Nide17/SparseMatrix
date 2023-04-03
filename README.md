# SparseMatrix

The SparseMatrix class uses an array of binary search trees to store the non-zero elements of the matrix. Each binary search tree in the array corresponds to a row of the matrix, and the nodes of the binary search trees store the column number and value of the non-zero elements in sorted order. The binary search trees are implemented using a Node struct, which contains the row number, column number, value, and pointers to the left and right children of the node.

This library provides a class for sparse matrices in C++ with the following features:

    Load matrix from a file
    Create a sparse matrix of any given size
    Print matrix to an output file
    Get the value of an element at a given position
    Set the value of an element at a given position
    Perform matrix addition, subtraction, and multiplication

**Dependencies**

This library depends on the following:

    iostream
    fstream
    limits.h
    math.h
    unistd.h

It also uses the GetMemUsage and LogManager utilities provided in the util directory.

#### Usage
Creating a Sparse Matrix

To create a sparse matrix from a file, use the constructor SparseMatrix(char *matrixFilePath). For example:

> SparseMatrix matrix("input.txt");

To create a sparse matrix of a given size, use the constructor SparseMatrix(int numRows, int numCols). For example:


> SparseMatrix matrix(5, 5);

#### Printing a Sparse Matrix

To print a sparse matrix to a file, use the method printToASCIIFile(char *outputFileName). For example:

> matrix.printToASCIIFile("output.txt");

#### Getting and Setting Elements

To get the value of an element at a given position, use the method getElement(int currRow, int currCol). For example:

> int value = matrix.getElement(2, 3);

To set the value of an element at a given position, use the method setElement(int currRow, int currCol, int value). For example:

> matrix.setElement(2, 3, 10);

#### Matrix Operations

The library supports matrix addition, subtraction, and multiplication. To perform these operations, use the following operators:

    + for addition
    - for subtraction
    * for multiplication

For example:


> SparseMatrix matrixA("inputA.txt");
> SparseMatrix matrixB("inputB.txt");
> 
> SparseMatrix result = matrixA + matrixB;
> result.printToASCIIFile("output.txt");
> 
> result = matrixA - matrixB;
> result.printToASCIIFile("output.txt");
> 
> result = matrixA * matrixB;
> result.printToASCIIFile("output.txt");


#### Implementation details

The SparseMatrix class represents a sparse matrix data structure using binary search trees to store the non-zero elements of the matrix in sorted order. The class provides methods to set and get the elements of the matrix and also to create and destroy the matrix.

The setElement() method first checks if the row and column number are within the range of the matrix. If the value of the element is zero, the method returns without adding the element to the matrix. If the binary search tree corresponding to the row of the element is empty, the method creates a new node and sets it as the root of the binary search tree. If the binary search tree already contains a node with the same column number as the element, the method updates the value of the node. Otherwise, the method traverses the binary search tree until it finds the correct position to insert the new node.

The getElement() method also checks if the row and column number are within the range of the matrix. If the binary search tree corresponding to the row of the element is empty, the method returns zero. Otherwise, the method performs a binary search on the binary search tree to find the node with the correct column number and returns its value.

The SparseMatrix class also contains a destructor to delete the binary search trees and the nodes. The errorMessage() function is used to throw an exception if an error occurs in the setElement() or getElement() method.



    #include "SparseMatrix.h"
    #include <iostream>
    using namespace std;
    
    int main()
    {
        SparseMatrix matrix(3, 3);
        matrix.setElement(0, 0, 1);
        matrix.setElement(0, 2, 2);
        matrix.setElement(1, 1, 3);
        matrix.setElement(2, 0, 4);
        matrix.setElement(2, 2, 5);
    
        for (int i = 0; i < 3; i++)
        {
            for (int j = 0; j < 3; j++)
            {
                cout << matrix.getElement(i, j) << " ";
            }
            cout << endl;
        }
    
        return 0;
    }
    
    
#### Authors

    Suryaprakash Kompalli
    Niyomwungeri Parmenide Ishimwe