
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>


std::vector<std::vector<int>> readMatrix(const std::string& filename, int& rows, int& cols) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file " + filename);
    }

    std::vector<std::vector<int>> matrix;
    rows = cols = 0;
    std::vector<int> row;

    while (file.good()) 
    {
        char c = file.get();
        if (!file.good()) 
        {
            if (!row.empty()) 
            {
                // last line
                matrix.push_back(row);
                cols = std::max(cols, static_cast<int>(row.size()));
                rows++;
            }
            break;
        }

        if (std::isdigit(c))
            row.push_back(c - '0');
        else if (c == '\n') 
        {
            if (!row.empty()) 
            {
                matrix.push_back(row);
                cols = std::max(cols, static_cast<int>(row.size()));
                rows++;
                row.clear();
            }
        }
    }

    file.close();
    if (rows == 0)
        throw std::runtime_error("File " + filename + " is empty");

    for (int i = 0; i < rows; ++i) 
    {
        while (matrix[i].size() < cols) 
        {
            matrix[i].push_back(0);
        }
    }

    return matrix;
}


int main()
{
    std::string fileA = "../Matrix1.txt";
    std::string fileB = "../Matrix2.txt";
    int rowsA, colsA, rowsB, colsB;
    auto matrixA = readMatrix(fileA, rowsA, colsA);
    auto matrixB = readMatrix(fileB, rowsB, colsB);
    

    return 0;
}