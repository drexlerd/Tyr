/*
 * Copyright (C) 2025 Dominik Drexler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_CHRONO_HPP_
#define TYR_COMMON_CHRONO_HPP_

#include <chrono>

namespace tyr
{

template<typename Rep, typename Period>
[[nodiscard]] inline std::chrono::milliseconds::rep to_ms(std::chrono::duration<Rep, Period> d) noexcept
{
    return std::chrono::duration_cast<std::chrono::duration<std::chrono::milliseconds::rep, std::milli>>(d).count();
}

template<typename T>
struct StopwatchScope
{
    StopwatchScope(T& cur_time) : m_cur_time(cur_time), m_start(std::chrono::steady_clock::now()) {}

    ~StopwatchScope() { m_cur_time += std::chrono::duration_cast<T>(std::chrono::steady_clock::now() - m_start); }

    T& m_cur_time;
    std::chrono::steady_clock::time_point m_start;
};

class CountdownWatch
{
private:
    std::chrono::steady_clock::time_point m_deadline;

public:
    template<typename Rep, typename Period>
    explicit CountdownWatch(std::chrono::duration<Rep, Period> timeout) :
        m_deadline(std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::steady_clock::duration>(timeout))
    {
    }

    bool has_finished() const { return std::chrono::steady_clock::now() >= m_deadline; }
};

}

#endif