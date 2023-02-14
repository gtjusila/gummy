/**
 * @file Matrix.hpp
 * @author Gennesaret Tjusila
 * @brief Description for a Matrix Class
 */
#ifndef Matrix_H
#define Matrix_H
#include <vector>

/// @brief A Square matrix class
class Matrix
{
public:
    Matrix(std::vector<std::vector<double>> data, unsigned n);
    Matrix(unsigned n);
    Matrix();
    unsigned size()
    {
        return _size;
    }
    double &operator()(unsigned row, unsigned column);
    double const &operator()(unsigned row, unsigned column) const;

private:
    std::vector<std::vector<double>> _data;
    unsigned _size;
};
#endif