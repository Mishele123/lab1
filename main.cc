
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>


std::vector<std::vector<size_t>> readMatrix(const std::string& filename, size_t& rows, size_t& cols) 
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file " + filename);

    std::vector<std::vector<size_t>> matrix;
    rows = cols = 0;
    std::string line;
    bool has_data = false;

    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        std::vector<size_t> row;
        size_t value;

        while (ss >> value) 
        {
            row.push_back(value);
            has_data = true;
        }

        if (!row.empty()) 
        {
            matrix.push_back(row);
            cols = std::max(cols, static_cast<size_t>(row.size()));
            rows++;
        }
    }

    file.close();
    if (!has_data || rows == 0)
        throw std::runtime_error("File " + filename + " is empty or contains no valid data");

    for (size_t i = 0; i < rows; ++i) 
    {
        while (matrix[i].size() < cols) 
        {
            matrix[i].push_back(0);
        }
    }

    return matrix;
}


std::vector<std::vector<size_t>> multiplyMatrices(const std::vector<std::vector<size_t>>& A,
    const std::vector<std::vector<size_t>>& B, const size_t rowsA, const size_t colsA, 
    const size_t colsB)
{
    std::vector<std::vector<size_t>> result(rowsA, std::vector<size_t>(colsB, 0));
    for (size_t i = 0; i < rowsA; i++)
    {
        for (size_t j = 0; j < colsB; j++)
        {
            for (size_t k = 0; k < colsA; k++)
            {
                result[i][j] += A[i][k] * B[k][j];
            }
            
        }
    }
    return result;
}


void writeMatrix(const std::string& filename, const std::vector<std::vector<size_t>>& matrix) 
{
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file for writing: " + filename);
    
    for (const auto& row : matrix) 
    {
        for (size_t val : row)
            file << val << " ";
        file << "\n";
    }
    file.close();
}


int main()
{
    try
    {
        size_t rowsA, colsA, rowsB, colsB;
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