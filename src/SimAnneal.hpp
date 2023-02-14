/**
 * @file SimAnneal.hpp
 * @author Arya Prasetya, Michael Adipoetra
 * @brief A simulated annealing heuristic solver class.
 */
#ifndef SimAnneal_H
#define SimAnneal_H

#include "Heuristic.hpp"
#include "Matrix.hpp"
#include <random>

class SimAnneal : public Heuristic
{

public:
    SimAnneal(Matrix const &matrix, unsigned size);
    SimAnneal(Matrix const &matrix, unsigned size, double Tinit, double Tfactor, unsigned AnnealCount, double TinitFactor, unsigned iterCount);
    void solve(unsigned time_limit) override;
    std::vector<bool> get_solution() override;
    std::string get_output() override;

private:
    std::string output;
    auto _initialize() -> void;
    /// @brief Simulated annealing search
    /// @param Tinit Initial temperature
    /// @param Tfactor Cooling factor
    /// @param TermCount Cooling duration count
    auto _simulated_annealing(double Tinit, double Tfactor, unsigned TermCount) -> void;
    auto _recompute_internal_variables() -> void;
    double _init_t;
    double _init_t_factor;
    double _t_factor;
    unsigned _iter_count;
    unsigned _anneal_count;

    unsigned _size;
    double _best_value;
    double _current_value;
    double _previous;
    Matrix const &_matrix;
    std::vector<double> _row_value;
    std::vector<double> _column_value;
    std::vector<double> _delta_x;
    std::vector<bool> _current_solution;
    std::vector<bool> _best_solution;
    std::mt19937 _random_engine;
};

#endif // !SimAnneal_H