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
 *<
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TYR_COMMON_UNORDERED_SET_HPP_
#define TYR_COMMON_UNORDERED_SET_HPP_

#include "tyr/common/declarations.hpp"

#include <unordered_set>

namespace tyr
{
template<typename T, typename H, typename E>
void intersect_inplace(std::unordered_set<T, H, E>& target, const std::unordered_set<T, H, E>& other)
{
    for (auto it = target.begin(); it != target.end();)
    {
        if (!other.contains(*it))
            it = target.erase(it);
        else
            ++it;
    }
}

template<typename T, typename H, typename E>
void union_inplace(std::unordered_set<T, H, E>& target, const std::unordered_set<T, H, E>& other)
{
    target.insert(other.begin(), other.end());
}
}

#endif