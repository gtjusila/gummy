/*
    reading the matrix and solution from QPLIB
*/
#include "Matrix.hpp"
#include <algorithm>
#include <bitset>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <random>
#include <sstream>
#include <string>
#include <vector>

/// @brief Count numbers of nonzero elements in square matrix
/// @param matrix Matrix Q
/// @return Number of nonzero elements in square matrix
int count_nonzero_all(std::vector<std::vector<double>> matrix)
{
    double sum = 0;
    for (unsigned long i = 0; i < matrix.size(); i++)
    {
        for (unsigned long j = 0; j < matrix.size(); j++)
        {
            if (int(matrix[i][j]) != 0)
            {
                sum += 1;
            }
        }
    }
    return sum;
}

/// @brief Count numbers of nonzero elements in matrix diagonal
/// @param matrix Matrix Q
/// @return Number of nonzero elements in matrix diagonal
int count_nonzero_diag(std::vector<std::vector<double>> matrix)
{
    double sum = 0;
    for (unsigned long i = 0; i < matrix.size(); i++)
    {
        if (int(matrix[i][i]) != 0)
        {
            sum += 1;
        }
    }
    return sum;
}

/// @brief Generate square zero matrix
/// @param n Matrix size
/// @return Square zero matrix
std::vector<std::vector<double>> create_zero_matrix(unsigned n)
{
    std::normal_distribution<double> generator{0, 1};
    std::vector<std::vector<double>> result;
    for (unsigned i = 0; i < n; ++i)
    {
        std::vector<double> sub_list(n, 0);
        result.push_back(sub_list);
    }
    return result;
}

/// @brief Read file from Prof. Koch instances
/// @param filepath Path of file
/// @return Matrix
std::vector<std::vector<double>> read_file(std::string filepath)
{
    std::ifstream input_file(filepath);

    unsigned row_count;
    unsigned entry_count;
    input_file >> row_count >> entry_count;
    std::vector<std::vector<double>> matrix = create_zero_matrix(row_count);
    for (unsigned i = 0; i < entry_count; ++i)
    {
        unsigned row;
        unsigned column;
        double value;
        input_file >> row >> column >> value;
        matrix[row - 1][column - 1] = value;
    }
    return matrix; // Exploit Copy Elision
}

/// @brief Read file from QPLIB instances
/// @param filepath Path of file
/// @return Matrix
Matrix read_file_test(std::string filepath)
{
    Matrix matrix;

    std::ifstream input_file(filepath);

    int iteration = 0;
    int row_count;
    int nonlinear_count;
    double nonlinear_value;
    int linear_count;
    std::string line;
    std::string str;

    while (getline(input_file, line))
    {
        iteration++;
        if (iteration < 4)
        {
            continue;
        }
        if (iteration == 4)
        {
            std::stringstream ss;
            ss.str(line);
            ss >> row_count;
            matrix = Matrix(row_count);
        }
        if (iteration == 5)
        {
            std::stringstream ss;
            ss.str(line);
            ss >> nonlinear_count;
        }
        if (iteration > 5 and iteration <= 5 + nonlinear_count)
        {
            std::stringstream ss;
            int row;
            int column;
            double value;
            ss.str(line);
            ss >> row >> column >> value;
            matrix(row - 1, column - 1) = value / 2;
            if (row - 1 < column - 1)
            {
                std::cout << "Ouch" << std::endl;
            }
        }
        if (iteration == 5 + nonlinear_count + 1)
        {
            std::stringstream ss;
            ss.str(line);
            ss >> nonlinear_value;
            for (int i = 0; i < row_count; ++i)
            {
                matrix(i, i) = nonlinear_value;
            }
        }
        if (iteration == 5 + nonlinear_count + 2)
        {
            std::stringstream ss;
            ss.str(line);
            ss >> linear_count;
        }

        if (iteration > 5 + nonlinear_count + 2 and iteration <= 5 + nonlinear_count + 2 + linear_count)
        {
            std::stringstream ss;
            int row;
            double value;
            ss.str(line);
            ss >> row >> value;
            matrix(row - 1, row - 1) = value;
        }
    }

    return matrix;
}

/// @brief Calculate number of nonzero elements in qs file
/// @param filepath File path
/// @return Number of nonzero elements (in double..???)
double read_file_nonzero(std::string filepath)
{
    double nonzero = 0;
    std::vector<std::vector<double>> matrix;

    std::ifstream input_file(filepath);

    int iteration = 0;
    int row_count;
    int nonlinear_count;
    int linear_count;
    std::string line;
    std::string str;

    while (getline(input_file, line))
    {
        iteration++;
        if (iteration < 4)
        {
            continue;
        }
        if (iteration == 4)
        {
            std::stringstream ss;
            ss.str(line);
            ss >> row_count;
            matrix = create_zero_matrix(row_count);
        }
        if (iteration == 5)
        {
            std::stringstream ss;
            ss.str(line);
            ss >> nonlinear_count;
            nonzero += nonlinear_count;
        }
        if (iteration > 5 and iteration <= 5 + nonlinear_count)
        {
            continue;
        }
        if (iteration == 5 + nonlinear_count + 2)
        {
            std::stringstream ss;
            ss.str(line);
            ss >> linear_count;
            nonzero += linear_count;
        }

        if (iteration > 5 + nonlinear_count + 2 and iteration <= 5 + nonlinear_count + 2 + linear_count)
        {
            continue;
        }
    }

    return nonzero / (row_count * row_count);
}

/// @brief Read objective vector for .qs files
/// @param filepath filepath
/// @param size size of problem
/// @return Solution vector
std::vector<bool> read_answer_test(std::string filepath, int size)
{
    std::ifstream input_file(filepath);
    std::vector<bool> vect(size, false);
    std::string line;
    int iteration = 0;

    while (getline(input_file, line))
    {
        iteration++;
        if (iteration < 2)
        {
            continue;
        }
        else
        {
            std::stringstream ss;
            std::string loc;
            std::string loc_1;
            double value;
            ss.str(line);
            ss >> loc >> value;
            for (unsigned long j = 0; j < loc.size(); j++)
            {
                if (j != 0)
                {
                    loc_1 = loc_1 + loc[j];
                }
            }
            vect[stoi(loc_1) - 2] = true;
        }
    }
    return vect;
}

/// @brief Read objective vector for .qs files
/// @param filepath filepath
/// @return Solution vector
double read_answer_best(std::string filepath)
{
    std::ifstream input_file(filepath);
    std::string obj;
    double ans;
    input_file >> obj >> ans;

    return ans; // Exploit Copy Elision
}
