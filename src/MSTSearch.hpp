/**
 * @file MTSSearch.hpp
 * @author Michael Adipoetra
 * @brief A Multistart tabu search heuristic solver class.
 */
#ifndef MSTSearch_H
#define MSTSearch_H

#include "Heuristic.hpp"
#include "Matrix.hpp"
#include <random>
#include <vector>
class MSTSearch : public Heuristic
{
public:
    MSTSearch(Matrix const &matrix, unsigned size);
    MSTSearch(Matrix const &matrix, unsigned size, unsigned seed);
    void solve(unsigned time_limit) override;
    std::vector<bool> get_solution() override;
    std::string get_output() override;

private:
    unsigned _size;
    double _best_value;
    double _current_value;
    Matrix const &_matrix;
    std::vector<double> _row_value;
    std::vector<double> _column_value;
    std::vector<double> _delta_x;
    std::vector<bool> _current_solution;
    std::vector<bool> _best_solution;
    std::mt19937 _random_engine;
    std::string output;

    auto MSTS(unsigned time_limit) -> void;
    auto initialize() -> void;
    auto recompute_internal_variables() -> void;
    auto STS() -> void;
    auto eval_search() -> std::vector<unsigned>;
    auto eval_descent(std::vector<unsigned> I_star) -> std::vector<unsigned>;
};
#endif