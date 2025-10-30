#include <iostream>
#include <thread>
#include <fstream>
#include <vector>
#include <regex>
#include <sstream>
#include <functional>


void readFileToVector(const std::string& filename, std::vector<std::string>& lines) 
{
    std::string fileText;
    std::fstream file;

    std::string path = "./matrix/" + filename;
    file.open(path);

    if(!file.is_open()){
        throw std::runtime_error("Error opening file");
    }

    while(getline(file, fileText)){
        lines.push_back(fileText);
    }
    file.close();
}

void processMatrixFromVector(std::vector<std::string>& matrixRaw, std::vector<std::vector<int>>& matrix, int& mtRowsInt, int& mtColsInt) 
{   
    std::string mt1Rows = matrixRaw[0];
    matrixRaw.erase(matrixRaw.begin());
    std::string mt1Cols = matrixRaw[0];
    matrixRaw.erase(matrixRaw.begin());

    mtRowsInt = std::stoi(mt1Rows.substr(2));
    mtColsInt = std::stoi(mt1Cols.substr(2));

    matrix.resize(mtRowsInt, std::vector<int>(mtColsInt, 0));

    if (mt1Rows.substr(0, 2) != "L=" || mt1Cols.substr(0, 2) != "C="){
        throw std::runtime_error("File format invalid");
    }

    for (int i = 0; i < mtRowsInt; ++i) {
        std::string line = matrixRaw[0];
        std::istringstream streamToken(line);

        for (int j = 0; j < mtColsInt; ++j) {
            if (streamToken.eof()) {
                throw std::runtime_error("Not enough columns in row " + std::to_string(i+1));
            }
            streamToken >> matrix[i][j];
        }
        matrixRaw.erase(matrixRaw.begin());
    }
}

void computeRow(int rowIndex, const std::vector<std::vector<int>>& matrix1, 
    const std::vector<std::vector<int>>& matrix2, std::vector<std::vector<int>>& result, 
    int mt1ColsInt, int mt2ColsInt) {
    for (int j = 0; j < mt2ColsInt; ++j) {
        for (int k = 0; k < mt1ColsInt; ++k) {
            result[rowIndex][j] += matrix1[rowIndex][k] * matrix2[k][j];
        }
    }
}

int main() {
    std::cout << "Matrix Multiplication using Multithreading" << std::endl;
    std::cout << "Pass the file name containing the matrices." << std::endl;
    std::string filename;
    std::cin >> filename;
    std::cout << "Processing file: " << filename << std::endl;
    
    try{
        std::vector<std::vector<int>> matrix1;
        std::vector<std::vector<int>> matrix2;
        std::vector<std::string> matrixRaw;
        int mt1RowsInt, mt1ColsInt, mt2RowsInt, mt2ColsInt;

        readFileToVector(filename, matrixRaw);
        processMatrixFromVector(matrixRaw, matrix1, mt1RowsInt, mt1ColsInt);
        processMatrixFromVector(matrixRaw, matrix2, mt2RowsInt, mt2ColsInt);

        std::vector<std::vector<int>> result(mt1RowsInt, std::vector<int>(mt2ColsInt, 0));
        std::vector<std::thread> threads;

        for (int i = 0; i < mt1RowsInt; ++i) {
            threads.emplace_back(
                computeRow,
                i,
                std::cref(matrix1),
                std::cref(matrix2),
                std::ref(result),
                mt1ColsInt,
                mt2ColsInt
            );
        }

        for (auto& thread : threads) {
            thread.join();
        }

        std::cout << "Resultant Matrix:" << std::endl;
        for (const auto& row : result) {
            for (const auto& val : row) {
                std::cout << val << " ";
            }
            std::cout << std::endl;
        }
    }catch(const std::runtime_error& e){
        std::cerr << "Error: " << e.what() << std::endl;
    }
 

    return 0;
}
