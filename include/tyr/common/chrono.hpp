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
    std::chrono::milliseconds m_timeout;
    std::chrono::steady_clock::time_point m_endTime;
    bool m_isRunning;

public:
    explicit CountdownWatch(uint32_t timeout_ms) : m_timeout(timeout_ms), m_isRunning(false) {}

    void start()
    {
        m_endTime = std::chrono::steady_clock::now() + m_timeout;
        m_isRunning = true;
    }

    bool has_finished() const
    {
        if (!m_isRunning)
        {
            return true;  // If never started, assume finished.
        }
        return std::chrono::steady_clock::now() >= m_endTime;
    }
};

}

#endif