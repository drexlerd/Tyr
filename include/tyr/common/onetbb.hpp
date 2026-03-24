

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

#ifndef TYR_COMMON_ONETBB_HPP_
#define TYR_COMMON_ONETBB_HPP_

#include <memory>
#include <oneapi/tbb/info.h>
#include <oneapi/tbb/task_arena.h>
#include <stdexcept>

namespace tyr
{

struct ExecutionContext
{
    using uint_t = std::size_t;

    explicit ExecutionContext(uint_t num_threads) : m_num_threads(num_threads), m_arena(static_cast<int>(num_threads))
    {
        if (num_threads == 0)
        {
            throw std::invalid_argument("num_threads must be at least 1.");
        }
        m_arena.initialize();
    }

    static std::shared_ptr<ExecutionContext> create(uint_t num_threads) { return std::make_shared<ExecutionContext>(num_threads); }

    uint_t get_num_threads() const noexcept { return m_num_threads; }

    oneapi::tbb::task_arena& arena() noexcept { return m_arena; }

    const oneapi::tbb::task_arena& arena() const noexcept { return m_arena; }

private:
    uint_t m_num_threads;
    oneapi::tbb::task_arena m_arena;
};

}

#endif
