#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <mutex>
#include <thread>
#include <cmath>
#include <string>

using namespace std;

mutex resultMutex;

void calculateBlockPartial(
    const vector<vector<int>> &matrixA,
    const vector<int> &vectorB,
    vector<int> &resultVectorC,
    int rowsPerBlock,
    int colsPerBlock,
    int rowStart,
    int colStart,
    int totalMatrixRows,
    int totalMatrixCols
) {
    int rowEnd = rowStart + rowsPerBlock;
    int colEnd = colStart + colsPerBlock;

    int numRowsInBlock = rowEnd - rowStart;

    if (numRowsInBlock <= 0) {
        return;
    }

    vector<int> localRowSums(numRowsInBlock, 0);

    for (int i = rowStart; i < rowEnd; i++) {
        int partialRowSum = 0;
        for (int j = colStart; j < colEnd; j++) {
            partialRowSum += matrixA[i][j] * vectorB[j];
        }
        localRowSums[i - rowStart] = partialRowSum;
    }

    lock_guard<mutex> lock(resultMutex);
    
    for (int localIndex = 0; localIndex < numRowsInBlock; localIndex++) {
        int globalRowIndex = rowStart + localIndex;
        resultVectorC[globalRowIndex] += localRowSums[localIndex];
    }
}

bool readVectorFromFile(const string& filename, vector<int>& vec, int size) {
    ifstream fileStream(filename);
    if (!fileStream) {
        return false;
    }

    vec.resize(size);
    for (int i = 0; i < size; ++i) {
        if (!(fileStream >> vec[i])) {
            return false; 
        }
    }
    fileStream.close();
    return true;
}

bool readMatrixFromFile(const string& filename, vector<vector<int>>& matrix, int rows, int cols) {
    ifstream fileStream(filename);
    if (!fileStream) {
        return false;
    }

    matrix.assign(rows, vector<int>(cols));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if (!(fileStream >> matrix[i][j])) {
                return false; 
            }
        }
    }
    fileStream.close();
    return true;
}

string Task1(string matrixFile, string vectorFile, int procNum, int rowNum, int colNum)
{
    stringstream functionOutput;
    if (procNum <= 0 || rowNum <= 0 || colNum <= 0) {
        functionOutput << "Error";
        return functionOutput.str();
    }

    vector<int> b;
    if (!readVectorFromFile(vectorFile, b, colNum)) {
        functionOutput << "No file containing vector or file is corrupted";
        return functionOutput.str();
    }

    vector<vector<int>> A;
    if (!readMatrixFromFile(matrixFile, A, rowNum, colNum)) {
        functionOutput << "No file containing matrix or file is corrupted";
        return functionOutput.str();
    }

    int blockRows = ceil(sqrt(procNum));
    int blockCols = procNum / blockRows;

    if (blockRows * blockCols != procNum || blockRows == 1 || blockCols == 1) {
        blockRows = procNum / 2;
        blockCols = 2;
    }
     
    int NumRowInBlock = (rowNum + blockRows - 1) / blockRows;
    int NumColInBlock = (colNum + blockCols - 1) / blockCols;
    
    vector<int> c(rowNum, 0);
    vector<thread> threads;
    threads.reserve(procNum); 
    
    int StartPointForRow = 0;
    int StartPointForCol = 0;
    int threadIndex = 0;

    for (int i = 0; i < rowNum; i += NumRowInBlock) {
        StartPointForCol = 0;
        for (int j = 0; j < colNum; j += NumColInBlock) {
            
            if (threadIndex >= procNum) {
                 break;
            }

            threads.emplace_back(
                calculateBlockPartial,
                ref(A),
                ref(b),
                ref(c),
                NumRowInBlock,
                NumColInBlock,
                StartPointForRow,
                StartPointForCol,
                rowNum,
                colNum
            );
            threadIndex++;
            StartPointForCol += NumColInBlock;
        }
        StartPointForRow += NumRowInBlock;
        if (threadIndex >= procNum) {
            break;
        }
    }

    for (auto& t : threads) {
        t.join();
    }

    for (int i = 0; i < rowNum; ++i) {
        functionOutput << c[i] << "\n";
    }

    return functionOutput.str();
}
