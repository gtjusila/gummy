#include "MSTSearch.hpp"
#include "Matrix.hpp"
#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <climits>
#include <chrono>
#include <fstream>

MSTSearch::MSTSearch(Matrix const &matrix, unsigned size)
    : MSTSearch(matrix, size, std::random_device()()) {}

MSTSearch::MSTSearch(Matrix const &matrix, unsigned size, unsigned seed)
    : _matrix{matrix},
      _size{size},
      output(),
      _delta_x(size, 0),
      _row_value(size, 0),
      _column_value(size, 0),
      _current_solution(size, false),
      _best_solution(size, false),
      _random_engine(seed) {}

auto MSTSearch::initialize() -> void
{
    std::uniform_real_distribution<> dis(0, 1.0);
    for (unsigned i = 0; i < _size; i++)
    {
        if (dis(_random_engine) > 0.5)
        {
            _current_solution[i] = !_current_solution[i];
        }
    }
    this->recompute_internal_variables();
}
auto MSTSearch::recompute_internal_variables() -> void
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

// STS
auto MSTSearch::STS() -> void
{
    unsigned c = fmin(20, _size / 4);
    unsigned a = 10000;     
    std::uniform_int_distribution<std::mt19937::result_type> dist10(1, 10); // distribution in range [1, 10]
    std::vector<bool> current_solution(_current_solution);
    std::vector<bool> best_solution(_current_solution);
    std::vector<double> column_value(_column_value);
    std::vector<double> row_value(_row_value);

    unsigned best_index = -1;
    double best_update = 0;
    double delta_i;
    unsigned last_update = 0;
    int rho;

    // Step 1
    unsigned iteration_count = 0;
    double best_value = _current_value;
    double current_value = _current_value;
    std::vector<double> tabu_tenure(_size, 0);

    while (iteration_count <= last_update + a)
    {
        // Step 2
        best_index = -1;
        best_update = INT_MIN;
        rho = 0;

        // Step 3
        for (unsigned i = 0; i < _size; i++)
        {
            // Step 3.1
            // if this variable is tabu go to next variable
            if (tabu_tenure[i] > iteration_count)
            {
                continue;
            }

            // Step 3.3
            // calculate delta
            delta_i = (1 - 2 * current_solution[i]) * (row_value[i] + column_value[i] + _matrix(i, i));
            // if best solution found
            if (current_value + delta_i > best_value)
            {
                best_index = i;
                best_update = delta_i;
                rho = 1;
                break;
            }

            // Step 3.4
            // find the highest delta
            if (delta_i > best_update)
            {
                best_index = i;
                best_update = delta_i;
            }
        }

        // Step 4
        // flip the bit
        for (unsigned i = 0; i < _size; i++)
        {
            if (i < best_index)
            {
                column_value[i] += (1 - 2 * current_solution[best_index]) * _matrix(best_index, i);
            }
            else if (i > best_index)
            {
                row_value[i] += (1 - 2 * current_solution[best_index]) * _matrix(i, best_index);
            }
        }
        current_solution[best_index] = !current_solution[best_index];
        current_value = current_value + best_update;

        // Step 5
        // do greedy local search if best solution found
        while (rho == 1)
        {
            // Local Search

            // Step 5.1
            rho = 0;

            // Step 5.2
            for (unsigned r = 0; r < _size; r++)
            {
                // Step 5.2.1
                ++(iteration_count);

                // Step 5.2.2
                double delta_i_local = (1 - 2 * current_solution[r]) * (row_value[r] + column_value[r] + _matrix(r, r));
                if (delta_i_local > 0)
                {
                    for (unsigned i = 0; i < _size; i++)
                    {
                        if (i < r)
                        {
                            column_value[i] += (1 - 2 * current_solution[r]) * _matrix(r, i);
                        }
                        else if (i > r)
                        {
                            row_value[i] += (1 - 2 * current_solution[r]) * _matrix(i, r);
                        }
                    }
                    current_solution[r] = !current_solution[r];
                    current_value = current_value + delta_i_local;
                    rho = 1;
                }
            }

            best_value = current_value;
            last_update = iteration_count;
            std::copy(current_solution.begin(), current_solution.end(), best_solution.begin());

            // Step 5.3
            if (rho == 0)
            {
                break;
            }
        }

        // Step 6
        // the variable becomes tabu
        tabu_tenure[best_index] = iteration_count + c + dist10(_random_engine);

        if (iteration_count > last_update + a)
        {
            break;
        }
        iteration_count++;
    }
    _current_value = best_value;
    std::copy(best_solution.begin(), best_solution.end(), _current_solution.begin());
}

// SELECT_VARIABLES
auto MSTSearch::eval_search() -> std::vector<unsigned>
{
    std::vector<bool> current_solution(_current_solution);
    std::vector<double> column_value(_column_value);
    std::vector<double> row_value(_row_value);

    double alpha = 0.4;
    unsigned n_prime = fmax(10, floor(alpha * _size));
    n_prime = fmin(n_prime, _size);
    double lambda = 5000;

    std::vector<unsigned> I_star;
    std::vector<double> d(_size, 0);
    std::vector<bool> I(_size, true);
    std::vector<double> e;
    std::vector<int> e_index;

    // Step 1
    for (unsigned i = 0; i < _size; ++i)
    {
        d[i] = (1 - 2 * current_solution[i]) * (row_value[i] + column_value[i] + _matrix(i, i));
    }

    // Step 2
    // score function
    for (unsigned i = 0; i < n_prime; ++i)
    {
        double d_min = INT_MAX;
        double d_max = INT_MIN;
        for (unsigned j = 0; j < _size; ++j)
        {
            if (I[j] == true)
            {
                d_max = fmax(d_max, d[j]);
                d_min = fmin(d_min, d[j]);
            }
        }

        if (d_max - d_min < 1e-6)
        {
            for (unsigned j = 0; j < _size; ++j)
            {
                // check if this variable is picked
                if (I[j] == true)
                {
                    e.push_back(1);
                    e_index.push_back(j);
                }
            }
        }
        else
        {
            for (unsigned j = 0; j < _size; ++j)
            {
                // check if this variable is picked
                if (I[j] == true)
                {
                    e_index.push_back(j);
                    if (d[j] <= 0 and d_min < 0)
                    {
                        e.push_back(1 - d[j] / d_min);
                    }
                    else if (d[j] == d_min and d_min == 0)
                    {
                        e.push_back(0);
                    }
                    else
                    {
                        e.push_back(1 + lambda * (d[j] / d_max));
                    }
                }
            }
        }

        // Step 3
        // assign probability
        double e_sum = 0;
        for (unsigned i = 0; i < e.size(); ++i)
        {
            e_sum += e[i];
        }
        std::vector<double> e_prob;
        for (unsigned i = 0; i < e.size(); ++i)
        {
            e_prob.push_back(e[i] / e_sum);
        }

        std::mt19937 gen(1);

        std::discrete_distribution<std::size_t> roll{e_prob.begin(), e_prob.end()};

        unsigned k = e_index[roll(gen)];

        // Step 4
        // pick variable
        I_star.push_back(k);
        I[k] = false;

        // update value
        for (unsigned i = 0; i < _size; i++)
        {
            if (I[i] == true)
            {
                double testing_1 = 0;
                double testing_2 = 0;
                if (current_solution[i] == current_solution[k])
                {
                    testing_1 = _matrix(i, k);
                    testing_2 = _matrix(k, i);
                }
                else
                {
                    testing_1 = -_matrix(i, k);
                    testing_2 = -_matrix(k, i);
                }
                double testing = testing_1 + testing_2;

                d[i] += testing;
            }
        }
    }
    return I_star;
}

// STEEPEST ASCENT
auto MSTSearch::eval_descent(std::vector<unsigned> I_star) -> std::vector<unsigned>
{
    std::vector<bool> current_solution(_current_solution);
    std::vector<double> column_value(_column_value);
    std::vector<double> row_value(_row_value);

    std::vector<double> h1(_size, 0);
    std::vector<double> h2(_size, 0);
    std::vector<double> q1(_size, 0);
    std::vector<double> q2(_size, 0);
    double r;
    double j;
    double v;
    std::vector<unsigned> flip(_size, 0);
    std::vector<unsigned> flip_test;

    // Step 1
    for (unsigned i = 0; i < I_star.size(); ++i)
    {
        unsigned index = I_star[i];
        double delta = (1 - 2 * current_solution[index]) * (row_value[index] + column_value[index] + _matrix(index, index));
        h1[index] = delta;
        h2[index] = 0;

        for (unsigned j = 0; j < I_star.size(); ++j)
        {
            unsigned index_inside = I_star[j];
            if (index == index_inside)
            {
                continue;
            }
            if (current_solution[index] == current_solution[index_inside])
            {
                h2[index] += 2 * ((_matrix(index, index_inside) + _matrix(index, index_inside)) / 2);
            }
            else
            {
                h2[index] -= 2 * ((_matrix(index, index_inside) + _matrix(index, index_inside)) / 2);
            }
        }
    }

    std::vector<bool> in_I_star(_size, false);
    for (unsigned i = 0; i < I_star.size(); ++i)
    {
        unsigned index = I_star[i];
        in_I_star[index] = true;
    }

    // Step 2
    for (unsigned iter = 0; iter < I_star.size(); ++iter)
    {
        // Step 2.1
        double V1 = INT_MIN;
        double V2 = INT_MIN;

        // Step 2.2
        for (unsigned i = 0; i < I_star.size(); ++i)
        {
            unsigned index = I_star[i];

            if (in_I_star[index] == false)
            {
                continue;
            }

            // Step 2.2.1
            q1[index] = 2 * h1[index] + h2[index];
            q2[index] = h1[index];

            // Step 2.2.2
            // find r value
            if (q1[index] > 0 or (q1[index] == 0 and q2[index] >= 0))
            {
                r = 1;
            }
            else
            {
                r = 0;
                q1[index] = -q1[index];
                q2[index] = -q2[index];
            }

            // Step 2.2.3
            // find the highest ascent
            if (q1[index] > V1 or (q1[index] == V1 and q2[index] > V2))
            {
                V1 = q1[index];
                V2 = q2[index];
                j = index;
                v = r;
            }
        }

        // Step 2.3
        // pick the variable with the highest ascent
        flip[j] = v;
        in_I_star[j] = false;
        if (v == 1)
        {
            flip_test.push_back(j);
        }

        I_star.erase(std::find(I_star.begin(), I_star.end(), j));

        // update value
        for (unsigned i = 0; i < I_star.size(); ++i)
        {
            unsigned index = I_star[i];
            if (in_I_star[i] == false)
            {
                continue;
            }

            double to_add = 0;
            if (current_solution[index] == current_solution[j])
            {
                to_add = (_matrix(index, j) + _matrix(j, index)) / 2;
            }
            else
            {
                to_add = - (_matrix(index, j) + _matrix(j, index)) / 2;
            }
            h2[index] -= 2 * to_add;

            if (v == 1)
            {
                h1[index] += 2 * to_add;
            }
        }
    }
    return flip;
}

// MST2
auto MSTSearch::MSTS(unsigned time_limit) -> void
{
    std::chrono::high_resolution_clock::time_point start_time_ms; // Start Time
    std::chrono::duration<double, std::milli> duration_ms;        // Stop time

    std::chrono::high_resolution_clock::time_point start_time_inside; // Start Time
    std::chrono::duration<double, std::milli> duration_inside;        // Stop time

    start_time_ms = std::chrono::high_resolution_clock::now();

    // std::cout << "Iteration " << iteration_count << "Current Best" << _best_value << std::endl;

    // Step 1
    this->initialize();
    start_time_inside = std::chrono::high_resolution_clock::now();

    // Step 2
    this->STS();
    this->recompute_internal_variables();
    // std::cout << "Best Local Value " << _current_value << std::endl;
    if (_current_value > _best_value)
    {
        _best_value = _current_value;
        std::copy(_current_solution.begin(), _current_solution.end(), _best_solution.begin());
    }

    unsigned iter_count = 0;
    unsigned last_update = 0;

    // Step 3
    while (iter_count < last_update + 100)
    {
        if (iter_count > 0)
        {
            start_time_inside = std::chrono::high_resolution_clock::now();
        }   
        // Step 4
        std::vector<unsigned> I_star;
        I_star = this->eval_search();

        // Step 5
        std::vector<unsigned> flip = this->eval_descent(I_star);

        // Step 6
        double summ = 0;
        for (unsigned i = 0; i < flip.size(); ++i)
        {
            unsigned index = i;
            if (flip[index] == 1)
            {
                double best_update = (1 - 2 * _current_solution[index]) * (_row_value[index] + _column_value[index] + _matrix(index, index));
                _current_solution[index] = !_current_solution[index];
                _current_value += best_update;
                summ += 1;
            }
        }
        this->recompute_internal_variables();

        // Step 7
        this->STS();
        this->recompute_internal_variables();

        if (_current_value > _best_value)
        {
            _best_value = _current_value;
            last_update = iter_count;
            std::copy(_current_solution.begin(), _current_solution.end(), _best_solution.begin());
        }
        
        ++ iter_count;

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

auto MSTSearch::solve(unsigned time_limit) -> void
{
    this->MSTS(time_limit);
}

auto MSTSearch::get_solution() -> std::vector<bool>
{
    return _best_solution;
}

auto MSTSearch::get_output() -> std::string
{
    return output;
}