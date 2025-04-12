
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>


std::vector<std::vector<int>> readMatrix(const std::string& filename, int& rows, int& cols)
{
    std::ifstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Cannot open file " + filename);
    }
    std::vector<std::vector<int>> matrix;
    rows = cols = 0;
    std::string line;

    while (getline(file, line))
    {
        std::stringstream ss(line);
        std::vector<int> row;
        int value;
        while (ss >> value)
        {
            row.push_back(value);
        }
        if (row.empty())
            continue;
        matrix.push_back(row);
        cols = std::max(cols, static_cast<int>(row.size()));
    }

    file.close();
    if (rows == 0)
    {
        throw std::runtime_error("File " + filename + " is empty");
    }

    for (int i = 0; i < rows; i++)
    {
        while (matrix[i].size() < cols)
            matrix[i].push_back(0);
    }
    return matrix;
}

int main()
{
    std::cout << "123" << std::endl;
    return 0;
}