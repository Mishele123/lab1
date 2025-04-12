
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>


std::vector<std::vector<int>> readMatrix(const std::string& filename, int& rows, int& cols) 
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file " + filename);

    std::vector<std::vector<int>> matrix;
    rows = cols = 0;
    std::string line;
    bool has_data = false;

    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        std::vector<int> row;
        int value;

        while (ss >> value) 
        {
            row.push_back(value);
            has_data = true;
        }

        if (!row.empty()) 
        {
            matrix.push_back(row);
            cols = std::max(cols, static_cast<int>(row.size()));
            ++rows;
        }
    }

    file.close();
    if (!has_data || rows == 0)
        throw std::runtime_error("File " + filename + " is empty or contains no valid data");

    for (int i = 0; i < rows; ++i) 
    {
        while (matrix[i].size() < cols) 
        {
            matrix[i].push_back(0);
        }
    }

    return matrix;
}


std::vector<std::vector<int>> multiplyMatrices(const std::vector<std::vector<int>>& A,
    const std::vector<std::vector<int>>& B, const int rowsA, const int colsA, const int colsB)
{
    std::vector<std::vector<int>> result(rowsA, std::vector<int>(colsB, 0));
    for (int i = 0; i < rowsA; i++)
    {
        for (int j = 0; j < colsB; j++)
        {
            for (int k = 0; k < colsA; k++)
            {
                result[i][j] += A[i][k] * B[k][j];
            }
            
        }
    }
    return result;
}


void writeMatrix(const std::string& filename, const std::vector<std::vector<int>>& matrix) 
{
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file for writing: " + filename);
    
    for (const auto& row : matrix) 
    {
        for (int val : row)
            file << val << " ";
        file << "\n";
    }
    file.close();
}


int main()
{
    try
    {
        int rowsA, colsA, rowsB, colsB;
        std::string fileA = "../../Matrix1.txt";
        std::string fileB = "../../Matrix2.txt";
        std::string resultFile = "../../result.txt";
        
        auto matrixA = readMatrix(fileA, rowsA, colsA);
        auto matrixB = readMatrix(fileB, rowsB, colsB);

        if (colsA != rowsB)
            throw std::runtime_error("Matrices cant ne multiplied colsA != colsB");
        
        auto start = std::chrono::high_resolution_clock::now();
        auto result = multiplyMatrices(matrixA, matrixB, rowsA, colsA, colsB);
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
        
        writeMatrix(resultFile, result);

        std::cout << "Lead time: " << duration << std::endl;
        std::cout << "Task volume: " << rowsA << "*" << colsB << std::endl;
        std::cout << "The result is written to the file: " << resultFile << std::endl;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error:" << ex.what() << std::endl;
        return -1; 
    }
    return 0;
}