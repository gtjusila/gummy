/**
 * @file DTSSearch.hpp
 * @author Gennesaret Tjusila
 * @brief A Diversification-driven tabu searc heuristic solver class
 */
#ifndef DTSSearch_H
#define DTSSearch_H

#include "Heuristic.hpp"
#include "Matrix.hpp"
#include <random>
#include <vector>

/// @brief Diversification-driven tabu search class
class DTSSearch : public Heuristic
{
public:
    /// @brief Diversification-driven tabu search instantiaiton
    /// @param matrix Q matrix
    /// @param size Problem size
    /// @param seed RNG seed
    DTSSearch(Matrix const &matrix, unsigned size, unsigned seed);
    /// @brief Diversification-driven tabu search instantiaiton
    /// @param matrix Q matrix
    /// @param size Problem size
    DTSSearch(Matrix const &matrix, unsigned size);
    /// @brief Solve QUBO problem with DTS heuristics.
    /// @param time_limit Computation time limit
    void solve(unsigned time_limit) override;
    /// @brief Get current running solution
    /// @return Current solution
    std::vector<bool> get_solution() override;
    /// @brief Generate data string of current run
    /// @return Data string of iteration run
    std::string get_output() override;

private:
    std::string output;
    unsigned _size;
    bool _solved;
    Matrix const &_matrix;
    std::vector<bool> _last_solution;
    std::mt19937 _random_engine;
    void
    _initialize(std::vector<bool> &current_solution);
    void
    _compute_row_and_column_values(const std::vector<bool> &solution_vector, std::vector<double> &row_value,
                                   std::vector<double> &column_value);
    auto _inner_search_loop(std::vector<bool> &final_solution, std::vector<double> &flip_count) -> double;
    auto _outer_search_loop(unsigned time_limit) -> void;
};
#endif