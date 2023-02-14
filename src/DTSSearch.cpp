#include "DTSSearch.hpp"
#include "Matrix.hpp"
#include <algorithm>
#include <chrono>
#include <climits>
#include <iostream>
#include <queue>
#include <random>
#include <utility>
#include <vector>
using elite_solution_container = std::priority_queue<
    std::pair<double, std::vector<bool>>,
    std::vector<std::pair<double, std::vector<bool>>>,
    std::greater<>>;
DTSSearch::DTSSearch(Matrix const &matrix, unsigned size)
    : DTSSearch(matrix, size, std::random_device()()) {}
DTSSearch::DTSSearch(Matrix const &matrix, unsigned size, unsigned seed)
    : _matrix{matrix},
      _size{size},
      output(),
      _solved{false},
      _last_solution(size, false),
      _random_engine(seed) {}
auto DTSSearch::_initialize(std::vector<bool> &current_solution) -> void
{
    std::uniform_real_distribution<> dis(0, 1.0);
    for (unsigned i = 0; i < _size; i++)
    {
        if (dis(_random_engine) > 0.5)
        {
            current_solution[i] = !current_solution[i];
        }
    }
}
auto DTSSearch::solve(unsigned time_limit) -> void
{
    this->_outer_search_loop(time_limit);
}
auto DTSSearch::_compute_row_and_column_values(const std::vector<bool> &solution_vector, std::vector<double> &row_value, std::vector<double> &column_value) -> void
{
    for (unsigned i = 0; i < _size; i++)
    {
        for (unsigned j = 0; j < i; j++)
        {
            row_value[i] += _matrix(i, j) * solution_vector[j];
            column_value[j] += _matrix(i, j) * solution_vector[i];
        }
    }
}
// tabu search
auto DTSSearch::_inner_search_loop(std::vector<bool> &final_solution, std::vector<double> &flip_count) -> double
{
    unsigned c = std::min(unsigned(20), _size / 4);
    unsigned a = 10000;                                                     // Improvement Cutoff
    std::uniform_int_distribution<std::mt19937::result_type> dist10(1, 10); // distribution in range [1, 10]
    std::vector<double> column_value(_size, 0);
    std::vector<bool> current_solution(final_solution);
    std::vector<bool> best_solution(_size, false);
    std::vector<double> row_value(_size, 0);
    this->_compute_row_and_column_values(current_solution, row_value, column_value);
    unsigned best_index = -1;
    double best_update = 0;
    double delta_i = 0;
    unsigned iteration_count = 0;
    double best_value = 0;
    double current_value = 0;
    for (unsigned i = 0; i < _size; i++)
    {
        current_value += (row_value[i] + _matrix(i, i)) * current_solution[i];
    }
    unsigned last_update = 0;
    unsigned local_search = false;
    std::vector<double> tabu_tenure(_size, 0);
    while (iteration_count <= last_update + a)
    {
        best_index = 0;
        best_update = INT_MIN;
        for (unsigned i = 0; i < _size; i++)
        {
            delta_i = (1 - 2 * current_solution[i]) * (row_value[i] + column_value[i] + _matrix(i, i));
            // if not tabu and higher delta
            if ((delta_i > best_update && (local_search || tabu_tenure[i] <= iteration_count)))
            {
                best_index = i;
                best_update = delta_i;
            }
            // found best solution
            if (current_value + delta_i > best_value)
            {
                // do local search
                local_search = true;
                best_index = i;
                best_update = delta_i;
                break;
            }
        }
        if (best_update <= 0)
        {
            local_search = false;
        }
        // update value
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
        // flip bit
        current_solution[best_index] = !current_solution[best_index];
        current_value = current_value + best_update;
        if (current_value > best_value)
        {
            best_value = current_value;
            last_update = iteration_count;
            std::copy(current_solution.begin(), current_solution.end(), best_solution.begin());
        }
        // bit become tabu
        tabu_tenure[best_index] = iteration_count + c + dist10(_random_engine);
        iteration_count++;
        flip_count[best_index]++;
    };
    std::copy(best_solution.begin(), best_solution.end(), final_solution.begin());
    return best_value;
}
auto DTSSearch::_outer_search_loop(unsigned time_limit) -> void
{
    std::chrono::high_resolution_clock::time_point start_time_ms; // Start Time
    std::chrono::duration<double, std::milli> duration_ms;        // Stop time

    std::chrono::high_resolution_clock::time_point start_time_inside; // Start Time
    std::chrono::duration<double, std::milli> duration_inside;        // Stop time

    start_time_ms = std::chrono::high_resolution_clock::now();
    std::vector<bool> current_solution(_size, false);
    std::vector<bool> best_found_solution(_size, false);
    this->_initialize(current_solution);
    elite_solution_container elite_solution;
    std::vector<double> elite_frequency(_size, 0);
    double best_found_value = INT_MIN;
    unsigned iter_count = 0;
    unsigned last_update = 0;
    // do until best solution stuck for 100 iterations
    while (iter_count < last_update + 100)
    {
        start_time_inside = std::chrono::high_resolution_clock::now();
        std::vector<double> flip_count(_size, 0);
        std::vector<double> score(_size, 0);
        auto current_value = this->_inner_search_loop(current_solution, flip_count);
        if (current_value > best_found_value)
        {
            std::copy(current_solution.begin(), current_solution.end(), best_found_solution.begin());
            best_found_value = current_value;
            last_update = iter_count;
        }
        int r = (int)elite_solution.size();
        // fill up the Elite Solution list
        if (r < 20)
        {
            for (unsigned i = 0; i < _size; i++)
            {
                if (current_solution[i] == 1)
                {
                    elite_frequency[i]++;
                }
            }
            elite_solution.emplace(current_value, std::vector<bool>(current_solution));
        }
        // substitute the worse elite solution
        else if (current_value > elite_solution.top().first)
        {
            std::vector<bool> to_go = elite_solution.top().second;
            elite_solution.pop();
            for (unsigned i = 0; i < _size; i++)
            {
                if (current_solution[i])
                {
                    elite_frequency[i]++;
                }
                if (to_go[i])
                {
                    elite_frequency[i]--;
                }
            }
            elite_solution.emplace(current_value, std::vector<bool>(current_solution));
        }
        double max_flip = *std::max(flip_count.begin(), flip_count.end());
        // score function
        for (unsigned i = 0; i < _size; i++)
        {
            score[i] = (elite_frequency[i] * (r - elite_frequency[i]) * 1.0) / (r * r) + 0.3 * (1 - flip_count[i] / max_flip);
        }
        // sort the scores
        std::vector<unsigned> idx(_size);
        std::iota(idx.begin(), idx.end(), 0);
        std::sort(idx.begin(), idx.end(),
                  [score](size_t i1, size_t i2)
                  { return score[i1] < score[i2]; });
        std::vector<double> probability(_size);
        std::uniform_real_distribution<double> distribution(0.0, 0.05);
        // assign probability
        for (unsigned i = 0; i < _size; i++)
        {
            probability[i] = pow(i + 1, -1.2) + distribution(_random_engine);
        }
        // pick critical variables
        std::discrete_distribution<unsigned> gen(probability.begin(), probability.end());
        std::vector<bool> picked(_size, false);
        int pick_count = _size / 10;
        int sum = 0;
        while (pick_count > 0)
        {
            unsigned q = gen(_random_engine);
            int index = idx[q];
            if (!picked[index])
            {
                current_solution[index] = !current_solution[index];
                pick_count--;
                picked[index] = true;
                sum += index;
            }
        }
        iter_count++;

        duration_ms = (std::chrono::high_resolution_clock::now() - start_time_ms) / 1000;
        duration_inside = (std::chrono::high_resolution_clock::now() - start_time_inside) / 1000;

        output += "Iteration: ";
        output += std::to_string(iter_count);
        output += ";Local Best: ";
        output += std::to_string(current_value);
        output += ";Current Best: ";
        output += std::to_string(best_found_value);
        output += ";time: ";
        output += std::to_string(duration_inside.count());
        output += "\n";

        if (duration_ms.count() + duration_ms.count() / iter_count >= time_limit)
        {
            break;
        }
    }
    std::copy(best_found_solution.begin(), best_found_solution.end(), _last_solution.begin());
}
auto DTSSearch::get_solution() -> std::vector<bool>
{
    return _last_solution;
}
auto DTSSearch::get_output() -> std::string
{
    return output;
}
