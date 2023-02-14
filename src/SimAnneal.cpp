#include "SimAnneal.hpp"
#include "Matrix.hpp"
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <climits>
#include <chrono>

SimAnneal::SimAnneal(Matrix const &matrix, unsigned size)
    : _matrix{matrix},
      _size{size},
      output(),
      _delta_x(size, 0),
      _row_value(size, 0),
      _column_value(size, 0),
      _current_solution(size, false),
      _best_solution(size, false),
      _random_engine(std::random_device()())
{
    _iter_count = 1000;
    _anneal_count = 10;
    _t_factor = 0.99;
    _init_t_factor = 1;
    _init_t = 0.1 * _size;
}

SimAnneal::SimAnneal(Matrix const &matrix, unsigned size, double Tinit, double Tfactor, unsigned annealCount, double TinitFactor, unsigned iterCount)
    : _matrix{matrix},
      _size{size},
      _delta_x(size, 0),
      _row_value(size, 0),
      _column_value(size, 0),
      _current_solution(size, false),
      _best_solution(size, false),
      _random_engine(std::random_device()())
{
    _iter_count = iterCount;
    _anneal_count = annealCount;
    _t_factor = Tfactor;
    _init_t_factor = TinitFactor;
    _init_t = Tinit;
}

auto SimAnneal::_initialize() -> void
{
    std::uniform_real_distribution<> dis(0, 1.0);
    for (unsigned i = 0; i < _size; i++)
    {
        if (dis(_random_engine) > 0.5)
        {
            _current_solution[i] = !_current_solution[i];
        }
    }
    this->_recompute_internal_variables();
}

auto SimAnneal::_recompute_internal_variables() -> void
{
    std::fill(_row_value.begin(), _row_value.end(), 0);
    std::fill(_column_value.begin(), _column_value.end(), 0);
    for (unsigned i = 0; i < _size; i++)
    {
        for (unsigned j = 0; j < i; j++)
        {
            _row_value[i] += _matrix(i, j) * _current_solution[j];
            _column_value[j] += _matrix(i, j) * _current_solution[i];
        }
    }
    _current_value = 0;
    for (unsigned i = 0; i < _size; i++)
    {
        _current_value += (_row_value[i] + _matrix(i, i)) * _current_solution[i];
    }
    for (unsigned i = 0; i < _size; i++)
    {
        _delta_x[i] = (1 - 2 * _current_solution[i]) * (_row_value[i] + _column_value[i] + _matrix(i, i));
    }
}

auto SimAnneal::solve(unsigned time_limit) -> void
{
    std::chrono::high_resolution_clock::time_point start_time_ms; // Start Time
    std::chrono::duration<double, std::milli> duration_ms;        // Stop time

    std::chrono::high_resolution_clock::time_point start_time_inside; // Start Time
    std::chrono::duration<double, std::milli> duration_inside;        // Stop time

    start_time_ms = std::chrono::high_resolution_clock::now();

    // Step 1
    double Tinit = _init_t;
    double TFactor = _t_factor;
    unsigned TermCount = _anneal_count;
    unsigned SACount = _iter_count;
    double StartTFactor = _init_t_factor;

    // Step 2
    this->_initialize();
    std::copy(_current_solution.begin(), _current_solution.end(), _best_solution.begin());
    _best_value = _current_value;

    unsigned iter_count = 0;

    // Step 3
    while (iter_count < SACount)
    {
        start_time_inside = std::chrono::high_resolution_clock::now();

        // Step 3.2
        this->_simulated_annealing(Tinit, TFactor, TermCount);

        // Step 3.3
        // cool down the temperature
        Tinit = StartTFactor * Tinit;

        ++iter_count;

        //std::cout << "Iteration: " << iter_count << " Current Best: " << _best_value << endl;

        duration_ms = (std::chrono::high_resolution_clock::now() - start_time_ms) / 1000;
        duration_inside = (std::chrono::high_resolution_clock::now() - start_time_inside) / 1000;

        output += "Iteration: ";
        output += std::to_string(iter_count);
        output += ";Local Best: ";
        output += std::to_string(_current_value);
        output += ";Current Best: ";
        output += std::to_string(_best_value);
        output += ";time: ";
        output += std::to_string(duration_inside.count());
        output += "\n";

        if (duration_ms.count() + duration_ms.count() / iter_count >= time_limit)
        {
            break;
        }
    }
}

auto SimAnneal::_simulated_annealing(double Tinit, double Tfactor, unsigned TermCount) -> void
{
    std::vector<unsigned> perm(_size);
    std::uniform_real_distribution<> dis(0, 1.0);
    std::iota(perm.begin(), perm.end(), 0);

    // Step 1
    double T = Tinit;
    unsigned Counter = 0;

    // Step 3
    while (Counter < TermCount)
    {
        // Step 3.1
        Counter += 1;

        // Step 3.2
        std::shuffle(perm.begin(), perm.end(), std::mt19937(_random_engine));

        // Step 3.3
        for (unsigned j = 0; j < _size; j++)
        {
            // Step 3.3.1
            int k = perm[j];

            // Step 2
            double delta = (1 - 2 * _current_solution[k]) * (_row_value[k] + _column_value[k] + _matrix(k, k));

            // Step 3.3.2
            if (delta > 0)
            {
                Counter = 0;
                _current_solution[k] = !_current_solution[k];
                _current_value += delta;
                this->_recompute_internal_variables();

                if (_current_value > _best_value)
                {
                    _best_value = _current_value;
                    std::copy(_current_solution.begin(), _current_solution.end(), _best_solution.begin());
                }
            }

            // Step 3.3.3
            else
            {
                std::mt19937 gen(1);
                if (dis(gen) < exp(delta / T))
                {
                    _current_solution[k] = !_current_solution[k];
                    _current_value += delta;
                    this->_recompute_internal_variables();
                }
            }
        }

        // Step 3.4
        // cool down the temperature
        T = Tfactor * T;
    }
}

auto SimAnneal::get_solution() -> std::vector<bool>
{
    return _best_solution;
}

auto SimAnneal::get_output() -> std::string
{
    return output;
}