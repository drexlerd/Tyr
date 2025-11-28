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

#ifndef TYR_FORMALISM_BOOLEAN_OPERATOR_UTILS_HPP_
#define TYR_FORMALISM_BOOLEAN_OPERATOR_UTILS_HPP_

#include "tyr/common/closed_interval.hpp"
#include "tyr/common/declarations.hpp"
#include "tyr/common/types.hpp"
#include "tyr/formalism/declarations.hpp"

namespace tyr::formalism
{

template<typename T>
inline bool apply(OpEq, T lhs, T rhs)
{
    return lhs == rhs;
}

template<typename T>
inline bool apply(OpNe, T lhs, T rhs)
{
    return lhs != rhs;
}

template<typename T>
inline bool apply(OpGe, T lhs, T rhs)
{
    return lhs >= rhs;
}

template<typename T>
inline bool apply(OpGt, T lhs, T rhs)
{
    return lhs > rhs;
}

template<typename T>
inline bool apply(OpLe, T lhs, T rhs)
{
    return lhs <= rhs;
}

template<typename T>
inline bool apply(OpLt, T lhs, T rhs)
{
    return lhs < rhs;
}

/**
 * Existential
 */

template<IsFloatingPoint A>
inline bool apply_existential(OpEq, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    // ∃ x ∈ lhs, ∃ y ∈ rhs : x = y.
    return lower(lhs) <= upper(rhs) && upper(lhs) >= lower(rhs);
}

template<IsFloatingPoint A>
inline bool apply_existential(OpNe, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    // ∃ x ∈ lhs, ∃ y ∈ rhs : x ≠ y.
    const bool lhs_is_point = lower(lhs) == upper(lhs);
    const bool rhs_is_point = lower(rhs) == upper(rhs);
    if (lhs_is_point && rhs_is_point)
        return lower(lhs) != lower(rhs);
    return true;
}

template<IsFloatingPoint A>
inline bool apply_existential(OpGe, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    // ∃ x ∈ lhs, ∃ y ∈ rhs : x >= y.
    return upper(lhs) >= lower(rhs);
}

template<IsFloatingPoint A>
inline bool apply_existential(OpGt, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    // ∃ x ∈ lhs, ∃ y ∈ rhs : x > y.
    return upper(lhs) > lower(rhs);
}

template<IsFloatingPoint A>
inline bool apply_existential(OpLe, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    // ∃ x ∈ lhs, ∃ y ∈ rhs : x <= y.
    return lower(lhs) <= upper(rhs);
}

template<IsFloatingPoint A>
inline bool apply_existential(OpLt, const ClosedInterval<A>& lhs, const ClosedInterval<A>& rhs)
{
    if (empty(lhs) || empty(rhs))
        return false;

    // ∃ x ∈ lhs, ∃ y ∈ rhs : x < y.
    return lower(lhs) < upper(rhs);
}
}

#endif
