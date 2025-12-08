/*
 * Copyright (C) 2023 Dominik Drexler and Simon Stahlberg
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
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_ITERTOOLS_HPP_
#define TYR_COMMON_ITERTOOLS_HPP_

#include <concepts>
#include <iostream>
#include <iterator>
#include <ranges>
#include <unordered_set>
#include <vector>

namespace tyr
{

/**
 * Cartesian set.
 */

template<class OuterRandomIt, class InnerRange = typename std::iterator_traits<OuterRandomIt>::value_type, class T = typename InnerRange::value_type, class F>
    requires std::random_access_iterator<OuterRandomIt> && std::ranges::random_access_range<InnerRange> && std::invocable<F, const std::vector<T>&>
void for_element_in_cartesian_set(OuterRandomIt first, OuterRandomIt last, F&& callback)
{
    const size_t n = last - first;

    thread_local std::vector<T> tmp;
    tmp.resize(n);

    if (n == 0)
    {
        callback(tmp);  // empty element for empty range
        return;
    }

    if (std::any_of(first, last, [](auto&& inner) { return std::distance(inner.begin(), inner.end()) == 0; }))
        return;

    thread_local std::vector<size_t> indices;

    indices.resize(n);
    indices.assign(n, 0);

    while (true)
    {
        // Emit current tuple
        for (size_t i = 0; i < n; ++i)
            tmp[i] = first[i][indices[i]];

        callback(tmp);

        // Mixed-radix increment (odometer)
        size_t pos = n - 1;
        while (true)
        {
            ++indices[pos];
            if (indices[pos] < first[pos].size())
                break;

            indices[pos] = 0;
            if (pos == 0)
                return;  // fully done

            --pos;
        }
    }
}

}

#endif
