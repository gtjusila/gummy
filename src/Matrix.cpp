#include "Matrix.hpp"
#include <vector>

Matrix::Matrix(std::vector<std::vector<double>> data, unsigned n) : _data{data}, _size{n} {};
Matrix::Matrix(unsigned n) : _data(n, std::vector<double>(n, 0)), _size{n} {};
Matrix::Matrix() : _data{}, _size{0} {};
double &Matrix::operator()(unsigned row, unsigned column)
{
    return _data[row][column];
}
double const &Matrix::operator()(unsigned row, unsigned column) const
{
    return _data[row][column];
}
