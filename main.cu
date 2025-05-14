#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <stdexcept>
#include <string>

#include "cuda_runtime.h"

using namespace std;



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


__global__ void matrixMultiplyKernel(float *A, float *B, float *C, int A_rows, int A_cols, int B_cols)
{
    int row = blockIdx.y * blockDim.y + threadIdx.y;
    int col = blockIdx.x * blockDim.x + threadIdx.x;

    if (row < A_rows && col < B_cols)
    {
        float sum = 0.0f;
        for (int i = 0; i < A_cols; ++i)
        {
            sum += A[row * A_cols + i] * B[i * B_cols + col];
        }
        C[row * B_cols + col] = sum;
    }
}


vector<vector<float>> mulMatricesCUDA(const vector<vector<float>> &A, const vector<vector<float>> &B)
{
    int A_rows = A.size(), A_cols = A[0].size(), B_cols = B[0].size();

    vector<float> h_A(A_rows * A_cols);
    vector<float> h_B(A_cols * B_cols);
    vector<float> h_C(A_rows * B_cols);

    for (int i = 0; i < A_rows; ++i)
        for (int j = 0; j < A_cols; ++j)
            h_A[i * A_cols + j] = A[i][j];

    for (int i = 0; i < A_cols; ++i)
        for (int j = 0; j < B_cols; ++j)
            h_B[i * B_cols + j] = B[i][j];

    float *d_A, *d_B, *d_C;
    size_t size_A = h_A.size() * sizeof(float);
    size_t size_B = h_B.size() * sizeof(float);
    size_t size_C = h_C.size() * sizeof(float);

    cudaMalloc(&d_A, size_A);
    cudaMalloc(&d_B, size_B);
    cudaMalloc(&d_C, size_C);

    cudaMemcpy(d_A, h_A.data(), size_A, cudaMemcpyHostToDevice);
    cudaMemcpy(d_B, h_B.data(), size_B, cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid((B_cols + 15) / 16, (A_rows + 15) / 16);

    matrixMultiplyKernel<<<blocksPerGrid, threadsPerBlock>>>(d_A, d_B, d_C, A_rows, A_cols, B_cols);
    cudaDeviceSynchronize();

    cudaMemcpy(h_C.data(), d_C, size_C, cudaMemcpyDeviceToHost);

    cudaFree(d_A);
    cudaFree(d_B);
    cudaFree(d_C);

    vector<vector<float>> result(A_rows, vector<float>(B_cols));
    for (int i = 0; i < A_rows; ++i)
        for (int j = 0; j < B_cols; ++j)
            result[i][j] = h_C[i * B_cols + j];

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
            auto result = mulMatricesCUDA(matrixA, matrixB);
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
            total_time += duration;

            writeMatrix(resultFile, result);

            std::cout << "Lead time for " << fileA << " and " << fileB << ": " << duration << " us" << std::endl;
            std::cout << "Task volume: " << rowsA << " * " << colsB << std::endl;
            std::cout << "The result is written to the file: " << resultFile << std::endl;
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