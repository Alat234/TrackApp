#include <iostream>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <mutex>
#include <thread>
#include <functional> 

using namespace std;
mutex mx;

bool ReadVectorFromFile(const string& filename, vector<int>& vec, int size) {
    ifstream fVector(filename);
    if (!fVector) {
        return false;
    }
    
    vec.resize(size); 
    for (int i = 0; i < size; ++i) {
        if (!(fVector >> vec[i])) {
            fVector.close();
            return false; 
        }
    }
    fVector.close();
    return true;
}


bool ReadMatrixFromFile(const string& filename, vector<vector<int>>& matrix, int rowNum, int colNum) {
    ifstream fMatrix(filename);
    if (!fMatrix) {
        return false;
    }


    matrix.resize(rowNum, vector<int>(colNum)); 
    for (int i = 0; i < rowNum; ++i) {
        for (int j = 0; j < colNum; ++j) {
            if (!(fMatrix >> matrix[i][j])) {
                fMatrix.close();
                return false; 
            }
        }
    }
    fMatrix.close();
    return true;
}


void CalculateSectorForRows(vector<vector<int>> &mat,vector<int> vectorOfNum,int NumOfRowsForTask, int startRow, int NumOfCol,vector<int> &c) {
    for (int row = 0; row < NumOfRowsForTask; row++) {
        int RowResultOfCal = 0;
        for (int col = 0; col < NumOfCol; col++) {
            RowResultOfCal += mat[startRow + row][col] * vectorOfNum[col];
        }
        lock_guard<mutex> lock(mx);
        c[startRow + row] = RowResultOfCal;
    }
}

string Task1(string matrixFile, string vectorFile, int procNum, int rowNum, int colNum) {

    stringstream functionOutput;

    if (procNum <= 0 || rowNum <= 0 || colNum <= 0 ) {
        functionOutput << "Error";
        return functionOutput.str();
    }

    int stripSize = rowNum / procNum;

    if (stripSize % 2 != 0) {
        functionOutput << "Number of rows is not a multiple of processing unit number";
        return functionOutput.str();
    }
    
    vector<int> b;
  
    if (!ReadVectorFromFile(vectorFile, b, colNum)) {
        functionOutput << "No file containing vector or file is corrupted";
        return functionOutput.str();
    }

    vector<vector<int>> A;
    
    if (!ReadMatrixFromFile(matrixFile, A, rowNum, colNum)) {
        functionOutput << "No file containing matrix or file is corrupted";
        return functionOutput.str();
    }
    
    vector<int> c(rowNum);
    vector<thread> threads(procNum);
    
    for (int i = 0; i < procNum; ++i) {
        int startRow = i * stripSize; 
        int endRow = (i == procNum - 1) ? rowNum : (i + 1) * stripSize; 
        
        threads[i] = thread(CalculateSectorForRows,
            ref(A),
            ref(b),
            stripSize,
            startRow,
            colNum,
            ref(c));
    }
    
    for (auto& t : threads) {
        t.join();
    }

    for (int i = 0; i < rowNum; ++i) {
        functionOutput << c[i] << "\n";
    }
    return functionOutput.str();
}


void CalculatePartialVector(std::vector<std::vector<int>>& mat, vector<int> vectorOfNum, vector<int>& partial_results,int NumOfRow,int NumOfColsForTask,int startCol) {
    partial_results.assign(NumOfRow, 0);
    for (int col = startCol; col < startCol + NumOfColsForTask; ++col) {
        for (int row = 0; row < NumOfRow; ++row) {
            partial_results[row] += mat[row][col] * vectorOfNum[col];
        }
    }
}

string Task2(string matrixFile, string vectorFile, int procNum, int rowNum, int colNum) {

    stringstream functionOutput;

    if (procNum <= 0 || rowNum <= 0 || colNum <= 0) {
        functionOutput << "Error";
        return functionOutput.str();
    }

    int stripSize = colNum / procNum;

    if (stripSize % 2 != 0) {
        functionOutput << "Number of columns is not a multiple of processing unit number";
        return functionOutput.str();
    }

    vector<int> b;
   
    if (!ReadVectorFromFile(vectorFile, b, colNum)) {
        functionOutput << "No file containing vector or file is corrupted";
        return functionOutput.str();
    }

    vector<vector<int>> A;
    
    if (!ReadMatrixFromFile(matrixFile, A, rowNum, colNum)) {
        functionOutput << "No file containing matrix or file is corrupted";
        return functionOutput.str();
    }

    vector<int> c(rowNum);
    vector<thread> threads(procNum);
    vector<vector<int>> partial_results(procNum, vector<int>(rowNum));

    for (int i = 0; i < procNum; ++i) {
        int startCol = i * stripSize;
        int NumOfColsForTask = stripSize;
        
        threads[i] = std::thread(CalculatePartialVector,
            std::ref(A),                 
            std::cref(b),          
            std::ref(partial_results[i]),    
            rowNum,                        
            NumOfColsForTask,              
            startCol);                     
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    for (int i = 0; i < procNum; ++i) {
        for (int row = 0; row < rowNum; ++row) {
            c[row] += partial_results[i][row];
        }
    }

    for (int i = 0; i < rowNum; ++i) {
        functionOutput << c[i] << "\n";
    }

    return functionOutput.str();
}
