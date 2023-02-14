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

#include "DTSSearch.hpp"
#include "Heuristic.hpp"
#include "read_file.hpp"
#include "MSTSearch.hpp"
#include "SimAnneal.hpp"

std::vector<std::string> split(std::string s, char delim)
{
    std::vector<std::string> res;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim))
    {
        res.push_back(item);
    }

    return res;
}

double eval(Matrix &Q, const std::vector<bool> x)
{
    double result = 0;
    for (unsigned i = 0; i < x.size(); i++)
    {
        for (unsigned j = 0; j <= i; j++)
        {
            if (j < i)
            {
                result += x[i] * Q(i, j) * x[j];
            }
            else
            {
                result += x[i] * Q(i, i) * x[j];
            }
        }
    }
    return result;
}
int main(int argc, char const *argv[])
{
    std::chrono::high_resolution_clock::time_point start_time_ms; // Start Time
    std::chrono::duration<double, std::milli> duration_ms;        // Stop time

    start_time_ms = std::chrono::high_resolution_clock::now();
    if (argc < 2)
    {
        std::cout << "Please give an input file" << std::endl;
        return 0;
    }
    std::string filename = argv[1];
    std::string filename_answer = argv[2];
    std::string solver = argv[3];
    Matrix matrix = read_file_test(filename); // read matrix from QPLIB

    std::vector<bool> vect = read_answer_test(filename_answer, matrix.size()); // read best vector solution from QPLIB

    std::vector<std::string> filename_split = split(filename, '/');

    auto size = matrix.size();
    std::vector<double> sum_of_positive_off_diagonal(size, 0);
    std::vector<double> sum_of_negative_off_diagonal(size, 0);
    std::vector<int> fixed_x(size, -1);
    for (unsigned i = 0; i < size; i++)
    {
        for (unsigned j = 0; j < i; j++)
        {
            if (matrix(i, j) > 0)
            {
                sum_of_positive_off_diagonal[i] += matrix(i, j);
            }
            if (matrix(i, j) < 0)
            {
                sum_of_negative_off_diagonal[i] += matrix(i, j);
            }
        }
    }

    std::vector<bool> result;
    unsigned time = 600;
    if (argc == 5)
    {
        time = std::stoi(argv[4]);
    }

    // matrix = lower_triagular_matrix(matrix, size); // make it lower triangular
    if (solver == "DTSS")
    {
        Heuristic *tabu_search = new DTSSearch{matrix, size};
        tabu_search->solve(time);
        result = tabu_search->get_solution();
        std::cout << solver << " " << filename_split[filename_split.size() - 1] << std::endl;
        std::cout << tabu_search->get_output() << std::endl;
    }
    else if (solver == "MSTS")
    {
        Heuristic *tabu_search = new MSTSearch{matrix, size};
        tabu_search->solve(time);
        result = tabu_search->get_solution();
        std::cout << solver << " " << filename_split[filename_split.size() - 1] << std::endl;
        std::cout << tabu_search->get_output() << std::endl;
    }
    else if (solver == "SA")
    {
        Heuristic *tabu_search = new SimAnneal{matrix, size};
        tabu_search->solve(time);
        result = tabu_search->get_solution();
        std::cout << solver << " " << filename_split[filename_split.size() - 1] << std::endl;
        std::cout << tabu_search->get_output() << std::endl;
    }

    duration_ms = std::chrono::high_resolution_clock::now() - start_time_ms;

    /*
    If we want to write the best solution vector in the output
    for (int i = 0; i < result.size(); i++)
    {
        std::cout << result[i] << " ";
    }
    std::cout << "\n" << std::endl;
    */

    std::cout << filename_split[filename_split.size() - 1] << " "
              << matrix.size() << " "
              << read_file_nonzero(filename) << " "
              << std::setprecision(0) << std::fixed << eval(matrix, result) << " "
              << std::setprecision(2) << std::fixed << duration_ms.count() / 1000 << "s "
              << std::setprecision(0) << std::fixed << eval(matrix, vect) << " "
              << read_answer_best(filename_answer) // read best objective value from QPLIB
              << std::endl;
}
