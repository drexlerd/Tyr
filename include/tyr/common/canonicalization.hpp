

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

#ifndef TYR_COMMON_CANONICALIZATION_HPP_
#define TYR_COMMON_CANONICALIZATION_HPP_

#include "tyr/common/comparators.hpp"
#include "tyr/common/types.hpp"

#include <algorithm>

namespace tyr
{

template<typename T>
bool is_canonical(const IndexList<T>& list)
{
    return std::is_sorted(list.begin(), list.end());
}

template<typename T>
bool is_canonical(const DataList<T>& list)
{
    return std::is_sorted(list.begin(), list.end());
}

template<typename T>
bool is_canonical(const ::cista::optional<T>& element)
{
    return true;
}

template<typename T>
void canonicalize(IndexList<T>& list)
{
    if (!is_canonical(list))
        std::sort(list.begin(), list.end());

    list.erase(std::unique(list.begin(), list.end()), list.end());
}

template<typename T>
void canonicalize(DataList<T>& list)
{
    if (!is_canonical(list))
        std::sort(list.begin(), list.end());

    list.erase(std::unique(list.begin(), list.end()), list.end());
}

template<typename T>
void canonicalize(::cista::optional<T>& element)
{
}

}

#endif
