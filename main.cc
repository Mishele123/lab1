#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <stdexcept>
#include <string>
#include <omp.h>



std::vector<std::vector<float>> readMatrix(const std::string& filename, size_t& rows, size_t& cols) 
{
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file: " + filename);

    std::vector<std::vector<float>> matrix;
    rows = 0;
    std::string line;

    while (std::getline(file, line)) 
    {
        std::stringstream ss(line);
        std::vector<float> row;
        float value;

        while (ss >> value) 
        {
            row.push_back(value);
        }

        if (!row.empty()) 
        {
            matrix.push_back(row);
            cols = std::max(cols, row.size());
            rows++;
        }
    }

    file.close();

    if (rows == 0)
        throw std::runtime_error("File is empty or contains no valid data");

    for (auto& row : matrix) 
    {
        row.resize(cols, 0.0f);
    }

    return matrix;
}


std::vector<std::vector<float>> multiplyMatrices(const std::vector<std::vector<float>>& A,
    const std::vector<std::vector<float>>& B, size_t rowsA, size_t colsA, size_t colsB)
{
    std::vector<std::vector<float>> result(rowsA, std::vector<float>(colsB, 0.0f));
    #pragma omp for collapce(2)
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


void writeMatrix(const std::string& filename, const std::vector<std::vector<float>>& matrix) 
{
    std::ofstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("Cannot open file for writing: " + filename);

    for (const auto& row : matrix) 
    {
        for (float val : row)
            file << val << " ";
        file << "\n";
    }
    file.close();
}


int main()
{
    try
    {
        long total_time = 0;
        size_t rowsA, colsA = 0, rowsB, colsB = 0;

        std::vector<std::pair<std::string, std::string>> files = {
            {"../../Matrix_1/matrix1_10.txt", "../../Matrix_2/matrix2_10.txt"},
            {"../../Matrix_1/matrix1_20.txt", "../../Matrix_2/matrix2_20.txt"},
            {"../../Matrix_1/matrix1_30.txt", "../../Matrix_2/matrix2_30.txt"},
            {"../../Matrix_1/matrix1_40.txt", "../../Matrix_2/matrix2_40.txt"},
            {"../../Matrix_1/matrix1_50.txt", "../../Matrix_2/matrix2_50.txt"},
            {"../../Matrix_1/matrix1_60.txt", "../../Matrix_2/matrix2_60.txt"},
            {"../../Matrix_1/matrix1_70.txt", "../../Matrix_2/matrix2_70.txt"},
            {"../../Matrix_1/matrix1_80.txt", "../../Matrix_2/matrix2_80.txt"},
            {"../../Matrix_1/matrix1_90.txt", "../../Matrix_2/matrix2_90.txt"},
            {"../../Matrix_1/matrix1_100.txt", "../../Matrix_2/matrix2_100.txt"},
            {"../../Matrix_1/matrix1_1000.txt", "../../Matrix_2/matrix2_1000.txt"}
        };

        omp_set_num_threads(omp_get_num_threads());

        for (size_t i = 0; i < files.size(); i++)
        {
            std::string fileA = files[i].first;
            std::string fileB = files[i].second;
            std::string resultFile = "../../Output/output_" + std::to_string((i + 1) * 10) + ".txt";

            auto matrixA = readMatrix(fileA, rowsA, colsA);
            auto matrixB = readMatrix(fileB, rowsB, colsB);

            if (colsA != rowsB)
                throw std::runtime_error("Matrices cannot be multiplied: colsA != rowsB");

            auto start = std::chrono::high_resolution_clock::now();
            auto result = multiplyMatrices(matrixA, matrixB, rowsA, colsA, colsB);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            total_time += duration;

            writeMatrix(resultFile, result);

            std::cout << "Lead time for " << fileA << " and " << fileB << ": " << duration << " us" << std::endl;
            std::cout << "Task volume: " << rowsA << " * " << colsB << std::endl;
            std::cout << "The result is written to the file: " << resultFile << std::endl;
            std::cout << "Number of threads used: " << omp_get_max_threads() << std::endl;
        }

        std::cout << "Total lead time: " << total_time << " us" << std::endl;
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return -1; 
    }
    return 0;
}
