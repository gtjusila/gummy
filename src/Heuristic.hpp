#ifndef Heuristic_H
#define Heuristic_H
#include <vector>
#include <string>
/// @brief Abstract Heuristic class
class Heuristic
{
public:
    /// @brief Solve Qubo problem with heuristic
    /// @param time_limit Computation time limit
    virtual void solve(unsigned time_limit) = 0;
    /// @brief Get current solution
    /// @return Solution vector
    virtual std::vector<bool> get_solution() = 0;
    /// @brief Get iteration data string
    /// @return Iteration data string
    virtual std::string get_output() = 0;
};
#endif