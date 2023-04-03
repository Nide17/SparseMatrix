/*
 * homework.cpp
 *
 *  Created on: Dec 13, 2020
 *      Author: kompalli
 */

#include "homework.h"

int main(int argc, char** argv) {
	LogManager::resetLogFile();
	LogManager::writePrintfToLog(LogManager::Level::Status, "main", "In main file.");
	if (argc < 4){
		printf("Usage:\n\n");
		printf("./homework addn pathToMatrix1   pathToMatrix2 outputPath\n\n");
		printf("./homework mult pathToMatrix1   pathToMatrix2 outputPath\n\n");
		printf("./homework subt pathToMatrix1   pathToMatrix2 outputPath\n\n");
		printf("./homework check pathToMatrix1  outputPath\n\n");
		return 0;
	}

	char outputMemPath[1024];
	if (argc == 4)
		sprintf(outputMemPath, "%s.memory_runtime", argv[3]);
	else
		sprintf(outputMemPath, "%s.memory_runtime", argv[4]);
	FILE* outFileStream = fopen(outputMemPath, "w");
	if (! outFileStream){
		std::string message("Cannot open output file for writing:");
		message = message + outputMemPath;
		throw std::ios_base::failure(message.c_str());
	}

	unsigned long peakMem1 = getPeakRSS();
	unsigned long currMem1 = getCurrentRSS();
	fprintf(outFileStream, "peakRSS = %lu, currMem=%lu\n", peakMem1, currMem1);
	auto start = std::chrono::high_resolution_clock::now();

	if (argc == 4){
		char path1[1024], output[1024];
		sprintf(path1, "%s", argv[2]);
		sprintf(output, "%s", argv[3]);

		if (!path1){
			printf("allocation of path1 failed\n");
			return -1;
		}
		if (!output){
			printf("allocation of outputPath failed\n");
			return -1;
		}
		SparseMatrix matrix1(path1);
		if (strcmp(argv[1], "check") == 0){
			/**
			 * This command line argument is used to check that file
			 * is loaded properly from text file and written properly to
			 * a text file.
			 */
			matrix1.printToASCIIFile(output);
		}
	}

	if (argc == 5){
		char path1[1024], path2[1024], output[1024];
		sprintf(path1, "%s", argv[2]);
		sprintf(path2, "%s", argv[3]);
		sprintf(output, "%s", argv[4]);

		if (!path1){
			printf("allocation of path1 failed\n");
			return -1;
		}
		if (!path2){
			printf("allocation of path2 failed\n");
			return -1;
		}
		if (!output){
			printf("allocation of outputPath failed\n");
			return -1;
		}
		SparseMatrix matrix1(path1);
		SparseMatrix matrix2(path2);
		if (strcmp(argv[1], "addn") == 0){
			/**
			 * This command line argument is used to check
			 * addition of two matrices.
			 */
			SparseMatrix newMat = matrix1 + matrix2;
			newMat.printToASCIIFile(output);
		}
		if (strcmp(argv[1], "subt") == 0){
			/**
			 * This command line argument is used to check
			 * subtraction of two matrices.
			 */
			SparseMatrix newMat = matrix1 - matrix2;
			newMat.printToASCIIFile(output);
		}
		if (strcmp(argv[1], "mult") == 0){
			/**
			 * This command line argument is used to check
			 * multiplication of two matrices.
			 */
			SparseMatrix newMat = matrix1 * matrix2;
			newMat.printToASCIIFile(output);
		}
		if (strcmp(argv[1], "check") == 0){
			/**
			 * This command line argument is used to check
			 * multiplication of two matrices.
			 */
			// SparseMatrix newMat = matrix1 * matrix2;
			printf("SparseMatrix::SparseMatrix(char *matrixFilePath)");
			matrix1.printToASCIIFile(output);
		}
	}


	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast < std::chrono::microseconds
			> (stop - start);
	fprintf(outFileStream, "run time =%d microseconds\n", (int) duration.count());

	unsigned long peakMem2 = getPeakRSS();
	unsigned long currMem2 = getCurrentRSS();
	fprintf(outFileStream, "peakRSS = %lu, currMem=%lu\n", peakMem2, currMem2);
	fprintf(outFileStream, "Diff peakRSS = %lu Bytes, currMem=%lu Bytes\n",
			peakMem2 - peakMem1, currMem2 - currMem1);

	fclose(outFileStream);
}
