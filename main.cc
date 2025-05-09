#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>
#include <stdexcept>
#include <string>
#include <mpi.h>

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

std::chrono::duration<double> parallelMatrixMultiply(
    const std::vector<std::vector<float>>& A,
    const std::vector<std::vector<float>>& B,
    std::vector<std::vector<float>>& result,
    size_t rowsA, size_t colsA, size_t colsB,
    int rank, int size)
{
    result.resize(rowsA, std::vector<float>(colsB, 0.0f));
    auto start = std::chrono::high_resolution_clock::now();

    size_t rowsPerProcess = rowsA / size;
    size_t startRow = rank * rowsPerProcess;
    size_t endRow = (rank == size - 1) ? rowsA : startRow + rowsPerProcess;

    for (size_t i = startRow; i < endRow; i++)
        for (size_t j = 0; j < colsB; j++)
            for (size_t k = 0; k < colsA; k++)
                result[i][j] += A[i][k] * B[k][j];
    

    if (rank != 0) 
    {
        for (size_t i = startRow; i < endRow; i++) 
            MPI_Send(result[i].data(), colsB, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
    } 
    else 
    {
        for (int proc = 1; proc < size; proc++) 
        {
            size_t procStart = proc * rowsPerProcess;
            size_t procEnd = (proc == size - 1) ? rowsA : procStart + rowsPerProcess;
            for (size_t i = procStart; i < procEnd; ++i)
                MPI_Recv(result[i].data(), colsB, MPI_FLOAT, proc, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    return end - start;
}

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    std::cout << "Size = " << size << std::endl;
    try {
        double totalTime = 0.0;
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

            size_t rowsA = 0, colsA = 0, rowsB = 0, colsB = 0;
            std::vector<std::vector<float>> matrixA, matrixB, result;

            if (rank == 0) {
                matrixA = readMatrix(fileA, rowsA, colsA);
                matrixB = readMatrix(fileB, rowsB, colsB);
                if (colsA != rowsB) 
                {
                    throw std::runtime_error("Matrices cannot be multiplied: colsA != rowsB");
                }
            }

            MPI_Bcast(&rowsA, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
            MPI_Bcast(&colsA, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
            MPI_Bcast(&rowsB, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);
            MPI_Bcast(&colsB, 1, MPI_UNSIGNED_LONG, 0, MPI_COMM_WORLD);

            if (rank != 0) 
            {
                matrixA.resize(rowsA, std::vector<float>(colsA));
                matrixB.resize(rowsB, std::vector<float>(colsB));
            }

            for (size_t r = 0; r < rowsA; ++r)
                MPI_Bcast(matrixA[r].data(), colsA, MPI_FLOAT, 0, MPI_COMM_WORLD);
    

            for (size_t r = 0; r < rowsB; ++r)
                MPI_Bcast(matrixB[r].data(), colsB, MPI_FLOAT, 0, MPI_COMM_WORLD);
            

            MPI_Barrier(MPI_COMM_WORLD);

            auto duration = parallelMatrixMultiply(matrixA, matrixB, result, rowsA, colsA, colsB, rank, size);
            double localTime = duration.count();
            double allTimeMul;

            MPI_Reduce(&localTime, &allTimeMul, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
            totalTime += allTimeMul;

            if (rank == 0) 
            {
                writeMatrix(resultFile, result);
                std::cout << "Lead time for " << fileA << " and " << fileB << ": " 
                          << std::chrono::duration_cast<std::chrono::microseconds>
                          (std::chrono::duration<double>(allTimeMul)).count() 
                          << " us" << std::endl;
                std::cout << "Task volume: " << rowsA << " * " << colsB << std::endl;
                std::cout << "The result is written to the file: " << resultFile << std::endl;
            }

            MPI_Barrier(MPI_COMM_WORLD);
        }

        if (rank == 0)
            std::cout << "Total lead time: " 
                      << std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::duration<double>(totalTime)).count() 
                      << " us" << std::endl;
    }
    catch (const std::exception& ex) 
    {
        if (rank == 0)
            std::cerr << "Error: " << ex.what() << std::endl;
        MPI_Finalize();
        return -1;
    }

    MPI_Finalize();
    return 0;
}